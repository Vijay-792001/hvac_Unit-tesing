/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

/* External handle(s) per source code */
TIM_HandleTypeDef htim3;
GPIO_TypeDef GPIOB_inst;
#define CH1 TIM_CHANNEL_1

void setUp(void)
{
    // Reset or clear any static/global state if necessary
    mock_stm32f4xx_hal_Init();
    mock_position_sensing_Init();
    // The implementation may rely on static variables;
    // If needed, call MotorController_Abort() to reset to STOPPED.
    MotorController_Abort();
}

void tearDown(void)
{
    mock_stm32f4xx_hal_Verify();
    mock_position_sensing_Verify();
}

/* === MC_01: Init sets safe state and starts PWM === */
void test_MotorController_Init_sets_safe_state_and_starts_PWM_MC_01(void)
{
    // GPIO Init expectation for both pins
    GPIO_InitTypeDef expectedInit = {0};
    expectedInit.Pin   = GPIO_PIN_0 | GPIO_PIN_1;
    expectedInit.Mode  = GPIO_MODE_OUTPUT_PP;
    expectedInit.Pull  = GPIO_NOPULL;
    expectedInit.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init_Expect(GPIOB, &expectedInit);

    // Safe outputs
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

    // PWM start
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    // Call
    MotorController_Init();

    // State checks
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());

    // Target must be 0 and returned correctly when not moving
    uint8_t tgt = 127;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL_UINT8(0, tgt);
}

/* === MC_02: Move forward when current < target === */
void test_MotorController_MoveTo_moves_forward_when_current_lt_target_MC_02(void)
{
    uint8_t fake_current = 1;
    // GetPosition returns valid, current < target
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&fake_current);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    // After MoveTo, should be moving FWD
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* === MC_03: Move reverse when current > target === */
void test_MotorController_MoveTo_moves_reverse_when_current_gt_target_MC_03(void)
{
    uint8_t fake_current = 5;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&fake_current);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* === MC_04: No movement when already at target (Abort) === */
void test_MotorController_MoveTo_aborts_if_at_target_MC_04(void)
{
    uint8_t fake_current = 3;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&fake_current);

    // Abort expected: pins reset, PWM stopped
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(3);

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* === MC_05: Abort if current position is invalid at start of MoveTo === */
void test_MotorController_MoveTo_aborts_if_position_invalid_MC_05(void)
{
    uint8_t dontcare = 99;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0); // returns invalid

    // Abort expected: pins reset, PWM stopped
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(5);

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* === MC_06: Update does nothing when motor not moving === */
void test_MotorController_Update_does_nothing_if_not_moving_MC_06(void)
{
    // Ensure stopped/not moving state
    MotorController_Abort();
    // No calls to PositionSensing_* or GPIO/TIM expected in Update.

    MotorController_Update();

    // Still not moving
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* === MC_07: Update aborts if GetPosition returns invalid while moving === */
void test_MotorController_Update_aborts_if_position_invalid_while_moving_MC_07(void)
{
    // Move first, then simulate Update
    uint8_t current = 2;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&current);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);

    // Update: PositionSensing_Update called, then GetPosition returns invalid
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    // On abort
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* === MC_08: Update aborts if IsAtTarget returns 1 while moving === */
void test_MotorController_Update_aborts_if_IsAtTarget_returns_1_MC_08(void)
{
    // Move first, then simulate Update at target
    uint8_t current = 2;
    uint8_t target = 4;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&current);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(target);

    // Update: PositionSensing_Update called, GetPosition valid, IsAtTarget==1
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&current);
    PositionSensing_IsAtTarget_ExpectAndReturn(target, 1);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* === MC_09: Update continues moving if not at target and position valid === */
void test_MotorController_Update_continues_moving_if_not_at_target_MC_09(void)
{
    // Move first, then simulate Update not yet at target
    uint8_t current = 2;
    uint8_t target = 4;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&current);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(target);

    // Update: PositionSensing_Update called, GetPosition valid, IsAtTarget==0
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&current);
    PositionSensing_IsAtTarget_ExpectAndReturn(target, 0);

    MotorController_Update();

    // No abort, should still be moving
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* === MC_10: Abort sets safe outputs and stops PWM === */
void test_MotorController_Abort_sets_safe_outputs_and_stops_PWM_MC_10(void)
{
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Abort();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* === MC_11: GetTarget returns value when stopped and target < 6, pointer valid === */
void test_MotorController_GetTarget_returns_target_when_stopped_and_valid_MC_11(void)
{
    // Set state: stopped, s_target_position to 2
    MotorController_Abort();

    // Move to 2, position valid but don't actually move (simulate abort)
    uint8_t fake_current = 2;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&fake_current);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(2);

    uint8_t tgt = 0;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL_UINT8(2, tgt);
}

/* === MC_12: GetTarget returns 0 when moving === */
void test_MotorController_GetTarget_returns_0_when_moving_MC_12(void)
{
    // Start a move so movement_active=1
    uint8_t fake_current = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&fake_current);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);

    uint8_t tgt = 42;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}

/* === MC_13: GetTarget returns 0 on NULL pointer === */
void test_MotorController_GetTarget_returns_0_on_null_pointer_MC_13(void)
{
    MotorController_Abort();
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(NULL));
}

/* === MC_14: GetTarget returns 0 when stored target is out of range (>=6) === */
void test_MotorController_GetTarget_returns_0_when_target_out_of_range_MC_14(void)
{
    // Simulate out-of-range by calling MoveTo(6) with invalid position to abort but leave s_target_position=6
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(6);

    uint8_t tgt = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}
