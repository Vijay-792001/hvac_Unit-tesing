#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

static PSensor_t sensor;

void setUp(void)
{
    memset(&sensor, 0, sizeof(sensor));
}

void tearDown(void)
{
}

void test_PositionSensing_ReadSensor_Valid(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1234);

    int value = PositionSensing_ReadSensor(&sensor);

    TEST_ASSERT_EQUAL(1234, value);
}

void test_PositionSensing_ReadSensor_HALPollError(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_TIMEOUT);

    int value = PositionSensing_ReadSensor(&sensor);

    TEST_ASSERT_EQUAL(-1, value);
}

void test_PositionSensing_ReadSensor_NullSensor(void)
{
    int value = PositionSensing_ReadSensor(NULL);

    TEST_ASSERT_EQUAL(-2, value);
}

void test_PositionSensing_ReadSensor_LowBoundary(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 0);

    int value = PositionSensing_ReadSensor(&sensor);

    TEST_ASSERT_EQUAL(0, value);
}

void test_PositionSensing_ReadSensor_HighBoundary(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4095);

    int value = PositionSensing_ReadSensor(&sensor);

    TEST_ASSERT_EQUAL(4095, value);
}
