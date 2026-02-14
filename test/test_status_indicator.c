#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_StatusIndicator_SetOn_CallsHALSetAndReturnsOK(void)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    HAL_StatusIndicator_Set_ExpectAndReturn(STATUS_ON, hal_status);
    StatusIndicatorStatus_t ret = StatusIndicator_Set(STATUS_ON);
    TEST_ASSERT_EQUAL(STATUS_INDICATOR_OK, ret);
}

void test_StatusIndicator_SetOff_CallsHALSetAndReturnsOK(void)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    HAL_StatusIndicator_Set_ExpectAndReturn(STATUS_OFF, hal_status);
    StatusIndicatorStatus_t ret = StatusIndicator_Set(STATUS_OFF);
    TEST_ASSERT_EQUAL(STATUS_INDICATOR_OK, ret);
}

void test_StatusIndicator_SetInvalidStatus_ReturnsInvalid(void)
{
    StatusIndicatorStatus_t ret = StatusIndicator_Set(42);
    TEST_ASSERT_EQUAL(STATUS_INDICATOR_INVALID, ret);
}

void test_StatusIndicator_HALReturnsError_OnSet(void)
{
    HAL_StatusTypeDef hal_status = HAL_ERROR;
    HAL_StatusIndicator_Set_ExpectAndReturn(STATUS_ON, hal_status);
    StatusIndicatorStatus_t ret = StatusIndicator_Set(STATUS_ON);
    TEST_ASSERT_EQUAL(STATUS_INDICATOR_HAL_ERROR, ret);
}

void test_StatusIndicator_NullPointer_NotApplicable(void)
{
    TEST_PASS();
}

void test_StatusIndicator_Initialize_SetsStatusOFF(void)
{
    HAL_StatusIndicator_Set_ExpectAndReturn(STATUS_OFF, HAL_OK);
    StatusIndicator_Initialize();
}
