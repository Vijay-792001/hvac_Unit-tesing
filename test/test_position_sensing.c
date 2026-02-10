#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void test_PositionSensor_Read_ShouldReturnAdcValue(void)
{
    uint32_t expected_adc = 1850;
    HAL_ADC_PollForConversion_ExpectAndReturn(NULL, 100, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(NULL, expected_adc);
    int32_t pos = PositionSensor_Read();
    TEST_ASSERT_EQUAL(expected_adc, pos);
}

void test_PositionSensor_AdcError_ShouldReturnError(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(NULL, 100, HAL_ERROR);
    int32_t pos = PositionSensor_Read();
    TEST_ASSERT_EQUAL(-1, pos);
}

void test_PositionSensor_NullHandle_ShouldReturnError(void)
{
    PositionSensor_InjectAdcHandle(NULL);
    int32_t pos = PositionSensor_Read();
    TEST_ASSERT_EQUAL(-2, pos);
}

void test_PositionSensor_OutOfBounds_ShouldReturnError(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(NULL, 100, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(NULL, 5000);
    int32_t pos = PositionSensor_Read();
    TEST_ASSERT_EQUAL(-3, pos);
}
