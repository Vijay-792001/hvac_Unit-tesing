#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_PositionSensing_Get_Success(void) {
    ADC_HandleTypeDef hadc_stub = {0};
    HAL_ADC_Start_ExpectAndReturn(&hadc_stub, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc_stub, 100, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc_stub, 2000);
    HAL_ADC_Stop_ExpectAndReturn(&hadc_stub, HAL_OK);
    position_t pos = {0};
    int ret = position_sensing_get(&pos);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(2000, pos.raw_adc);
}

void test_PositionSensing_Get_ADCStartFailure(void) {
    ADC_HandleTypeDef hadc_stub = {0};
    HAL_ADC_Start_ExpectAndReturn(&hadc_stub, HAL_ERROR);
    position_t pos = {0};
    int ret = position_sensing_get(&pos);
    TEST_ASSERT_EQUAL(-1, ret);
}

void test_PositionSensing_Get_ADCPollForConversionFailure(void) {
    ADC_HandleTypeDef hadc_stub = {0};
    HAL_ADC_Start_ExpectAndReturn(&hadc_stub, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc_stub, 100, HAL_ERROR);
    HAL_ADC_Stop_ExpectAndReturn(&hadc_stub, HAL_OK);
    position_t pos = {0};
    int ret = position_sensing_get(&pos);
    TEST_ASSERT_EQUAL(-2, ret);
}

void test_PositionSensing_Get_ADCStopFailure(void) {
    ADC_HandleTypeDef hadc_stub = {0};
    HAL_ADC_Start_ExpectAndReturn(&hadc_stub, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc_stub, 100, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc_stub, 1234);
    HAL_ADC_Stop_ExpectAndReturn(&hadc_stub, HAL_ERROR);
    position_t pos = {0};
    int ret = position_sensing_get(&pos);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(1234, pos.raw_adc);
}

void test_PositionSensing_Get_NullPointer(void) {
    int ret = position_sensing_get(NULL);
    TEST_ASSERT_EQUAL(-3, ret);
}
