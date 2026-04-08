/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

void setUp(void) {}
void tearDown(void) {}

/* MC_01: Init sets safe state and starts PWM */
void test_motor_controller_MC_01(void)
{
    HAL_GPIO_Init_Expect(GPIOB, NULL);  // Will not check contents
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Init();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    uint8_t tgt = 123;
    TEST_ASSERT_TRUE(MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL_UINT8(0, tgt);
}

/* MC_02: Move forward when current < target */
void test_motor_controller_MC_02(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out((uint8_t[]){1});

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
}

/* MC_03: Move reverse when current > target */
void test_motor_controller_MC_03(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out((uint8_t[]){5});

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
}

/* MC_04: No movement when already at target (Abort) */
void test_motor_controller_MC_04(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out((uint8_t[]){3});

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(3);

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_05: Abort if current position is invalid at start of MoveTo */
void test_motor_controller_MC_05(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_06: Update does nothing when motor not moving */
void test_motor_controller_MC_06(void)
{
    MotorController_Init();

    // No PositionSensing_Update, GetPosition, IsAtTarget, no GPIO/PWM calls expected
    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_07: While moving and during Update, if GetPosition invalid -> Abort */
void test_motor_controller_MC_07(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out((uint8_t[]){2});
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_08: While moving, stop when target reached using IsAtTarget */
void test_motor_controller_MC_08(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out((uint8_t[]){2});
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out((uint8_t[]){4});
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_09: While moving, continue if not at target */
void test_motor_controller_MC_09(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out((uint8_t[]){2});
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 0);

    // No abort outputs called
    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* MC_10: Abort sets safe outputs and stops PWM */
void test_motor_controller_MC_10(void)
{
    MotorController_Init();
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Abort();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_11: GetTarget returns target when stopped and target < 6 and ptr valid */
void test_motor_controller_MC_11(void)
{
    MotorController_Init();
    uint8_t target_val = 2;
    MotorController_MoveTo(target_val);

    // Abort movement to be stopped for GetTarget to work
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Abort();

    uint8_t out = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&out));
    TEST_ASSERT_EQUAL_UINT8(target_val, out);
}

/* MC_12: GetTarget returns 0 when moving */
void test_motor_controller_MC_12(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out((uint8_t[]){1});
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    uint8_t tgt = 123;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}

/* MC_13: GetTarget returns 0 on NULL pointer */
void test_motor_controller_MC_13(void)
{
    MotorController_Init();
    uint8_t target_val = 2;
    MotorController_MoveTo(target_val);

    // Ensure stopped
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Abort();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(NULL));
}

/* MC_14: GetTarget returns 0 when stored target is out of range (>=6) */
void test_motor_controller_MC_14(void)
{
    MotorController_Init();
    // Set s_target_position out of range via MoveTo (since no direct setter)
    // No hardware side effect if GetPosition returns 0 in MoveTo
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(6);
    // Now suppose s_target_position is 6 or more (aborted, so stopped)
    uint8_t tgt;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}
