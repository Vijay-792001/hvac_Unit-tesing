/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_motor_controller_MC01(void)
{
    TIM_HandleTypeDef htim;
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim, TIM_CHANNEL_1, HAL_OK);
    int ret = MotorController_StartPWM(&htim);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_motor_controller_MC02(void)
{
    TIM_HandleTypeDef htim;
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim, TIM_CHANNEL_1, HAL_ERROR);
    int ret = MotorController_StartPWM(&htim);
    TEST_ASSERT_EQUAL_INT(-1, ret);
}

void test_motor_controller_MC03(void)
{
    TIM_HandleTypeDef htim;
    HAL_TIM_PWM_Stop_ExpectAndReturn(&htim, TIM_CHANNEL_1, HAL_OK);
    int ret = MotorController_StopPWM(&htim);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_motor_controller_MC04(void)
{
    TIM_HandleTypeDef htim;
    HAL_TIM_PWM_Stop_ExpectAndReturn(&htim, TIM_CHANNEL_1, HAL_ERROR);
    int ret = MotorController_StopPWM(&htim);
    TEST_ASSERT_EQUAL_INT(-1, ret);
}

void test_motor_controller_MC05(void)
{
    TIM_HandleTypeDef htim;
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim, TIM_CHANNEL_1, HAL_OK);
    HAL_TIM_PWM_Stop_ExpectAndReturn(&htim, TIM_CHANNEL_1, HAL_OK);
    int ret_start = MotorController_StartPWM(&htim);
    int ret_stop = MotorController_StopPWM(&htim);
    TEST_ASSERT_EQUAL_INT(0, ret_start);
    TEST_ASSERT_EQUAL_INT(0, ret_stop);
}