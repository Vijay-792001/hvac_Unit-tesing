#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/* Test ID: PSEN-1 Valid ADC reading */
void test_PositionSensing_ReadPosition_ValidAdcValue(void)
{
    uint32_t raw_adc = 2048; // mid value
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, raw_adc);
    int result = PositionSensing_ReadPosition();
    TEST_ASSERT_EQUAL((int)raw_adc, result);
}

/* Test ID: PSEN-2 ADC conversion timeout/error */
void test_PositionSensing_ReadPosition_ConversionTimeout_ShouldReturnError(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_TIMEOUT);
    int result = PositionSensing_ReadPosition();
    TEST_ASSERT_EQUAL(-1, result);
}

/* Test ID: PSEN-3 ADC hardware failure during read */
void test_PositionSensing_ReadPosition_ConversionError_ShouldReturnError(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_ERROR);
    int result = PositionSensing_ReadPosition();
    TEST_ASSERT_EQUAL(-1, result);
}

/* Test ID: PSEN-4 NULL pointer (API no args, N/A) */

/* Test ID: PSEN-5 Boundary value: lowest ADC reading */
void test_PositionSensing_ReadPosition_AdcMinimum_ShouldReturnZero(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 0);
    int result = PositionSensing_ReadPosition();
    TEST_ASSERT_EQUAL(0, result);
}

/* Test ID: PSEN-6 Boundary value: highest ADC reading */
void test_PositionSensing_ReadPosition_AdcMaximum_ShouldReturnFullScale(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4095);
    int result = PositionSensing_ReadPosition();
    TEST_ASSERT_EQUAL(4095, result);
}
