/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* Test Plan ID: TC01 */
void test_position_sensing_TC01(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1234);
    int result = position_sensing_read();
    TEST_ASSERT_EQUAL(1234, result);
}

/* Test Plan ID: TC02 */
void test_position_sensing_TC02(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_ERROR);
    int result = position_sensing_read();
    TEST_ASSERT_EQUAL(-1, result);
}

/* Test Plan ID: TC03 */
void test_position_sensing_TC03(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_ERROR);
    int result = position_sensing_read();
    TEST_ASSERT_EQUAL(-2, result);
}

/* Test Plan ID: TC04 */
void test_position_sensing_TC04(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 0);
    int result = position_sensing_read();
    TEST_ASSERT_EQUAL(0, result);
}

/* Test Plan ID: TC05 */
void test_position_sensing_TC05(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4095);
    int result = position_sensing_read();
    TEST_ASSERT_EQUAL(4095, result);
}