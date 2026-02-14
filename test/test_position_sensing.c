#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_PositionSensing_Init_Should_Call_HALADCStart(void) {
    HAL_ADC_Start_Expect(&hadc1);
    PositionSensing_Init();
}

void test_PositionSensing_ReadPosition_Should_ReturnPosition_WhenValid(void) {
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3050);
    int pos = PositionSensing_ReadPosition();
    TEST_ASSERT_EQUAL(3050, pos);
}

void test_PositionSensing_ReadPosition_Should_ReturnErrorOnTimeout(void) {
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10, HAL_TIMEOUT);
    int pos = PositionSensing_ReadPosition();
    TEST_ASSERT_EQUAL(-1, pos);
}

void test_PositionSensing_Deinit_Should_Call_ADC_Stop(void) {
    HAL_ADC_Stop_Expect(&hadc1);
    PositionSensing_Deinit();
}
