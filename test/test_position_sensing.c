#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_PositionSensing_GetCurrent_NormalValues(void)
{
    ADC_HandleTypeDef hadc;
    HAL_ADC_Start_ExpectAndReturn(&hadc, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc, 100, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc, 1234);

    int16_t position;
    int result = PositionSensing_GetCurrent(&hadc, &position);
    TEST_ASSERT_EQUAL(POSITION_SUCCESS, result);
    TEST_ASSERT_EQUAL(1234, position);
}

void test_PositionSensing_GetCurrent_NullAdcPointer_ReturnsError(void)
{
    int16_t pos;
    int result = PositionSensing_GetCurrent(NULL, &pos);
    TEST_ASSERT_EQUAL(POSITION_ERROR, result);
}

void test_PositionSensing_GetCurrent_NullPositionPointer(void)
{
    ADC_HandleTypeDef hadc;
    int result = PositionSensing_GetCurrent(&hadc, NULL);
    TEST_ASSERT_EQUAL(POSITION_ERROR, result);
}

void test_PositionSensing_GetCurrent_AdcStartFails(void)
{
    ADC_HandleTypeDef hadc;
    HAL_ADC_Start_ExpectAndReturn(&hadc, HAL_ERROR);

    int16_t pos;
    int result = PositionSensing_GetCurrent(&hadc, &pos);
    TEST_ASSERT_EQUAL(POSITION_ERROR, result);
}

void test_PositionSensing_GetCurrent_AdcPollTimeout(void)
{
    ADC_HandleTypeDef hadc;
    HAL_ADC_Start_ExpectAndReturn(&hadc, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc, 100, HAL_TIMEOUT);

    int16_t pos;
    int result = PositionSensing_GetCurrent(&hadc, &pos);
    TEST_ASSERT_EQUAL(POSITION_ERROR, result);
}
