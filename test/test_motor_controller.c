/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

extern TIM_HandleTypeDef htim3;

void setUp(void) {}
void tearDown(void) {}

/*
 * TestPlan: Motor Controller
 * MC_TC01: Init sets safe state and starts PWM
 * MC_TC02: Move forward when current < target
 * MC_TC03: Move reverse when current > target
 * MC_TC04: No movement when already at target (stop)
 * MC_TC05: Abort if current position is invalid at start of MoveTo
 * MC_TC06: Update does nothing when motor not moving
 * MC_TC07: While moving, if GetPosition becomes invalid in Update -> Abort
 * MC_TC08: While moving, stop when target reached
 * MC_TC09: While moving, continue if not at target
 * MC_TC10: Abort sets safe outputs/pins/PWM
 * MC_TC11: GetTarget returns stored if valid and stopped
 * MC_TC12: GetTarget returns 0 when moving
 * MC_TC13: GetTarget returns 0 if NULL pointer
 * MC_TC14: GetTarget returns 0 if stored target is >= 6
 */

void test_MC_TC01_InitSetsSafeAndStartsPWM(void)
{
    HAL_GPIO_Init_Ignore();
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Init();

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());

    uint8_t tgt = 0xFE;
    TEST_ASSERT_EQUAL(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL(0, tgt);
}

void test_MC_TC02_MoveForward_CurrentLessThanTarget(void)
{
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
}

void test_MC_TC03_MoveReverse_CurrentGreaterThanTarget(void)
{
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
}

void test_MC_TC04_NoMovementWhenAtTarget(void)
{
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(3);

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

void test_MC_TC05_AbortIfPositionInvalidAtStartMoveTo(void)
{
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(5);

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

void test_MC_TC06_UpdateDoesNothingWhenNotMoving(void)
{
    MotorController_Abort();
    MotorController_Update();

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

void test_MC_TC07_Update_AbortIfPositionBecomesInvalid(void)
{
    MotorController_MoveTo(2);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

void test_MC_TC08_Update_StopAtTarget(void)
{
    MotorController_MoveTo(4);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

void test_MC_TC09_Update_ContinueIfNotAtTarget(void)
{
    MotorController_MoveTo(4);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 0);

    MotorController_Update();

    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
}

void test_MC_TC10_AbortSetsSafeOutputs(void)
{
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Abort();

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

void test_MC_TC11_GetTargetWorksWhenStoppedAndValid(void)
{
    MotorController_Init();
    MotorController_MoveTo(2);
    MotorController_Abort();
    uint8_t tgt = 0xAB;
    TEST_ASSERT_EQUAL(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL(2, tgt);
}

void test_MC_TC12_GetTargetFailsWhenMoving(void)
{
    MotorController_MoveTo(2);
    uint8_t tgt = 0;
    TEST_ASSERT_EQUAL(0, MotorController_GetTarget(&tgt));
}

void test_MC_TC13_GetTargetFailsOnNULLPointer(void)
{
    MotorController_Abort();
    TEST_ASSERT_EQUAL(0, MotorController_GetTarget(NULL));
}

void test_MC_TC14_GetTargetFailsOnTargetOutOfRange(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(6);
    uint8_t tgt = 0;
    TEST_ASSERT_EQUAL(0, MotorController_GetTarget(&tgt));
}
