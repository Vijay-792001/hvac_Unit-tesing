/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

extern TIM_HandleTypeDef htim3;
extern GPIO_TypeDef GPIOB_inst;
#define MOTOR_GPIO_PORT_DIR   (&GPIOB_inst)
#define MOTOR_PIN_DIR_FWD     GPIO_PIN_0
#define MOTOR_PIN_DIR_REV     GPIO_PIN_1
#define MOTOR_PWM_HANDLE      htim3
#define MOTOR_PWM_CHANNEL     TIM_CHANNEL_1

/* -- Convenient helpers -- */
void expect_direction_fwd(void)
{
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_REV, GPIO_PIN_SET);
}

void expect_direction_rev(void)
{
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_FWD, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
}

void expect_safe_outputs_and_pwm_stop(void)
{
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&MOTOR_PWM_HANDLE, MOTOR_PWM_CHANNEL);
}

void expect_safe_outputs_and_pwm_start(void)
{
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&MOTOR_PWM_HANDLE, MOTOR_PWM_CHANNEL);
}

/* =========================
 * MC_01: Init sets safe state and starts PWM
 * SW-REQ: SWE-REQ-009
 */
void test_MotorController_Init_ShouldSetSafeStateAndStartPwm(void)
{
    HAL_GPIO_Init_Expect(MOTOR_GPIO_PORT_DIR, (GPIO_InitTypeDef*)ANY);
    /* Safe outputs to STOPPED, 0, 0; Both DIRs RESET; PWM started */
    expect_safe_outputs_and_pwm_start();

    MotorController_Init();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetState());
    uint8_t tgt = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL_UINT8(0, tgt);
}

/* =========================
 * MC_02: Move forward when current < target
 * SW-REQ: SWE-REQ-009
 */
