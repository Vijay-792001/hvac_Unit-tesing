/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"
// TEST PLAN: TC-PS-001 - Get current position, normal
void test_PositionSensing_GetPosition_Normal_ShouldReturnValue(void) {
    HAL_ADC_Read_ExpectAndReturn(ADC_CHANNEL_POSITION, 50);
    int pos = position_sensing_get_position();
    TEST_ASSERT_EQUAL(50, pos);
}
// TEST PLAN: TC-PS-002 - Out-of-range low ADC value
void test_PositionSensing_GetPosition_ADCTooLow_ShouldClampZero(void) {
    HAL_ADC_Read_ExpectAndReturn(ADC_CHANNEL_POSITION, -12);
    int pos = position_sensing_get_position();
    TEST_ASSERT_EQUAL(0, pos);
}
// TEST PLAN: TC-PS-003 - Out-of-range high ADC value
void test_PositionSensing_GetPosition_ADCHigh_ShouldClampMax(void) {
    HAL_ADC_Read_ExpectAndReturn(ADC_CHANNEL_POSITION, 1025);
    int pos = position_sensing_get_position();
    TEST_ASSERT_EQUAL(100, pos);
}
// TEST PLAN: TC-PS-004 - HAL returns error code
void test_PositionSensing_GetPosition_HALError_ShouldReturnError(void) {
    HAL_ADC_Read_ExpectAndReturn(ADC_CHANNEL_POSITION, -1000); // convention: < 0 is error
    int pos = position_sensing_get_position();
    TEST_ASSERT_EQUAL(PS_ERR_HAL, pos);
}
