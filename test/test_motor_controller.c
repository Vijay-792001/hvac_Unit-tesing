#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_MotorController_Init_InitializesHardware(void)
{
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim1, TIM_CHANNEL_1, HAL_OK);
    TEST_ASSERT_EQUAL(MC_OK, MotorController_Init());
}

void test_MotorController_Init_HALFails_ReturnsError(void)
{
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim1, TIM_CHANNEL_1, HAL_ERROR);
    TEST_ASSERT_EQUAL(MC_HW_FAIL, MotorController_Init());
}

void test_MotorController_SetSpeed_NormalSpeed_SetsPWM(void)
{
    uint8_t speed = 50;
    HAL_TIM_PWM_SetDutyCycle_ExpectAndReturn(&htim1, TIM_CHANNEL_1, 50, HAL_OK);
    TEST_ASSERT_EQUAL(MC_OK, MotorController_SetSpeed(speed));
}

void test_MotorController_SetSpeed_ZeroSpeed_SetsPWM(void)
{
    uint8_t speed = 0;
    HAL_TIM_PWM_SetDutyCycle_ExpectAndReturn(&htim1, TIM_CHANNEL_1, 0, HAL_OK);
    TEST_ASSERT_EQUAL(MC_OK, MotorController_SetSpeed(speed));
}

void test_MotorController_SetSpeed_MaxSpeed_SetsPWM(void)
{
    uint8_t speed = 100;
    HAL_TIM_PWM_SetDutyCycle_ExpectAndReturn(&htim1, TIM_CHANNEL_1, 100, HAL_OK);
    TEST_ASSERT_EQUAL(MC_OK, MotorController_SetSpeed(speed));
}

void test_MotorController_SetSpeed_AboveMax_ReturnsError(void)
{
    uint8_t speed = 101;
    TEST_ASSERT_EQUAL(MC_INVALID_PARAM, MotorController_SetSpeed(speed));
}

void test_MotorController_SetSpeed_HALFails_ReturnsError(void)
{
    uint8_t speed = 50;
    HAL_TIM_PWM_SetDutyCycle_ExpectAndReturn(&htim1, TIM_CHANNEL_1, 50, HAL_ERROR);
    TEST_ASSERT_EQUAL(MC_HW_FAIL, MotorController_SetSpeed(speed));
}

void test_MotorController_Stop_StopsPWM(void)
{
    HAL_TIM_PWM_Stop_ExpectAndReturn(&htim1, TIM_CHANNEL_1, HAL_OK);
    TEST_ASSERT_EQUAL(MC_OK, MotorController_Stop());
}

void test_MotorController_Stop_HALFails_ReturnsError(void)
{
    HAL_TIM_PWM_Stop_ExpectAndReturn(&htim1, TIM_CHANNEL_1, HAL_ERROR);
    TEST_ASSERT_EQUAL(MC_HW_FAIL, MotorController_Stop());
}
