/* ===== test_motor_controller.c ===== */

#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

// ===== Fake externals stub =====
TIM_HandleTypeDef htim3;
GPIO_TypeDef GPIOB_inst;
GPIO_TypeDef *GPIOB = &GPIOB_inst;

// ===== Helper: Reset static state (not visible, so use API) =====
static void MotorController_ResetInternalState(void)
{
    // There is no direct way to reset, so call Init.
    MotorController_Init();
}

// ===== Test Lifecycle Hooks =====

void setUp(void)
{
    // Always reset static state before each test to ensure clean environment.
    MotorController_ResetInternalState();
}

void tearDown(void)
{
    // Nothing
}

// MC_01: Init sets safe state and starts PWM
void test_MC_01_MotorController_Init_sets_safe_state_and_starts_PWM(void)
{
    // HAL_GPIO_Init just needs to be called; don't validate arguments
    HAL_GPIO_Init_Expect(GPIOB, (GPIO_InitTypeDef *)UnityAnyPtr);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Init();

    // Check public API for stopped state and correct target
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());

    uint8_t t = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&t));
    TEST_ASSERT_EQUAL_UINT8(0, t);
}

// MC_02: Move forward when current < target (forward)
void test_MC_02_MoveTo_moves_forward_when_current_less_than_target(void)
{
    // current=1, target=4
    uint8_t fake_current = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&fake_current, 1);

    // Should set FWD=RESET, REV=SET
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    // PWM start
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
}

// MC_03: Move reverse when current > target (reverse)
void test_MC_03_MoveTo_moves_reverse_when_current_greater_than_target(void)
{
    // current=5, target=2
    uint8_t fake_current = 5;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&fake_current, 1);

    // Should set FWD=SET, REV=RESET
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    // PWM start
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
}

// MC_04: No movement when already at target (Abort)
void test_MC_04_MoveTo_does_not_move_when_already_at_target_calls_abort(void)
{
    // current=3, target=3
    uint8_t fake_current = 3;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&fake_current, 1);

    // Should call abort: GPIO both reset + PWM stop
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(3);

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

// MC_05: Abort if current position is invalid (at start of MoveTo)
void test_MC_05_MoveTo_aborts_if_GetPosition_returns_invalid(void)
{
    // GetPosition fails (returns 0); no current value set
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    // Should call abort: GPIO both reset + PWM stop
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(1); // any target

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

// MC_06: Update does nothing if not moving
void test_MC_06_Update_does_nothing_when_not_moving(void)
{
    // Should NOT call PositionSensing_Update or any HAL/position_sensing method
    // s_movement_active == 0 after Init

    MotorController_Update();

    // Check that state remains stopped, not moving
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

// MC_07: While moving, if GetPosition becomes invalid, abort
void test_MC_07_Update_aborts_when_position_becomes_invalid_while_moving(void)
{
    // Start moving (simulate s_movement_active = 1 by MoveTo)
    uint8_t fake_current = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&fake_current, 1);

    // FWD, REV, PWM
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    // Now update, PositionSensing_Update and then GetPosition returns 0 (invalid)
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    // Should abort: GPIO both reset + PWM stop
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

// MC_08: While moving: stop when target reached using IsAtTarget
void test_MC_08_Update_aborts_when_IsAtTarget_returns_1(void)
{
    // Start moving (simulate movement to target=4)
    uint8_t fake_current = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&fake_current, 1);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    // Now update: PositionSensing_Update, GetPosition: valid, IsAtTarget=1
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);

    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);

    // Abort: GPIO both reset + PWM stop
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

// MC_09: While moving: continue moving if not at target
void test_MC_09_Update_continues_moving_if_not_at_target(void)
{
    // Start moving
    uint8_t fake_current = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&fake_current, 1);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    // Now update: PositionSensing_Update, GetPosition valid, IsAtTarget=0
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_IsAtTarget_ExpectAndReturn(2, 0);

    // No abort, so no more HAL_GPIO_WritePin or HAL_TIM_PWM_Stop calls

    MotorController_Update();

    // Still moving
    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
}

// MC_10: Abort sets safe outputs and stops PWM
void test_MC_10_Abort_sets_safe_outputs_and_stops_PWM(void)
{
    // Might be moving or stopped – should always set both pins reset and stop PWM
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Abort();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

// MC_11: GetTarget returns target when stopped, target<6, and pointer valid
void test_MC_11_GetTarget_returns_1_and_target_when_stopped_and_target_valid(void)
{
    // Move to a valid target, then abort so motor is stopped
    uint8_t fake_current = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&fake_current, 1);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    // Abort to stop
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Abort();

    uint8_t target = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&target));
    TEST_ASSERT_EQUAL_UINT8(2, target);
}

// MC_12: GetTarget returns 0 when moving
void test_MC_12_GetTarget_returns_0_when_motor_moving(void)
{
    // MoveTo but don't abort, so motor is still moving
    uint8_t fake_current = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&fake_current, 1);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    uint8_t target = 0xAA;
    // Should return 0 when moving
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&target));
}

// MC_13: GetTarget returns 0 on NULL pointer
void test_MC_13_GetTarget_returns_0_on_null_pointer(void)
{
    // Ensure motor is stopped (call abort to ensure state)
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Abort();

    // NULL pointer should return 0 (nothing to dereference)
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(NULL));
}

// MC_14: GetTarget returns 0 when stored target is out of range (>=6)
void test_MC_14_GetTarget_returns_0_when_stored_target_out_of_range(void)
{
    // Set up: move to invalid target >= 6, but GetPosition must be invalid to allow this value
    //  -> In the API, MoveTo sets s_target_position = target. If GetPosition fails, target is not reset.
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0); // Simulate invalid position

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(6);

    // Now stopped, but target_position = 6 >= 6
    uint8_t t = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&t));
}

