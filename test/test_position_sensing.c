#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_Position_Sensing_Init_should_Configure_ADC(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    Position_Sensing_Init();
}

void test_Position_Sensing_Get_Position_should_Return_ADC_Value(void)
{
    uint32_t expected_adc = 2048;
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, expected_adc);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    uint16_t pos = Position_Sensing_Get_Position();
    TEST_ASSERT_EQUAL_UINT16(expected_adc, pos);
}

void test_Position_Sensing_Get_Position_should_Handle_Null_ADC(void)
{
}

void test_Position_Sensing_Get_Position_should_Handle_Poll_Timeout(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_TIMEOUT);
    uint16_t pos = Position_Sensing_Get_Position();
    TEST_ASSERT_EQUAL_UINT16(0, pos);
}

void test_Position_Sensing_Get_Position_should_Handle_Smallest_Value(void)
{
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 0);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    uint16_t pos = Position_Sensing_Get_Position();
    TEST_ASSERT_EQUAL_UINT16(0, pos);
}

void test_Position_Sensing_Get_Position_should_Handle_Largest_Value(void)
{
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 0xFFF);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    uint16_t pos = Position_Sensing_Get_Position();
    TEST_ASSERT_EQUAL_UINT16(0xFFF, pos);
}
