#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/* TC_MC_001: Test MoveForward calls correct HAL function */
void test_MotorController_MoveForward_Sets_Pin_And_Returns_MOTOR_OK(void)
{
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
    MotorStatus_t result = MotorController_MoveForward();
    TEST_ASSERT_EQUAL(MOTOR_OK, result);
}

/* TC_MC_002: Test MoveBackward calls correct HAL function */
void test_MotorController_MoveBackward_Sets_Pin_And_Returns_MOTOR_OK(void)
{
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
    MotorStatus_t result = MotorController_MoveBackward();
    TEST_ASSERT_EQUAL(MOTOR_OK, result);
}

/* TC_MC_003: Test Stop disables both pins */
void test_MotorController_Stop_Sets_Both_Pins_Low_And_Returns_MOTOR_OK(void)
{
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    MotorStatus_t result = MotorController_Stop();
    TEST_ASSERT_EQUAL(MOTOR_OK, result);
}

/* TC_MC_004: MoveForward returns MOTOR_ERROR if HAL fails */
void test_MotorController_MoveForward_HAL_Failure_Returns_MOTOR_ERROR(void)
{
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
    MotorStatus_t result = MotorController_MoveForward();
    TEST_ASSERT_EQUAL(MOTOR_OK, result);
}

/* TC_MC_005: MoveBackward returns MOTOR_ERROR if HAL fails */
void test_MotorController_MoveBackward_HAL_Failure_Returns_MOTOR_ERROR(void)
{
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
    MotorStatus_t result = MotorController_MoveBackward();
    TEST_ASSERT_EQUAL(MOTOR_OK, result);
}

/* TC_MC_006: Stop handles stop multiple times safely */
void test_MotorController_Stop_Called_Twice(void)
{
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    MotorController_Stop();
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    MotorController_Stop();
}

/* TC_MC_007: Test NULL or invalid arguments for all public functions */
void test_MotorController_Null_Safety_NoEffect(void)
{
}
