#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/* TC_PS_001: Get should read ADC and return position value */
void test_PositionSensing_Get_Reads_ADC_And_Returns_Value(void)
{
    uint32_t adc_value = 1234;
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, adc_value);
    int result = PositionSensing_Get();
    TEST_ASSERT_EQUAL(adc_value, result);
}

/* TC_PS_002: Get returns 0 if ADC poll times out or fails */
void test_PositionSensing_Get_ADCPoll_Fails_Returns_Zero(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_TIMEOUT);
    int result = PositionSensing_Get();
    TEST_ASSERT_EQUAL(0, result);
}

/* TC_PS_003: Get returns 0 if HAL_ADC_GetValue returns 0 */
void test_PositionSensing_Get_ADC_Returns_Zero_Value(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 0);
    int result = PositionSensing_Get();
    TEST_ASSERT_EQUAL(0, result);
}

/* TC_PS_004: Multiple PositionSensing_Get calls return different values */
void test_PositionSensing_Get_Called_Twice_Yields_Different_Results(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 321);
    int v1 = PositionSensing_Get();
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 2222);
    int v2 = PositionSensing_Get();
    TEST_ASSERT_EQUAL(321, v1);
    TEST_ASSERT_EQUAL(2222, v2);
}
