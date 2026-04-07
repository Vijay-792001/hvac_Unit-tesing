/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

extern TIM_HandleTypeDef htim3;

#define DIR_FWD GPIO_PIN_0
#define DIR_REV GPIO_PIN_1

// MC_01: Init sets safe state and starts PWM
void test_MC_01_MotorController_Init_sets_safe_state_and_starts_PWM(void)
{
    GPIO_InitTypeDef dummy_init;
    HAL_GPIO_Init_Expect(GPIOB, &dummy_init); // This will be called, but struct contents not used by Unity/CMock here
    HAL_GPIO_WritePin_Expect(GPIOB, DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Init();
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    uint8_t tgt = 0xFF;
    TEST_ASSERT_EQUAL(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL_UINT8(0, tgt);
}

// MC_02: Move forward when current < target
void test_MC_02_Move_forward_when_current_lt_target_starts_FWD_PWM(void)
{
    uint8_t current = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&current);
    HAL_GPIO_WritePin_Expect(GPIOB, DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, DIR_REV, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);
    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
}

// MC_03: Move reverse when current > target
void test_MC_03_Move_reverse_when_current_gt_target_starts_REV_PWM(void)
{
    uint8_t current = 5;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&current);
    HAL_GPIO_WritePin_Expect(GPIOB, DIR_FWD, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOB, DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(2);
    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
}

// MC_04: No movement when already at target (Abort)
void test_MC_04_No_movement_when_already_at_target_aborts_and_stops(void)
{
    uint8_t current = 3;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&current);

    // Should abort: sets output safe
    HAL_GPIO_WritePin_Expect(GPIOB, DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(3);
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

// MC_05: Abort if position invalid at start of MoveTo
void test_MC_05_Abort_if_position_invalid_on_MoveTo(void)
{
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    HAL_GPIO_WritePin_Expect(GPIOB, DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

// MC_06: Update does nothing when motor not moving
void test_MC_06_Update_does_nothing_when_motor_not_moving(void)
{
    // No movement_active; update should do nothing: No PositionSensing or HAL calls
    MotorController_Init();
    MotorController_Update();
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

// MC_07: While moving, if GetPosition becomes invalid, abort called
void test_MC_07_Update_aborts_if_GetPosition_invalid_while_moving(void)
{
    // Set up state: start movement active
    // Should call PositionSensing_Update, PositionSensing_GetPosition (returns 0)
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    HAL_GPIO_WritePin_Expect(GPIOB, DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    // Fake movement active flag
    extern void set_motor_movement_active(uint8_t);
    set_motor_movement_active(1);
    MotorController_Update();
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

// MC_08: While moving, stop when target reached using IsAtTarget
void test_MC_08_Update_aborts_when_IsAtTarget_returns_1(void)
{
    PositionSensing_Update_Expect();
    uint8_t dummy = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&dummy);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    extern void set_motor_state_for_update(uint8_t target, uint8_t active);
    set_motor_state_for_update(4, 1);
    MotorController_Update();
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

// MC_09: While moving, continue if not at target
void test_MC_09_MotorController_Update_continues_when_not_at_target(void)
{
    PositionSensing_Update_Expect();
    uint8_t dummy = 2;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&dummy);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 0);
    extern void set_motor_state_for_update(uint8_t target, uint8_t active);
    set_motor_state_for_update(4, 1);
    MotorController_Update();
    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
}

// MC_10: Abort sets safe outputs and stops PWM
void test_MC_10_Abort_stops_PWM_and_safes_GPIO(void)
{
    HAL_GPIO_WritePin_Expect(GPIOB, DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Abort();
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

// MC_11: GetTarget returns target if stopped and ptr valid
void test_MC_11_GetTarget_returns_target_when_stopped_and_ptr_valid(void)
{
    MotorController_Init();
    MotorController_MoveTo(2); // Now stopped state & target=2
    MotorController_Abort();
    uint8_t tgt = 0xFF;
    TEST_ASSERT_EQUAL(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL_UINT8(2, tgt);
}

// MC_12: GetTarget returns 0 when moving
void test_MC_12_GetTarget_returns_0_when_moving(void)
{
    MotorController_Init();
    // Begin "move": active=1
    // Set state to moving, so GetTarget returns 0 [no need for direction]
    extern void set_motor_movement_active(uint8_t);
    set_motor_movement_active(1);
    uint8_t tgt = 0xFF;
    TEST_ASSERT_EQUAL(0, MotorController_GetTarget(&tgt));
}

// MC_13: GetTarget returns 0 on NULL pointer
void test_MC_13_GetTarget_returns_0_on_NULL_ptr(void)
{
    MotorController_Init();
    MotorController_Abort();
    TEST_ASSERT_EQUAL(0, MotorController_GetTarget(NULL));
}

// MC_14: GetTarget returns 0 if stored target out of range (>=6)
void test_MC_14_GetTarget_returns_0_if_target_is_invalid(void)
{
    extern void set_motor_target_value(uint8_t);
    MotorController_Init();
    set_motor_target_value(6);
    MotorController_Abort();
    uint8_t tgt = 0xFF;
    TEST_ASSERT_EQUAL(0, MotorController_GetTarget(&tgt));
}
