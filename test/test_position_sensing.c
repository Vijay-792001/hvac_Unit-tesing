/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/* TC-PS-01: Test initialization triggers correct ADC configuration */
void test_PositionSensing_Init_HalAdcInitCalled(void)
{
    HAL_ADC_Init_ExpectAnyArgsAndReturn(HAL_OK);
    TEST_ASSERT_EQUAL(POSITION_OK, PositionSensing_Init());
}

/* TC-PS-02: Test initialization detects HAL failure */
void test_PositionSensing_Init_HalAdcFails_ReturnsError(void)
{
    HAL_ADC_Init_ExpectAnyArgsAndReturn(HAL_ERROR);
    TEST_ASSERT_EQUAL(POSITION_ERROR, PositionSensing_Init());
}

/* TC-PS-03: Test reading position triggers correct ADC start/stop */
void test_PositionSensing_Read_StartsAndStopsAdcProperly(void)
{
    HAL_ADC_Start_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_ADC_PollForConversion_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_ADC_GetValue_ExpectAnyArgsAndReturn(789U);
    HAL_ADC_Stop_ExpectAnyArgsAndReturn(HAL_OK);
    TEST_ASSERT_EQUAL(789, PositionSensing_Read());
}

/* TC-PS-04: Test read detects ADC conversion failure */
void test_PositionSensing_Read_ConversionFails_ReturnsError(void)
{
    HAL_ADC_Start_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_ADC_PollForConversion_ExpectAnyArgsAndReturn(HAL_ERROR);
    HAL_ADC_Stop_ExpectAnyArgsAndReturn(HAL_OK);
    TEST_ASSERT_EQUAL(POSITION_ERROR, PositionSensing_Read());
}

/* TC-PS-05: Test read when ADC start fails */
void test_PositionSensing_Read_AdcStartFails_ReturnsError(void)
{
    HAL_ADC_Start_ExpectAnyArgsAndReturn(HAL_ERROR);
    TEST_ASSERT_EQUAL(POSITION_ERROR, PositionSensing_Read());
}

/* TC-PS-06: Test read when ADC stop fails (should still return value) */
void test_PositionSensing_Read_StopFails_ReturnsValue(void)
{
    HAL_ADC_Start_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_ADC_PollForConversion_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_ADC_GetValue_ExpectAnyArgsAndReturn(456U);
    HAL_ADC_Stop_ExpectAnyArgsAndReturn(HAL_ERROR);
    TEST_ASSERT_EQUAL(456, PositionSensing_Read());
}

/* TC-PS-07: Test PositionSensing_Read with NULL ADC handle */
void test_PositionSensing_Read_NullHandle_ReturnsError(void)
{
    // Simulates NULL handle (if API supports it)
    TEST_ASSERT_EQUAL(POSITION_ERROR, PositionSensing_Read_NullHandle());
}
