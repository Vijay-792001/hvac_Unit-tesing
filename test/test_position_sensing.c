#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/*
Test Plan ID: TC_PS_01
Description: Test PositionSensing_Init initializes HAL ADC and state.
*/
void test_PositionSensing_Init_should_initialize_HAL_ADC_TC_PS_01(void)
{
    HAL_ADC_Init_ExpectAndReturn(HAL_OK);

    TEST_ASSERT_EQUAL(POSITION_SENSING_OK, PositionSensing_Init());
}

/*
Test Plan ID: TC_PS_02
Description: Test PositionSensing_Init failure path.
*/
void test_PositionSensing_Init_should_return_error_on_HAL_failure_TC_PS_02(void)
{
    HAL_ADC_Init_ExpectAndReturn(HAL_ERROR);

    TEST_ASSERT_EQUAL(POSITION_SENSING_ERROR, PositionSensing_Init());
}

/*
Test Plan ID: TC_PS_03
Description: Test PositionSensing_GetPosition reads position successfully.
*/
void test_PositionSensing_GetPosition_should_read_position_success_TC_PS_03(void)
{
    int16_t pos = 1234;
    HAL_ADC_Read_ExpectAndReturn(&pos, HAL_OK);

    TEST_ASSERT_EQUAL(POSITION_SENSING_OK, PositionSensing_GetPosition(&pos));
}

/*
Test Plan ID: TC_PS_04
Description: Test PositionSensing_GetPosition handles HAL ADC read failure.
*/
void test_PositionSensing_GetPosition_should_return_error_on_HAL_failure_TC_PS_04(void)
{
    int16_t pos;
    HAL_ADC_Read_ExpectAndReturn(&pos, HAL_ERROR);

    TEST_ASSERT_EQUAL(POSITION_SENSING_ERROR, PositionSensing_GetPosition(&pos));
}

/*
Test Plan ID: TC_PS_05
Description: Test PositionSensing_GetPosition handles NULL pointer argument.
*/
void test_PositionSensing_GetPosition_should_return_error_on_null_pointer_TC_PS_05(void)
{
    TEST_ASSERT_EQUAL(POSITION_SENSING_INVALID_PARAM, PositionSensing_GetPosition(NULL));
}

/*
Test Plan ID: TC_PS_06
Description: Test boundary value at both extremes within allowed sensor range.
*/
void test_PositionSensing_GetPosition_should_handle_minimum_and_maximum_TC_PS_06(void)
{
    int16_t min = POSITION_SENSOR_MIN;
    HAL_ADC_Read_ExpectAndReturn(&min, HAL_OK);
    TEST_ASSERT_EQUAL(POSITION_SENSING_OK, PositionSensing_GetPosition(&min));

    int16_t max = POSITION_SENSOR_MAX;
    HAL_ADC_Read_ExpectAndReturn(&max, HAL_OK);
    TEST_ASSERT_EQUAL(POSITION_SENSING_OK, PositionSensing_GetPosition(&max));
}
