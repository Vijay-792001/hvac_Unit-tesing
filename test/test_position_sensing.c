#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_PositionSensor_Init_InitializesADC(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    TEST_ASSERT_EQUAL(PS_OK, PositionSensor_Init());
}

void test_PositionSensor_Init_HALFails_ReturnsError(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_ERROR);
    TEST_ASSERT_EQUAL(PS_HW_FAIL, PositionSensor_Init());
}

void test_PositionSensor_Read_ReadsADCValue(void)
{
    uint16_t adcValue = 1234;
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, adcValue);
    uint16_t pos = 0;
    TEST_ASSERT_EQUAL(PS_OK, PositionSensor_Read(&pos));
    TEST_ASSERT_EQUAL(adcValue, pos);
}

void test_PositionSensor_Read_NullPointer_ReturnsError(void)
{
    TEST_ASSERT_EQUAL(PS_NULL_PTR, PositionSensor_Read(NULL));
}

void test_PositionSensor_Read_PollForConversionFails_ReturnsError(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_ERROR);
    uint16_t pos;
    TEST_ASSERT_EQUAL(PS_HW_FAIL, PositionSensor_Read(&pos));
}
