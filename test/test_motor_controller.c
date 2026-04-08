/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* MC_01: Init sets safe state and starts PWM */
void test_motor_controller_MC_01(void)
{
    HAL_GPIO_Init_Ignore(); // Structure content is not public
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Init();

    TEST_ASSERT_EQUAL_UINT8(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    uint8_t tgt = 99;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL_UINT8(0, tgt);
}

/* MC_02: Move forward when current < target */
void test_motor_controller_MC_02(void)
{
    uint8_t cur_pos = 1;
    PositionSensing_GetPosition_ExpectAndReturn(&cur_pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_UINT8(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
}

/* MC_03: Move reverse when current > target */
void test_motor_controller_MC_03(void)
{
    uint8_t cur_pos = 5;
    PositionSensing_GetPosition_ExpectAndReturn(&cur_pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_UINT8(MOTOR_STATE_MOVING_REV, MotorController_GetState());
}

/* MC_04: No movement when already at target (Abort) */
void test_motor_controller_MC_04(void)
{
    uint8_t cur_pos = 3;
    PositionSensing_GetPosition_ExpectAndReturn(&cur_pos, 1);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(3);

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_UINT8(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_05: Abort if current position is invalid at start of MoveTo */
void test_motor_controller_MC_05(void)
{
    uint8_t dummy = 99;
    PositionSensing_GetPosition_ExpectAndReturn(&dummy, 0);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_UINT8(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_06: Update does nothing when motor not moving */
void test_motor_controller_MC_06(void)
{
    MotorController_Abort();
    MotorController_Update();
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_UINT8(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_07: While motor moving and during MotorController_Update if GetPosition becomes invalid -> Abort */
void test_motor_controller_MC_07(void)
{
    uint8_t pos = 1;
    PositionSensing_GetPosition_ExpectAndReturn(&pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);

    PositionSensing_Update_Expect();
    uint8_t dummy = 42;
    PositionSensing_GetPosition_ExpectAndReturn(&dummy, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_UINT8(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_08: While motor moving: stop when target reached using IsAtTarget */
void test_motor_controller_MC_08(void)
{
    uint8_t pos = 3;
    PositionSensing_GetPosition_ExpectAndReturn(&pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAndReturn(&pos, 1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_UINT8(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_09: While motor moving: continue moving if not at target */
void test_motor_controller_MC_09(void)
{
    uint8_t pos = 1;
    PositionSensing_GetPosition_ExpectAndReturn(&pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAndReturn(&pos, 1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 0);

    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* MC_10: Abort sets safe outputs and stops PWM */
void test_motor_controller_MC_10(void)
{
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Abort();
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_11: GetTarget returns target when stopped and target < 6 and ptr valid */
void test_motor_controller_MC_11(void)
{
    MotorController_Abort();
    uint8_t tgt = 99;
    MotorController_MoveTo(2);
    MotorController_Abort();
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL_UINT8(2, tgt);
}

/* MC_12: GetTarget returns 0 when moving */
void test_motor_controller_MC_12(void)
{
    uint8_t pos = 1;
    PositionSensing_GetPosition_ExpectAndReturn(&pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);

    uint8_t tgt = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}

/* MC_13: GetTarget returns 0 on NULL pointer */
void test_motor_controller_MC_13(void)
{
    MotorController_Abort();
    MotorController_MoveTo(2);
    MotorController_Abort();
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(NULL));
}

/* MC_14: GetTarget returns 0 when stored target is out of range (>=6) */
void test_motor_controller_MC_14(void)
{
    MotorController_Abort();
    MotorController_MoveTo(6);
    MotorController_Abort();
    uint8_t tgt = 0xAB;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}

