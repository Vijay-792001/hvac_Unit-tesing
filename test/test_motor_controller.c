#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_MotorController_Move_Nominal(void)
{
    HAL_GPIO_WritePin_Expect(MOTOR_PORT, MOTOR_PIN_1, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(MOTOR_PORT, MOTOR_PIN_2, GPIO_PIN_RESET);

    int result = MotorController_Move(100);

    TEST_ASSERT_EQUAL(0, result);
}

void test_MotorController_Move_Negative_Position(void)
{
    int result = MotorController_Move(-1);

    TEST_ASSERT_EQUAL(-1, result);
}

void test_MotorController_Move_Too_High(void)
{
    int result = MotorController_Move(MOTOR_MAX_POSITION + 1);

    TEST_ASSERT_EQUAL(-1, result);
}

void test_MotorController_Move_HAL_GPIO_Failure(void)
{
    HAL_GPIO_WritePin_Expect(MOTOR_PORT, MOTOR_PIN_1, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(MOTOR_PORT, MOTOR_PIN_2, GPIO_PIN_RESET);

    int result = MotorController_Move(10);

    TEST_ASSERT_EQUAL(0, result);
}

void test_MotorController_Move_Zero_Position(void)
{
    HAL_GPIO_WritePin_Expect(MOTOR_PORT, MOTOR_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(MOTOR_PORT, MOTOR_PIN_2, GPIO_PIN_RESET);

    int result = MotorController_Move(0);

    TEST_ASSERT_EQUAL(0, result);
}

void test_MotorController_Move_Maximum_Position(void)
{
    HAL_GPIO_WritePin_Expect(MOTOR_PORT, MOTOR_PIN_1, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(MOTOR_PORT, MOTOR_PIN_2, GPIO_PIN_RESET);

    int result = MotorController_Move(MOTOR_MAX_POSITION);

    TEST_ASSERT_EQUAL(0, result);
}
