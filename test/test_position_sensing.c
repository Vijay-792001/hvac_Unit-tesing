#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_PositionSensing_Read_Returns_Valid_ADC_Value(void)
{
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 200);
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);

    int result = PositionSensing_Read();

    TEST_ASSERT_EQUAL(200, result);
}

void test_PositionSensing_Read_HAL_ADC_Start_Error(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_ERROR);

    int result = PositionSensing_Read();

    TEST_ASSERT_EQUAL(-1, result);
}

void test_PositionSensing_Read_Maximum_Value(void)
{
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4095);
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);

    int result = PositionSensing_Read();

    TEST_ASSERT_EQUAL(4095, result);
}

void test_PositionSensing_Read_Minimum_Value(void)
{
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 0);
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);

    int result = PositionSensing_Read();

    TEST_ASSERT_EQUAL(0, result);
}

void test_PositionSensing_Read_NULL_ADC_Handle(void)
{
    extern ADC_HandleTypeDef* hadc1;
    ADC_HandleTypeDef* temp = hadc1;
    hadc1 = NULL;

    int result = PositionSensing_Read();

    TEST_ASSERT_EQUAL(-1, result);

    hadc1 = temp;
}
