/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* Test Plan ID: TC01 */
void test_motor_controller_TC01(void)
{
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim1, TIM_CHANNEL_1, HAL_OK);
    int result = motor_controller_start();
    TEST_ASSERT_EQUAL(0, result);
}

/* Test Plan ID: TC02 */
void test_motor_controller_TC02(void)
{
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim1, TIM_CHANNEL_1, HAL_ERROR);
    int result = motor_controller_start();
    TEST_ASSERT_EQUAL(-1, result);
}

/* Test Plan ID: TC03 */
void test_motor_controller_TC03(void)
{
    HAL_TIM_PWM_Stop_ExpectAndReturn(&htim1, TIM_CHANNEL_1, HAL_OK);
    int result = motor_controller_stop();
    TEST_ASSERT_EQUAL(0, result);
}

/* Test Plan ID: TC04 */
void test_motor_controller_TC04(void)
{
    HAL_TIM_PWM_Stop_ExpectAndReturn(&htim1, TIM_CHANNEL_1, HAL_ERROR);
    int result = motor_controller_stop();
    TEST_ASSERT_EQUAL(-1, result);
}

/* Test Plan ID: TC05 */
void test_motor_controller_TC05(void)
{
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim1, TIM_CHANNEL_1, HAL_OK);
    HAL_TIM_PWM_Stop_ExpectAndReturn(&htim1, TIM_CHANNEL_1, HAL_OK);
    int result = motor_controller_restart();
    TEST_ASSERT_EQUAL(0, result);
}