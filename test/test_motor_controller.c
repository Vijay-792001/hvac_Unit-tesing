#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/* Test ID: MCTR-1 Start motor with valid speed */
void test_MotorController_StartMotor_ValidSpeed_ShouldSucceed(void)
{
    uint8_t speed = 100;
    HAL_TIM_PWM_Start_Expect(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start_Expect(&htim1, TIM_CHANNEL_2);
    __HAL_TIM_SET_COMPARE_Expect(&htim1, TIM_CHANNEL_1, speed);
    int result = MotorController_StartMotor(speed);
    TEST_ASSERT_EQUAL(0, result);
}

/* Test ID: MCTR-2 Start motor with speed out of range */
void test_MotorController_StartMotor_InvalidSpeed_ShouldFail(void)
{
    uint8_t speed = 250; // Assume > 200 is invalid (example boundary)
    int result = MotorController_StartMotor(speed);
    TEST_ASSERT_EQUAL(-1, result);
}

/* Test ID: MCTR-3 Stop motor */
void test_MotorController_StopMotor_Normal_ShouldSucceed(void)
{
    HAL_TIM_PWM_Stop_Expect(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop_Expect(&htim1, TIM_CHANNEL_2);
    int result = MotorController_StopMotor();
    TEST_ASSERT_EQUAL(0, result);
}

/* Test ID: MCTR-4 NULL pointer (API takes value, not pointer) – skip if not applicable */

/* Test ID: MCTR-5 Start motor at minimum speed boundary */
void test_MotorController_StartMotor_MinSpeed_ShouldSucceed(void)
{
    uint8_t min_speed = 0;
    HAL_TIM_PWM_Start_Expect(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start_Expect(&htim1, TIM_CHANNEL_2);
    __HAL_TIM_SET_COMPARE_Expect(&htim1, TIM_CHANNEL_1, min_speed);
    int result = MotorController_StartMotor(min_speed);
    TEST_ASSERT_EQUAL(0, result);
}

/* Test ID: MCTR-6 Start motor at maximum speed boundary */
void test_MotorController_StartMotor_MaxSpeed_ShouldSucceed(void)
{
    uint8_t max_speed = 200;
    HAL_TIM_PWM_Start_Expect(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start_Expect(&htim1, TIM_CHANNEL_2);
    __HAL_TIM_SET_COMPARE_Expect(&htim1, TIM_CHANNEL_1, max_speed);
    int result = MotorController_StartMotor(max_speed);
    TEST_ASSERT_EQUAL(0, result);
}
