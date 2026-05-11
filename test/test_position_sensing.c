/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_position_sensing_PS01(void)
{
    ADC_HandleTypeDef hadc;
    HAL_ADC_Start_ExpectAndReturn(&hadc, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc, 1234U);
    uint32_t value = PositionSensing_ReadADC(&hadc);
    TEST_ASSERT_EQUAL_UINT32(1234U, value);
}

void test_position_sensing_PS02(void)
{
    ADC_HandleTypeDef hadc;
    HAL_ADC_Start_ExpectAndReturn(&hadc, HAL_ERROR);
    uint32_t value = PositionSensing_ReadADC(&hadc);
    TEST_ASSERT_EQUAL_UINT32(0U, value);
}

void test_position_sensing_PS03(void)
{
    ADC_HandleTypeDef hadc;
    HAL_ADC_Start_ExpectAndReturn(&hadc, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc, 10, HAL_ERROR);
    uint32_t value = PositionSensing_ReadADC(&hadc);
    TEST_ASSERT_EQUAL_UINT32(0U, value);
}

void test_position_sensing_PS04(void)
{
    ADC_HandleTypeDef hadc;
    HAL_ADC_Start_ExpectAndReturn(&hadc, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc, 4095U);
    uint32_t value = PositionSensing_ReadADC(&hadc);
    TEST_ASSERT_EQUAL_UINT32(4095U, value);
}

void test_position_sensing_PS05(void)
{
    ADC_HandleTypeDef hadc;
    HAL_ADC_Start_ExpectAndReturn(&hadc, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc, 0U);
    uint32_t value = PositionSensing_ReadADC(&hadc);
    TEST_ASSERT_EQUAL_UINT32(0U, value);
}