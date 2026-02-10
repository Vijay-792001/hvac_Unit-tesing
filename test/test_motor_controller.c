#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"

void test_Motor_Start_ShouldStartPWMSuccessfully(void)
{
    HAL_TIM_PWM_Start_ExpectAndReturn(NULL, MOTOR_PWM_CHANNEL, HAL_OK);
    int result = MotorController_Start();
    TEST_ASSERT_EQUAL(0, result);
}

void test_Motor_Stop_ShouldStopPWMSuccessfully(void)
{
    HAL_TIM_PWM_Stop_ExpectAndReturn(NULL, MOTOR_PWM_CHANNEL, HAL_OK);
    int result = MotorController_Stop();
    TEST_ASSERT_EQUAL(0, result);
}

void test_Motor_SetSpeed_ValidSpeed_ShouldUpdatePWMDutyCycle(void)
{
    HAL_TIM_PWM_Start_IgnoreAndReturn(HAL_OK);
    HAL_TIM_PWM_ConfigChannel_ExpectAndReturn(NULL, NULL, MOTOR_PWM_CHANNEL, HAL_OK);
    int result = MotorController_SetSpeed(80);
    TEST_ASSERT_EQUAL(0, result);
}

void test_Motor_SetSpeed_OutOfRange_ShouldReturnError(void)
{
    int result = MotorController_SetSpeed(150);
    TEST_ASSERT_EQUAL(-1, result);
}

void test_MotorController_NullHandle_ReturnsError(void)
{
    MotorController_InjectTimerHandle(NULL);
    int result = MotorController_Start();
    TEST_ASSERT_EQUAL(-2, result);
}
