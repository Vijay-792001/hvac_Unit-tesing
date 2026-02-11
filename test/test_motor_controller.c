#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"

void test_MotorController_Start_ShouldEnableMotorAndReturnSuccess(void)
{
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT, MOTOR_GPIO_PIN, GPIO_PIN_SET);
    HAL_Delay_Expect(5);
    int result = MotorController_Start();
    TEST_ASSERT_EQUAL_INT(MOTOR_CONTROLLER_SUCCESS, result);
}

void test_MotorController_Stop_ShouldDisableMotorAndReturnSuccess(void)
{
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT, MOTOR_GPIO_PIN, GPIO_PIN_RESET);
    int result = MotorController_Stop();
    TEST_ASSERT_EQUAL_INT(MOTOR_CONTROLLER_SUCCESS, result);
}

void test_MotorController_SetSpeed_ValidInput_ShouldSetPWMDutyCycle(void)
{
    uint8_t speed = 50;
    HAL_TIM_PWM_Start_ExpectAnyArgsAndReturn(HAL_OK);
    int result = MotorController_SetSpeed(speed);
    TEST_ASSERT_EQUAL_INT(MOTOR_CONTROLLER_SUCCESS, result);
}

void test_MotorController_SetSpeed_InvalidInput_ShouldReturnError(void)
{
    uint8_t speed = 110; // Out-of-bounds
    int result = MotorController_SetSpeed(speed);
    TEST_ASSERT_EQUAL_INT(MOTOR_CONTROLLER_ERROR_INVALID_SPEED, result);
}

void test_MotorController_SetSpeed_ZeroSpeed_ShouldHandleAsStop(void)
{
    uint8_t speed = 0;
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT, MOTOR_GPIO_PIN, GPIO_PIN_RESET);
    int result = MotorController_SetSpeed(speed);
    TEST_ASSERT_EQUAL_INT(MOTOR_CONTROLLER_SUCCESS, result);
}
