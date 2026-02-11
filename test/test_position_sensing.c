#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void test_PositionSensing_Read_ValidChannel_ShouldReturnValue(void)
{
    uint16_t expected_adc_value = 1234;
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, expected_adc_value);
    uint16_t result = 0;
    int ret = PositionSensing_Read(POSITION_CHANNEL_1, &result);
    TEST_ASSERT_EQUAL_INT(POSITION_SENSING_SUCCESS, ret);
    TEST_ASSERT_EQUAL_UINT16(expected_adc_value, result);
}

void test_PositionSensing_Read_InvalidChannel_ShouldReturnError(void)
{
    uint16_t result = 0;
    int ret = PositionSensing_Read((PositionChannel_t)99, &result);
    TEST_ASSERT_EQUAL_INT(POSITION_SENSING_ERROR_INVALID_CHANNEL, ret);
}

void test_PositionSensing_Read_NullPointer_ShouldReturnError(void)
{
    int ret = PositionSensing_Read(POSITION_CHANNEL_1, NULL);
    TEST_ASSERT_EQUAL_INT(POSITION_SENSING_ERROR_NULL, ret);
}

void test_PositionSensing_Read_ChannelLowerBound_ShouldSucceed(void)
{
    uint16_t expected_adc_value = 100;
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, expected_adc_value);
    uint16_t result = 0;
    int ret = PositionSensing_Read(POSITION_CHANNEL_0, &result);
    TEST_ASSERT_EQUAL_INT(POSITION_SENSING_SUCCESS, ret);
    TEST_ASSERT_EQUAL_UINT16(expected_adc_value, result);
}

void test_PositionSensing_HAL_Error_ShouldReturnError(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_ERROR);
    uint16_t result = 0;
    int ret = PositionSensing_Read(POSITION_CHANNEL_1, &result);
    TEST_ASSERT_EQUAL_INT(POSITION_SENSING_ERROR_HW, ret);
}
