/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"
// Global handles needed as extern by production code
TIM_HandleTypeDef htim3;
GPIO_TypeDef GPIOB_inst;
GPIO_TypeDef *GPIOB = &GPIOB_inst;

void setUp(void) { MotorController_Init(); }
void tearDown(void) {}
// MC_01: Init sets safe state and starts PWM
void test_MC_01_Init_sets_safe_state_and_starts_PWM(void)
{
    HAL_GPIO_Init_Expect(GPIOB, &((GPIO_InitTypeDef){
        .Pin = GPIO_PIN_0 | GPIO_PIN_1,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW
    }));
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Init();
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_FALSE(MotorController_IsMoving());
    uint8_t t = 99;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&t));
    TEST_ASSERT_EQUAL_UINT8(0, t);
}
// MC_02: Move forward when current < target
void test_MC_02_Move_forward_when_current_less_than_target(void)
{
    uint8_t pos = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&pos);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
    TEST_ASSERT_TRUE(MotorController_IsMoving());
}
// MC_03: Move reverse when current > target
void test_MC_03_Move_reverse_when_current_greater_than_target(void)
{
    uint8_t pos = 5;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&pos);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(2);
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
    TEST_ASSERT_TRUE(MotorController_IsMoving());
}
// MC_04: No movement when already at target (Abort)
void test_MC_04_No_movement_when_at_target_Abort(void)
{
    uint8_t pos = 3;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&pos);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(3);
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_FALSE(MotorController_IsMoving());
}
// MC_05: Abort if current position invalid at MoveTo
void test_MC_05_Abort_if_position_invalid_on_MoveTo(void)
{
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(2);
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_FALSE(MotorController_IsMoving());
}
// MC_06: Update does nothing when not moving
void test_MC_06_Update_does_nothing_when_motor_not_moving(void)
{
    MotorController_Update();
    TEST_ASSERT_FALSE(MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}
// MC_07: While motor moving: if GetPosition becomes invalid -> Abort
void test_MC_07_Abort_on_invalid_position_during_movement(void)
{
    uint8_t pos = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&pos);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(3);
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Update();
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_FALSE(MotorController_IsMoving());
}
// MC_08: Stop when target reached (IsAtTarget=1) during movement
void test_MC_08_Stop_when_target_reached_via_IsAtTarget(void)
{
    uint8_t pos = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&pos);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Update();
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_FALSE(MotorController_IsMoving());
}
// MC_09: Continue moving if not at target (IsAtTarget=0)
void test_MC_09_Continue_moving_if_not_at_target(void)
{
    uint8_t pos = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&pos);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 0);
    MotorController_Update();
    TEST_ASSERT_TRUE(MotorController_IsMoving());
}
// MC_10: Abort sets safe outputs and stops PWM
void test_MC_10_Abort_sets_safe_outputs_and_stops_PWM(void)
{
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Abort();
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_FALSE(MotorController_IsMoving());
}
// MC_11: GetTarget returns target when stopped and valid and ptr valid
void test_MC_11_GetTarget_returns_target_when_stopped_and_target_valid(void)
{
    uint8_t pos = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&pos);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(2);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Abort();
    uint8_t t = 0xFF;
    int result = MotorController_GetTarget(&t);
    TEST_ASSERT_EQUAL(1, result);
    TEST_ASSERT_EQUAL_UINT8(2, t);
}
// MC_12: GetTarget returns 0 when moving
void test_MC_12_GetTarget_returns_0_when_moving(void)
{
    uint8_t pos = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&pos);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(3);
    uint8_t t = 0xAA;
    int result = MotorController_GetTarget(&t);
    TEST_ASSERT_EQUAL(0, result);
}
// MC_13: GetTarget returns 0 on NULL pointer
void test_MC_13_GetTarget_returns_0_on_NULL_pointer(void)
{
    int result = MotorController_GetTarget(NULL);
    TEST_ASSERT_EQUAL(0, result);
}
// MC_14: GetTarget returns 0 if stored target is out of range
void test_MC_14_GetTarget_returns_0_on_target_out_of_range(void)
{
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(6);
    uint8_t t = 0x22;
    int result = MotorController_GetTarget(&t);
    TEST_ASSERT_EQUAL(0, result);
}
// End of test_motor_controller.c
