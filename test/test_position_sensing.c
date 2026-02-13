#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_PositionSensing_Get_ReturnsPosition(void)
{
    uint16_t expected_adc_val = 1234;
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, expected_adc_val);
    HAL_ADC_Stop_ExpectAndReturn(&hadc1, HAL_OK);
    int pos = PositionSensing_Get();
    TEST_ASSERT_EQUAL(expected_adc_val, pos);
}

void test_PositionSensing_Get_HALADCStartFails_ReturnsError(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_ERROR);
    int pos = PositionSensing_Get();
    TEST_ASSERT_EQUAL(POSITION_ERR, pos);
}

void test_PositionSensing_Get_ConversionTimeout_ReturnsError(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_TIMEOUT);
    HAL_ADC_Stop_ExpectAndReturn(&hadc1, HAL_OK);
    int pos = PositionSensing_Get();
    TEST_ASSERT_EQUAL(POSITION_ERR, pos);
}

void test_PositionSensing_Get_NullADCHandle_ReturnsError(void)
{
    extern ADC_HandleTypeDef *phadc1;
    phadc1 = NULL;
    int pos = PositionSensing_Get();
    TEST_ASSERT_EQUAL(POSITION_ERR, pos);
}
