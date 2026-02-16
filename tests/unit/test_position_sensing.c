#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

// Test Case: PS_001 - Init position sensor successfully
void test_PositionSensing_Init_SuccessHAL_ReturnsOK(void)
{
    HAL_PositionSensor_Init_ExpectAndReturn(HAL_OK);
    PositionStatus_t status = PositionSensing_Init();
    TEST_ASSERT_EQUAL(POSITION_STATUS_OK, status);
}

// Test Case: PS_002 - Init position sensor HAL failure path
void test_PositionSensing_Init_HALFailure_ReturnsHWError(void)
{
    HAL_PositionSensor_Init_ExpectAndReturn(HAL_ERROR);
    PositionStatus_t status = PositionSensing_Init();
    TEST_ASSERT_EQUAL(POSITION_STATUS_HW_ERROR, status);
}

// Test Case: PS_003 - Get position with valid pointer and valid HAL value
void test_PositionSensing_GetPosition_ValidPointer_CopiesPosition_ReturnsOK(void)
{
    HAL_Position_Read_ExpectAndReturn(123);
    uint16_t pos = 0;
    PositionStatus_t status = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL(POSITION_STATUS_OK, status);
    TEST_ASSERT_EQUAL(123, pos);
}

// Test Case: PS_004 - Get position with NULL pointer
void test_PositionSensing_GetPosition_NullPointer_ReturnsError(void)
{
    PositionStatus_t status = PositionSensing_GetPosition(NULL);
    TEST_ASSERT_EQUAL(POSITION_STATUS_NULL_PTR, status);
}

// Test Case: PS_005 - Get position with HAL error
void test_PositionSensing_GetPosition_HALError_ReturnsHWError(void)
{
    HAL_Position_Read_ExpectAndReturn(-1); // Indicates HAL read failure
    uint16_t pos = 0;
    PositionStatus_t status = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL(POSITION_STATUS_HW_ERROR, status);
}
