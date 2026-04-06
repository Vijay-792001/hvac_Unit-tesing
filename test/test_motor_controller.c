/* ===== test_motor_controller.c ===== */

#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

TIM_HandleTypeDef htim3_inst;
GPIO_TypeDef GPIOB_inst;

void setUp(void)
{
    // Nothing needed
}

void tearDown(void)
{
    // Nothing needed
}

/* MC_01: Init sets safe state and starts PWM */
void test_MC_01_MotorController_Init_sets_safe_state_and_starts_PWM(void)
{
    GPIO_InitTypeDef expected_init_struct = {
        .Pin = GPIO_PIN_0 | GPIO_PIN_1,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW
    };

    // Expects
    HAL_GPIO_Init_Expect(&GPIOB_inst, &expected_init_struct);
    HAL_GPIO_WritePin_Expect(&GPIOB_inst, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOB_inst, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3_inst, TIM_CHANNEL_1);

    // Act
    MotorController_Init();

    // Test state get functions/post-conditions
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    uint8_t tgt = 0xFF;
    int gt_res = MotorController_GetTarget(&tgt);
    TEST_ASSERT_EQUAL(1, gt_res);
    TEST_ASSERT_EQUAL_UINT8(0, tgt);
}

/* MC_02: Move forward when current < target */
void test_MC_02_Move_forward_when_current_lt_target(void)
{
    // Init expects
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    // Set current=1, target=4
    uint8_t dummy_current = 1;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&dummy_current);
    HAL_GPIO_WritePin_Expect(&GPIOB_inst, GPIO_PIN_0, GPIO_PIN_RESET); // FWD - RESET
    HAL_GPIO_WritePin_Expect(&GPIOB_inst, GPIO_PIN_1, GPIO_PIN_SET);   // REV - SET
    HAL_TIM_PWM_Start_Expect(&htim3_inst, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* MC_03: Move reverse when current > target */
void test_MC_03_Move_reverse_when_current_gt_target(void)
{
    // Init expects
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    uint8_t dummy_current = 5;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&dummy_current);

    HAL_GPIO_WritePin_Expect(&GPIOB_inst, GPIO_PIN_0, GPIO_PIN_SET);    // FWD - SET
    HAL_GPIO_WritePin_Expect(&GPIOB_inst, GPIO_PIN_1, GPIO_PIN_RESET);  // REV - RESET
    HAL_TIM_PWM_Start_Expect(&htim3_inst, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* MC_04: No movement when already at target (Abort) */
void test_MC_04_No_movement_when_already_at_target(void)
{
    // current = 3, target = 3 (should Abort)
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    uint8_t dummy_current = 3;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&dummy_current);

    /* Expect abort safe sequence */
    HAL_GPIO_WritePin_Expect(&GPIOB_inst, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOB_inst, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3_inst, TIM_CHANNEL_1);

    MotorController_MoveTo(3);

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_05: Abort if current position is invalid at start of MoveTo */
void test_MC_05_Abort_if_invalid_position_on_MoveTo_start(void)
{
    // PositionSensing_GetPosition returns 0 (invalid)
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    HAL_GPIO_WritePin_Expect(&GPIOB_inst, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOB_inst, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3_inst, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_06: Update does nothing when motor not moving */
void test_MC_06_Update_does_nothing_when_motor_not_moving(void)
{
    // Arrange: ensure not moving
    // No calls to position or HAL functions expected!
    MotorController_Abort(); // to set s_movement_active = 0
    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_07: While motor moving, if GetPosition invalid -> Abort */
void test_MC_07_Update_aborts_if_position_invalid_while_moving(void)
{
    // Ready the system in a moving state
    PositionSensing_GetPosition_IgnoreAndReturn(1);
    uint8_t dummy_pos = 1;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&dummy_pos);
    HAL_GPIO_WritePin_Ignore();
    HAL_TIM_PWM_Start_Ignore();

    MotorController_MoveTo(3);

    // Now simulate update: GetPosition returns 0 (invalid)!
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    HAL_GPIO_WritePin_Expect(&GPIOB_inst, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOB_inst, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3_inst, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_08: While moving, stop when at target using IsAtTarget */
void test_MC_08_Update_aborts_when_at_target(void)
{
    // Prepare state
    PositionSensing_GetPosition_IgnoreAndReturn(1);
    uint8_t dummy_pos = 3;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&dummy_pos);
    HAL_GPIO_WritePin_Ignore();
    HAL_TIM_PWM_Start_Ignore();
    MotorController_MoveTo(4); // s_movement_active = 1

    // On update:
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);

    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);

    HAL_GPIO_WritePin_Expect(&GPIOB_inst, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOB_inst, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3_inst, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_09: While moving, continue if not at target */
void test_MC_09_Update_keeps_moving_if_not_at_target(void)
{
    // Prepare state
    PositionSensing_GetPosition_IgnoreAndReturn(1);
    uint8_t dummy_pos = 2;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&dummy_pos);
    HAL_GPIO_WritePin_Ignore();
    HAL_TIM_PWM_Start_Ignore();
    MotorController_MoveTo(4); // s_movement_active = 1

    // On update:
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);

    PositionSensing_IsAtTarget_ExpectAndReturn(4, 0);

    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* MC_10: Abort sets safe outputs and stops PWM */
void test_MC_10_Abort_sets_safe_outputs_and_stops_PWM(void)
{
    HAL_GPIO_WritePin_Expect(&GPIOB_inst, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOB_inst, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3_inst, TIM_CHANNEL_1);

    MotorController_Abort();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_11: GetTarget returns target when stopped and <6 and pointer valid */
void test_MC_11_GetTarget_returns_stored_target_when_idle_and_valid_ptr(void)
{
    // Set to target=2, not moving
    PositionSensing_GetPosition_IgnoreAndReturn(1);
    uint8_t dummy_pos = 1;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&dummy_pos);
    HAL_GPIO_WritePin_Ignore();
    HAL_TIM_PWM_Start_Ignore();
    MotorController_MoveTo(2);

    // Abort to stop motor ("stopped" state, but s_target_position is 2)
    HAL_GPIO_WritePin_Ignore();
    HAL_TIM_PWM_Stop_Ignore();
    MotorController_Abort();

    uint8_t tgt_val = 0xFF;
    int ret = MotorController_GetTarget(&tgt_val);

    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL_UINT8(2, tgt_val);
}

/* MC_12: GetTarget returns 0 when moving */
void test_MC_12_GetTarget_returns_0_when_motor_moving(void)
{
    // Move and do not abort
    PositionSensing_GetPosition_IgnoreAndReturn(1);
    uint8_t dummy_pos = 1;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&dummy_pos);
    HAL_GPIO_WritePin_Ignore();
    HAL_TIM_PWM_Start_Ignore();
    MotorController_MoveTo(2);

    // Should be moving
    uint8_t tgt_val = 0xFE;
    int ret = MotorController_GetTarget(&tgt_val);

    TEST_ASSERT_EQUAL(0, ret);
}

/* MC_13: GetTarget returns 0 on NULL pointer */
void test_MC_13_GetTarget_returns_0_on_NULL_pointer(void)
{
    // Not moving, stopped, any target
    HAL_GPIO_WritePin_Ignore();
    HAL_TIM_PWM_Stop_Ignore();
    MotorController_Abort();

    int ret = MotorController_GetTarget(NULL);

    TEST_ASSERT_EQUAL(0, ret);
}

/* MC_14: GetTarget returns 0 if stored target is >=6 */
void test_MC_14_GetTarget_returns_0_if_stored_target_invalid(void)
{
    // Scenario: call MoveTo(6), PositionSensing_GetPosition valid
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    uint8_t dummy_pos = 1;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&dummy_pos);
    // This will move, setting s_target_position = 6
    HAL_GPIO_WritePin_Expect(&GPIOB_inst, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOB_inst, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3_inst, TIM_CHANNEL_1);

    MotorController_MoveTo(6);

    // Abort to stop motor, but s_target_position remains 6
    HAL_GPIO_WritePin_Ignore();
    HAL_TIM_PWM_Stop_Ignore();
    MotorController_Abort();

    uint8_t tgt_val = 0;
    int ret = MotorController_GetTarget(&tgt_val);

    TEST_ASSERT_EQUAL(0, ret);
}
