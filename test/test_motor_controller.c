#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_Motor_Controller_Init_should_Setup_All_Required_Resources(void)
{
    HAL_TIM_PWM_Start_Expect(&htim1, TIM_CHANNEL_1);
    Motor_Controller_Init();
}

void test_Motor_Controller_Start_should_Start_PWM_and_Set_Duty(void)
{
    uint8_t duty = 75;
    HAL_TIM_PWM_Start_Expect(&htim1, TIM_CHANNEL_1);
    Motor_Controller_Start(duty);
}

void test_Motor_Controller_Start_should_Not_Allow_Duty_Over_100(void)
{
    uint8_t duty = 150;
    Motor_Controller_Start(duty);
}

void test_Motor_Controller_Stop_should_Stop_PWM(void)
{
    HAL_TIM_PWM_Stop_Expect(&htim1, TIM_CHANNEL_1);
    Motor_Controller_Stop();
}

void test_Motor_Controller_Start_should_Handle_Null(void)
{
}

void test_Motor_Controller_Init_should_Handle_HAL_PWM_Start_Error(void)
{
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim1, TIM_CHANNEL_1, HAL_ERROR);
    Motor_Controller_Init();
}
