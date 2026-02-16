#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

// Test Case: SI_001 - Status indicator ON success
void test_StatusIndicator_TurnOn_Successful_ReturnsOK(void)
{
    HAL_Status_Indicator_On_ExpectAndReturn(HAL_OK);

    StatusIndicatorStatus_t status = StatusIndicator_TurnOn();

    TEST_ASSERT_EQUAL(STATUS_INDICATOR_OK, status);
}

// Test Case: SI_002 - Status indicator ON HAL error
void test_StatusIndicator_TurnOn_HALError_ReturnsHWError(void)
{
    HAL_Status_Indicator_On_ExpectAndReturn(HAL_ERROR);

    StatusIndicatorStatus_t status = StatusIndicator_TurnOn();

    TEST_ASSERT_EQUAL(STATUS_INDICATOR_HW_ERROR, status);
}

// Test Case: SI_003 - Status indicator OFF success
void test_StatusIndicator_TurnOff_Successful_ReturnsOK(void)
{
    HAL_Status_Indicator_Off_ExpectAndReturn(HAL_OK);

    StatusIndicatorStatus_t status = StatusIndicator_TurnOff();

    TEST_ASSERT_EQUAL(STATUS_INDICATOR_OK, status);
}

// Test Case: SI_004 - Status indicator OFF HAL error
void test_StatusIndicator_TurnOff_HALError_ReturnsHWError(void)
{
    HAL_Status_Indicator_Off_ExpectAndReturn(HAL_ERROR);

    StatusIndicatorStatus_t status = StatusIndicator_TurnOff();

    TEST_ASSERT_EQUAL(STATUS_INDICATOR_HW_ERROR, status);
}
