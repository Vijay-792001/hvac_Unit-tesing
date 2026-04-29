/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

extern TIM_HandleTypeDef htim3;

void setUp(void) {}
void tearDown(void) {}

/* MC_01 */
void test_motor_controller_MC_01(void)
{
    HAL_GPIO_Init_Expect(GPIOB, NULL);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Init();

    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    uint8_t target;
    TEST_ASSERT_EQUAL_UINT8(1U, MotorController_GetTarget(&target));
    TEST_ASSERT_EQUAL_UINT8(0U, target);
}

/* MC_02 */
void test_motor_controller_MC_02(void)
{
    uint8_t pos = 1;
    PositionSensing_GetPosition_ExpectAndReturn(&pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    TEST_ASSERT_EQUAL_UINT8(1U, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
}

/* MC_03 */
void test_motor_controller_MC_03(void)
{
    uint8_t pos = 5;
    PositionSensing_GetPosition_ExpectAndReturn(&pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL_UINT8(1U, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
}

/* MC_04 */
void test_motor_controller_MC_04(void)
{
    uint8_t pos = 3;
    PositionSensing_GetPosition_ExpectAndReturn(&pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(3);

    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_05 */
void test_motor_controller_MC_05(void)
{
    PositionSensing_GetPosition_ExpectAndReturn(NULL, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());
}

/* MC_06 */
void test_motor_controller_MC_06(void)
{
    MotorController_Update();
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_07 */
void test_motor_controller_MC_07(void)
{
    uint8_t pos = 0;
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAndReturn(&pos, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);
    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());
}

/* MC_08 */
void test_motor_controller_MC_08(void)
{
    uint8_t pos = 4;
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAndReturn(&pos, 1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);
    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());
}

/* MC_09 */
void test_motor_controller_MC_09(void)
{
    uint8_t pos = 3;
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAndReturn(&pos, 1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 0);

    MotorController_MoveTo(4);
    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(1U, MotorController_IsMoving());
}

/* MC_10 */
void test_motor_controller_MC_10(void)
{
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Abort();

    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_11 */
void test_motor_controller_MC_11(void)
{
    uint8_t target;
    MotorController_MoveTo(2);
    MotorController_Abort();
    TEST_ASSERT_EQUAL_UINT8(1U, MotorController_GetTarget(&target));
    TEST_ASSERT_EQUAL_UINT8(2U, target);
}

/* MC_12 */
void test_motor_controller_MC_12(void)
{
    MotorController_MoveTo(2);
    uint8_t target;
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_GetTarget(&target));
}

/* MC_13 */
void test_motor_controller_MC_13(void)
{
    MotorController_Abort();
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_GetTarget(NULL));
}

/* MC_14 */
void test_motor_controller_MC_14(void)
{
    MotorController_Abort();
    MotorController_MoveTo(6);
    uint8_t target;
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_GetTarget(&target));
}