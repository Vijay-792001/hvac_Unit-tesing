/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

// Stub globals referenced by the module under test
TIM_HandleTypeDef htim3;
GPIO_TypeDef GPIOB_inst;
GPIO_TypeDef *GPIOB = &GPIOB_inst;

void setUp(void)
{
    // Reset (simulate) global motor controller state before each test
    MotorController_Abort();
}

void tearDown(void)
{
    // Clean up, if needed
}

/* ---- MC_01: MotorController_Init() sets safe, starts PWM ---- */
void test_MC_01_MotorController_Init_SetsSafeState_AndStartsPWM(void)
{
    GPIO_InitTypeDef config_check;
    // Expect GPIO Init for both DIR pins
    HAL_GPIO_Init_Expect(GPIOB, &config_check);
    // Set state safe: both DIR pins RESET, PWM started
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    // Call
    MotorController_Init();

    // Should now have default states
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    // target = 0, get target works
    uint8_t t = 11;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&t));
    TEST_ASSERT_EQUAL_UINT8(0, t);
}

/* ---- MC_02: Move forward when current < target ---- */
void test_MC_02_MoveForward_WhenCurrentLTTarget(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    uint8_t cur = 1;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&cur);

    // Set direction FORWARD: FWD RESET, REV SET
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    // Moving fwd
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* ---- MC_03: Move reverse when current > target ---- */
void test_MC_03_MoveReverse_WhenCurrentGTTarget(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    uint8_t cur = 5;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&cur);

    // Set direction REV: FWD SET, REV RESET
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* ---- MC_04: No movement when already at target (Abort) ---- */
void test_MC_04_NoMovement_WhenAtTarget_Aborts(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    uint8_t cur = 3;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&cur);

    // Should abort: both pins RESET, PWM STOP
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(3);

    // Stopped!
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* ---- MC_05: Abort if current position invalid at MoveTo() ---- */
void test_MC_05_MoveTo_InvalidPosition_AbortsImmediately(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    // Abort: pins RESET, PWM STOP
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(1);

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* ---- MC_06: Update does nothing when motor not moving ---- */
void test_MC_06_Update_NoMovement_NoAction(void)
{
    MotorController_Init();
    // No expectations for PositionSensing nor HAL for this idle update
    MotorController_Update();
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* ---- MC_07: Update while moving, GetPosition invalid -> Abort ---- */
void test_MC_07_Update_Moving_GetPositionInvalid_Aborts(void)
{
    MotorController_Init();
    // Fake entry to moving state
    // set target and movement_active >0
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    uint8_t cur = 0;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&cur);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(2);

    // Simulate position update: call Update, GetPosition returns 0 (invalid)
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    // Abort output sequence
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* ---- MC_08: Update moving; at target, should abort ---- */
void test_MC_08_Update_Moving_AtTarget_StopsMotor(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    uint8_t cur = 2;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&cur);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    uint8_t newpos = 4;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&newpos);

    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);

    // Abort output
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* ---- MC_09: Update while moving, not at target, continue ---- */
void test_MC_09_Update_Moving_NotAtTarget_ContinuesMovement(void)
{
    MotorController_Init();
    // Move to set up "moving" state
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    uint8_t cur = 2;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&cur);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    uint8_t mp = 3;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&mp);

    PositionSensing_IsAtTarget_ExpectAndReturn(4, 0);

    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* ---- MC_10: Abort sets both safe and stops PWM ---- */
void test_MC_10_Abort_SetsSafeAndStopsPWM(void)
{
    MotorController_Init();

    // Set state: move to moving
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    uint8_t cur = 2;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&cur);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);
    // Now actually test abort
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Abort();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* ---- MC_11: GetTarget returns target (stopped, valid, ptr valid) ---- */
void test_MC_11_GetTarget_Stopped_ValidPtr_ValidTarget_Returns1(void)
{
    MotorController_Init();
    // Move to position 2
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    uint8_t cur = 0;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&cur);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(2);

    // Abort to stop
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Abort();

    uint8_t tgt = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL_UINT8(2, tgt);
}

/* ---- MC_12: GetTarget returns 0 when moving ---- */
void test_MC_12_GetTarget_Returns0_WhenMoving(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    uint8_t cur = 0;
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&cur);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(3);

    uint8_t tgt = 111;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}

/* ---- MC_13: GetTarget returns 0 on NULL pointer ---- */
void test_MC_13_GetTarget_NullPointer_Returns0(void)
{
    MotorController_Init();
    // Stopped, but valid target value
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(NULL));
}

/* ---- MC_14: GetTarget returns 0 on stored out-of-range target ---- */
void test_MC_14_GetTarget_Returns0_OnInvalidTarget(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0); // Force abort
    // MoveTo(6), will be rejected as position invalid first; so target is >=6
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(6); // set s_target_position = 6

    uint8_t tgt = 1;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}
