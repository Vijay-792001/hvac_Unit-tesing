/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_position_sensing.h"
#include "mock_stm32f4xx_hal.h"

static TIM_HandleTypeDef dummy_htim3;
extern TIM_HandleTypeDef htim3;

#define TEST_MOTOR_PORT GPIOB
#define TEST_MOTOR_PIN_DIR_FWD GPIO_PIN_0
#define TEST_MOTOR_PIN_DIR_REV GPIO_PIN_1
#define TEST_PWM_CHANNEL TIM_CHANNEL_1

void setUp(void)
{
    htim3 = dummy_htim3;
}

void tearDown(void)
{
}

/* MC_01: MotorController_Init sets safe state and starts PWM */
void test_MC_01_MotorController_Init_sets_safe_state_and_starts_PWM(void)
{
    HAL_GPIO_Init_Ignore(); // Configuration sequence, not checked
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TEST_PWM_CHANNEL);

    MotorController_Init();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());

    uint8_t tgt = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL_UINT8(0, tgt);
}

/* MC_02: Move forward when current < target */
void test_MC_02_Move_forward_when_current_lt_target(void)
{
    uint8_t fake_pos = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&fake_pos);

    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_REV, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TEST_PWM_CHANNEL);

    MotorController_MoveTo(4);

    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
}

/* MC_03: Move reverse when current > target */
void test_MC_03_Move_reverse_when_current_gt_target(void)
{
    uint8_t fake_pos = 5;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&fake_pos);

    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_FWD, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TEST_PWM_CHANNEL);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
}

/* MC_04: No movement when already at target (Abort) */
void test_MC_04_No_movement_when_already_at_target(void)
{
    uint8_t fake_pos = 3;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&fake_pos);

    /* Should abort when current == target */
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TEST_PWM_CHANNEL);

    MotorController_MoveTo(3);

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_05: Abort if current position is invalid at start of MoveTo */
void test_MC_05_Abort_when_GetPosition_fails_on_MoveTo(void)
{
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TEST_PWM_CHANNEL);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_06: Update does nothing when motor not moving */
void test_MC_06_Update_does_nothing_when_motor_not_moving(void)
{
    // Setup initial state: not moving
    MotorController_Abort();

    /* No mocks should be called => Just invoke update and check status */
    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_07: Abort during update if GetPosition fails (while moving) */
void test_MC_07_Abort_if_GetPosition_invalid_during_update(void)
{
    // Start moving first
    uint8_t fake_pos = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&fake_pos);
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_REV, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TEST_PWM_CHANNEL);
    MotorController_MoveTo(2);

    // Now set up update with GetPosition failing, should abort
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TEST_PWM_CHANNEL);

    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_08: Abort during update if IsAtTarget returns true (while moving) */
void test_MC_08_Abort_if_IsAtTarget_true_during_update(void)
{
    // Start moving first
    uint8_t fake_pos = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&fake_pos);
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_REV, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TEST_PWM_CHANNEL);
    MotorController_MoveTo(4);

    // Update: should see valid position and IsAtTarget true, triggers abort
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);

    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TEST_PWM_CHANNEL);

    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_09: Continue moving on update if not at target */
void test_MC_09_Continue_moving_on_update_if_not_at_target(void)
{
    // Start moving
    uint8_t fake_pos = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&fake_pos);
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_REV, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TEST_PWM_CHANNEL);
    MotorController_MoveTo(4);

    // Update, valid position, IsAtTarget returns false, should NOT abort
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 0);

    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* MC_10: Abort sets safe outputs and stops PWM */
void test_MC_10_Abort_sets_safe_output_and_stops_PWM(void)
{
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TEST_PWM_CHANNEL);

    MotorController_Abort();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_11: GetTarget returns target when stopped, target < 6, and pointer valid */
void test_MC_11_GetTarget_returns_target_when_stopped_and_valid(void)
{
    // Ensure stopped state and set a valid target
    MotorController_Abort();
    MotorController_MoveTo(2);
    MotorController_Abort();

    uint8_t tgt = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL_UINT8(2, tgt);
}

/* MC_12: GetTarget returns 0 when moving */
void test_MC_12_GetTarget_returns_0_when_moving(void)
{
    // Move so motor is moving
    uint8_t fake_pos = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&fake_pos);
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_REV, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TEST_PWM_CHANNEL);
    MotorController_MoveTo(4);

    uint8_t tgt = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}

/* MC_13: GetTarget returns 0 on NULL pointer */
void test_MC_13_GetTarget_returns_0_on_NULL_pointer(void)
{
    MotorController_Abort();
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(NULL));
}

/* MC_14: GetTarget returns 0 when stored target >= 6 */
void test_MC_14_GetTarget_returns_0_when_stored_target_ge_6(void)
{
    // Manually set target out of range: MoveTo(6) with invalid position will set s_target_position=6 and abort due to position fail
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(TEST_MOTOR_PORT, TEST_MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TEST_PWM_CHANNEL);

    MotorController_MoveTo(6);

    uint8_t tgt = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}
