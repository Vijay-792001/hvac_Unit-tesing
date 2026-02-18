/* ===== test_motor_controller.c ===== */

#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

// Global handles required by the module under test
TIM_HandleTypeDef htim3;
GPIO_TypeDef GPIOB_inst;
GPIO_TypeDef *GPIOB = &GPIOB_inst;

// Helper for test isolation: local access to module static state via API where possible
void setUp(void)
{
    mock_position_sensing_Init();
    mock_stm32f4xx_hal_Init();
    // Always re-init for clean state; MC_01 always covers this code and sets known state
    MotorController_Init();
}
void tearDown(void)
{
    mock_position_sensing_Verify();
    mock_position_sensing_Destroy();
    mock_stm32f4xx_hal_Verify();
    mock_stm32f4xx_hal_Destroy();
}

/* MC_01: Init sets safe state and starts PWM */
void test_MC_01_Init_sets_safe_state_and_starts_PWM(void)
{
    // GPIO_InitStruct always used, HAL_GPIO_Init always called
    HAL_GPIO_Init_Expect(GPIOB, (GPIO_InitTypeDef *)&((GPIO_InitTypeDef){
        .Pin = 0x3, // GPIO_PIN_0 | GPIO_PIN_1
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW}));

    // All pins safe (RESET), PWM started
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Init();

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());

    uint8_t target = 0xFF;
    uint8_t r = MotorController_GetTarget(&target);
    TEST_ASSERT_EQUAL(1, r);
    TEST_ASSERT_EQUAL(0, target);
}

/* MC_02: Move forward when current < target */
void test_MC_02_Move_forward_when_current_lt_target(void)
{
    // target=4, current=1
    uint8_t pos = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&pos, 1);

    // Expect GPIOs: FWD RESET, REV SET; State: MOVING_FWD, PWM started
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); // FWD
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);   // REV
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
}

/* MC_03: Move reverse when current > target */
void test_MC_03_Move_reverse_when_current_gt_target(void)
{
    // target=2, current=5
    uint8_t pos = 5;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&pos, 1);

    // Expect GPIOs: FWD SET, REV RESET; State: MOVING_REV, PWM started
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);     // FWD
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);   // REV
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
}

/* MC_04: No movement when already at target (Abort) */
void test_MC_04_MoveTo_aborts_when_already_at_target(void)
{
    // target=3, current=3
    uint8_t pos = 3;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&pos, 1);

    // Expects Abort sequence: FWD RESET, REV RESET, PWM stop
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(3);

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_05: Abort if current position is invalid at start of MoveTo */
void test_MC_05_Abort_when_PositionSensing_returns_invalid_in_MoveTo(void)
{
    // target=2, GetPosition fails (returns 0)
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_06: Update does nothing when motor not moving */
void test_MC_06_Update_does_nothing_when_not_moving(void)
{
    // Ensure we start from STOPPED
    MotorController_Abort();

    // No PositionSensing/hal calls expected
    MotorController_Update();

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_07: While moving, if GetPosition becomes invalid, Abort */
void test_MC_07_Update_aborts_when_GetPosition_invalid_while_moving(void)
{
    // Setup as moving
    uint8_t pos = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(2);

    // Now, set update to invalid
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_08: While moving, stop when target reached (IsAtTarget=1) */
void test_MC_08_Update_aborts_when_IsAtTarget(void)
{
    // Setup as moving
    uint8_t pos = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);

    // Now, simulate update where target is reached
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&pos, 1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_09: While moving, continue if not at target (IsAtTarget=0) */
void test_MC_09_Update_continues_when_not_at_target(void)
{
    // Setup as moving
    uint8_t pos = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);

    // Now, simulate update NOT at target
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&pos, 1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 0);

    MotorController_Update();

    // Movement continues (no abort called)
    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
}

/* MC_10: Abort sets safe outputs and stops PWM */
void test_MC_10_Abort_sets_safe_outputs_and_stops_PWM(void)
{
    // Make sure movement_active != 0
    uint8_t pos = 0;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(1);

    // Now call Abort
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Abort();

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

/* MC_11: GetTarget returns target and 1 when stopped, target<6, ptr valid */
void test_MC_11_GetTarget_returns_stored_target_when_stopped_ptr_valid_target_lt6(void)
{
    // Move to set target, then abort to stop
    uint8_t pos = 3;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&pos, 1);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4); // sets s_target_position=4

    // Abort so we are stopped
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Abort();

    uint8_t t = 0xFF;
    int r = MotorController_GetTarget(&t);
    TEST_ASSERT_EQUAL(1, r);
    TEST_ASSERT_EQUAL(4, t);
}

/* MC_12: GetTarget returns 0 when moving */
void test_MC_12_GetTarget_returns_0_when_moving(void)
{
    // Setup movement
    uint8_t pos = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);

    uint8_t t = 0xEE;
    int r = MotorController_GetTarget(&t);
    TEST_ASSERT_EQUAL(0, r);
}

/* MC_13: GetTarget returns 0 on NULL pointer */
void test_MC_13_GetTarget_returns_0_if_ptr_null(void)
{
    // In stopped state after init
    int r = MotorController_GetTarget(NULL);
    TEST_ASSERT_EQUAL(0, r);
}

/* MC_14: GetTarget returns 0 when stored target is out of range */
void test_MC_14_GetTarget_returns_0_when_target_stored_out_of_range(void)
{
    // Move to a target >= 6, position invalid so s_target_position stays >=6
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    // MoveTo will call abort (no movement started, but s_target_position is set)
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(6);

    uint8_t tgt = 0xAB;
    int r = MotorController_GetTarget(&tgt);
    TEST_ASSERT_EQUAL(0, r);
}

/* ===== end of test_motor_controller.c ===== */