void test_MotorController_MoveTo_ShouldMoveForward_WhenCurrentLtTarget(void)
{
    uint8_t dummy = 1; // current position
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos(&dummy);

    expect_direction_fwd();
    HAL_TIM_PWM_Start_Expect(&MOTOR_PWM_HANDLE, MOTOR_PWM_CHANNEL);

    MotorController_MoveTo(4);
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* =========================
 * MC_03: Move reverse when current > target
 * SW-REQ: SWE-REQ-009
 */
void test_MotorController_MoveTo_ShouldMoveReverse_WhenCurrentGtTarget(void)
{
    uint8_t dummy = 5; // current position
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos(&dummy);

    expect_direction_rev();
    HAL_TIM_PWM_Start_Expect(&MOTOR_PWM_HANDLE, MOTOR_PWM_CHANNEL);

    MotorController_MoveTo(2);
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* =========================
 * MC_04: No movement when already at target (Abort)
 * SW-REQ: SWE-REQ-009
 */
void test_MotorController_MoveTo_ShouldAbort_WhenAlreadyAtTarget(void)
{
    uint8_t dummy = 3; // current position
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos(&dummy);

    /* Expect abort sequence */
    expect_safe_outputs_and_pwm_stop();

    MotorController_MoveTo(3);

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* =========================
 * MC_05: Abort if position invalid at start of MoveTo
 * SW-REQ: SWE-REQ-054
 */
void test_MotorController_MoveTo_ShouldAbort_WhenPositionInvalid(void)
{
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    /* Expect abort (GPIOs safe + pwm stop) */
    expect_safe_outputs_and_pwm_stop();

    MotorController_MoveTo(4);
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* =========================
 * MC_06: Update does nothing if motor not moving
 * SW-REQ: SWE-REQ-010
 */
void test_MotorController_Update_ShouldDoNothing_WhenNotMoving(void)
{
    // Manually set stopped state using public abort API (to guarantee inactivity)
    expect_safe_outputs_and_pwm_stop();
    MotorController_Abort();

    // No calls to PositionSensing or HAL expected:
    MotorController_Update();
    // No asserts -- absence of side effect is correctness; state remains stopped
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* =========================
 * MC_07: While moving, update should Abort if GetPosition() becomes invalid
 * SW-REQ: SWE-REQ-010
 */
void test_MotorController_Update_ShouldAbort_WhenPositionBecomesInvalid(void)
{
    // Simulate moving state
    uint8_t dummy = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos(&dummy);
    expect_direction_fwd();
    HAL_TIM_PWM_Start_Expect(&MOTOR_PWM_HANDLE, MOTOR_PWM_CHANNEL);
    MotorController_MoveTo(4);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0); // now returns invalid

    // Update will abort
    expect_safe_outputs_and_pwm_stop();

    MotorController_Update();
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* =========================
 * MC_08: While moving, stop when target reached via IsAtTarget
 * SW-REQ: SWE-REQ-010
 */
void test_MotorController_Update_ShouldAbort_WhenTargetReached(void)
{
    // Simulate moving state
    uint8_t dummy = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos(&dummy);
    expect_direction_fwd();
    HAL_TIM_PWM_Start_Expect(&MOTOR_PWM_HANDLE, MOTOR_PWM_CHANNEL);
    MotorController_MoveTo(4);

    // When update is called, position is valid, IsAtTarget returns 1:
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos(&dummy);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);

    expect_safe_outputs_and_pwm_stop();
    MotorController_Update();
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* =========================
 * MC_09: While moving, continue if not at target
 * SW-REQ: SWE-REQ-010
 */
void test_MotorController_Update_ShouldContinueMoving_WhenNotAtTarget(void)
{
    // Simulate moving state
    uint8_t dummy = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos(&dummy);
    expect_direction_fwd();
    HAL_TIM_PWM_Start_Expect(&MOTOR_PWM_HANDLE, MOTOR_PWM_CHANNEL);
    MotorController_MoveTo(4);

    // On update, position valid, not at target:
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos(&dummy);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 0);

    // No abort expected/no safe outputs
    MotorController_Update();
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* =========================
 * MC_10: Abort sets safe outputs and stops PWM
 * SW-REQ: SWE-REQ-009
 */
void test_MotorController_Abort_ShouldSetSafeOutputsAndStopPwm(void)
{
    expect_safe_outputs_and_pwm_stop();
    MotorController_Abort();
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* =========================
 * MC_11: GetTarget returns target when stopped, target < 6, ptr valid
 * SW-REQ: SWE-REQ-047
 */
void test_MotorController_GetTarget_ShouldReturnTarget_WhenValid(void)
{
    // Move to a valid target
    uint8_t dummy = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos(&dummy);

    expect_direction_fwd();
    HAL_TIM_PWM_Start_Expect(&MOTOR_PWM_HANDLE, MOTOR_PWM_CHANNEL);
    MotorController_MoveTo(2);

    // Abort to move to stopped state
    expect_safe_outputs_and_pwm_stop();
    MotorController_Abort();

    uint8_t tgt = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL_UINT8(2, tgt);
}

/* =========================
 * MC_12: GetTarget returns 0 when moving
 * SW-REQ: SWE-REQ-047
 */
void test_MotorController_GetTarget_ShouldReturn0_WhenMoving(void)
{
    uint8_t dummy = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos(&dummy);
    expect_direction_fwd();
    HAL_TIM_PWM_Start_Expect(&MOTOR_PWM_HANDLE, MOTOR_PWM_CHANNEL);
    MotorController_MoveTo(4);

    uint8_t tgt = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}

/* =========================
 * MC_13: GetTarget returns 0 on NULL ptr
 * SW-REQ: SWE-REQ-047
 */
void test_MotorController_GetTarget_ShouldReturn0_OnNullPointer(void)
{
    expect_safe_outputs_and_pwm_stop();
    MotorController_Abort();
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(NULL));
}

/* =========================
 * MC_14: GetTarget returns 0 if stored target is >=6
 * SW-REQ: SWE-REQ-047
 */
void test_MotorController_GetTarget_ShouldReturn0_WhenTargetIsOutOfRange(void)
{
    // Set target to an out-of-range value and stop the motor first
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0); // cause abort
    expect_safe_outputs_and_pwm_stop();
    MotorController_MoveTo(6);

    uint8_t tgt = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}
