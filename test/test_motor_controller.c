/* ===== test_motor_controller.c ===== */

#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

#include <string.h>

/* Provide the external TIM handle used by the module */
TIM_HandleTypeDef htim3;
GPIO_TypeDef GPIOB_inst;

#define RESET_STATE()    do { \
        MotorController_Abort(); \
    } while(0)

void setUp(void)
{
    /* Ensure safe state before each test */
    RESET_STATE();
}

void tearDown(void)
{
    /* Ensure safe state after */
    RESET_STATE();
}

/*
 * MC_01: Init sets safe state and starts PWM
 */
void test_MC_01_Init_sets_safe_state_and_starts_PWM(void)
{
    /* Expect Init sequence: GPIO Init, two WritePin (RESET), PWM Start */
    GPIO_InitTypeDef gpio_last = {0};
    HAL_GPIO_Init_Expect(GPIOB, &gpio_last);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Init();

    /* State checks */
    TEST_ASSERT_EQUAL_UINT8(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());

    uint8_t target = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&target));
    TEST_ASSERT_EQUAL_UINT8(0, target);
}

/*
 * MC_02: Move forward when current < target
 */
void test_MC_02_Move_forward_when_current_less_than_target(void)
{
    MotorController_Init();

    uint8_t current = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&current);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_UINT8(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
}

/*
 * MC_03: Move reverse when current > target
 */
void test_MC_03_Move_reverse_when_current_greater_than_target(void)
{
    MotorController_Init();

    uint8_t current = 5;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&current);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_UINT8(MOTOR_STATE_MOVING_REV, MotorController_GetState());
}

/*
 * MC_04: No movement when already at target (Abort)
 */
void test_MC_04_No_movement_when_already_at_target_should_abort(void)
{
    MotorController_Init();

    uint8_t current = 3;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&current);

    /* Should abort: two GPIO resets + PWM stop */
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(3);

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_UINT8(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/*
 * MC_05: Abort if current position is invalid at start of MoveTo
 */
void test_MC_05_Abort_if_current_position_invalid_at_start_of_MoveTo(void)
{
    MotorController_Init();

    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    /* Should abort: two GPIO resets + PWM stop */
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_UINT8(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/*
 * MC_06: Update does nothing when motor not moving
 */
void test_MC_06_Update_does_nothing_when_motor_not_moving(void)
{
    MotorController_Init();

    /* When not moving, should NOT call PositionSensing or any HAL fn */
    MotorController_Update();

    /* Validate state remains stopped and not moving */
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_UINT8(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/*
 * MC_07: While motor moving, GetPosition invalid --> Abort
 */
void test_MC_07_Abort_update_if_position_becomes_invalid_while_moving(void)
{
    /* Init & start movement */
    MotorController_Init();

    /* Force s_movement_active = 1 */
    /* MoveTo will start movement only if GetPosition returns valid and positions are different */
    uint8_t current = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&current);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    /* Now simulate update, which will abort due to invalid GetPosition */
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_UINT8(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/*
 * MC_08: While motor moving, stop when target reached (IsAtTarget true)
 */
void test_MC_08_Stop_when_target_reached_while_motor_moving(void)
{
    MotorController_Init();

    uint8_t current = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&current);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    /* Now: moving. On update, GetPosition returns valid, IsAtTarget == true --> abort */
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&current);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_UINT8(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/*
 * MC_09: While motor moving, do not stop if not at target
 */
void test_MC_09_Continue_moving_when_not_at_target_in_update(void)
{
    MotorController_Init();

    uint8_t current = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&current);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    /* On update, GetPosition valid, IsAtTarget == 0 => no abort */
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&current);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 0);

    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
    TEST_ASSERT_NOT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/*
 * MC_10: Abort sets safe outputs and stops PWM
 */
void test_MC_10_Abort_sets_safe_outputs_and_stops_PWM(void)
{
    MotorController_Init();

    /* Directly call Abort; expect both pins RESET and PWM stop */
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Abort();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_UINT8(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/*
 * MC_11: GetTarget returns target when stopped and in range
 */
void test_MC_11_GetTarget_ok_when_stopped_and_target_in_range(void)
{
    MotorController_Init();

    /* Set up a target, and stopped, and test GetTarget */
    uint8_t current = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&current);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2); /* sets s_target_position = 2 and s_movement_active = 1 */

    /* Now Abort to set stopped, then test GetTarget */
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Abort();

    uint8_t tgt = 0xEE;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL_UINT8(2, tgt);
}

/*
 * MC_12: GetTarget returns 0 when moving
 */
void test_MC_12_GetTarget_returns_0_when_moving(void)
{
    MotorController_Init();

    uint8_t current = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&current);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    uint8_t tgt = 0xAB;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}

/*
 * MC_13: GetTarget returns 0 on NULL pointer
 */
void test_MC_13_GetTarget_returns_0_on_NULL_pointer(void)
{
    MotorController_Init();

    uint8_t current = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&current);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(NULL));
}

/*
 * MC_14: GetTarget returns 0 when stored target is out of range (>=6)
 */
void test_MC_14_GetTarget_returns_0_when_stored_target_out_of_range(void)
{
    MotorController_Init();

    /* Force target position to 6, but not moving, and call GetTarget */
    /* Direct state manipulation is not possible, so indirectly cause MoveTo(6) with invalid position */
    /* This should abort and s_target_position == 6 */

    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(6);

    uint8_t t = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&t));
}
