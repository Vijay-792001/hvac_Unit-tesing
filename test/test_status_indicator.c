#include "unity.h"
#include "status_indicator.h"
#include "mock_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_StatusIndicator_Set_On_ShouldCallHALAndSucceed(void)
{
    Indicator_t indicator = INDICATOR_POWER;
    IndicatorState_t state = INDICATOR_STATE_ON;
    mock_HAL_SetIndicator_ExpectAndReturn(indicator, state, HAL_OK);
    int result = StatusIndicator_Set(indicator, state);
    TEST_ASSERT_EQUAL(0, result);
}

void test_StatusIndicator_Set_Off_ShouldCallHALAndSucceed(void)
{
    Indicator_t indicator = INDICATOR_ERROR;
    IndicatorState_t state = INDICATOR_STATE_OFF;
    mock_HAL_SetIndicator_ExpectAndReturn(indicator, state, HAL_OK);
    int result = StatusIndicator_Set(indicator, state);
    TEST_ASSERT_EQUAL(0, result);
}

void test_StatusIndicator_Set_HAL_Error_ShouldReturnError(void)
{
    Indicator_t indicator = INDICATOR_POWER;
    IndicatorState_t state = INDICATOR_STATE_ON;
    mock_HAL_SetIndicator_ExpectAndReturn(indicator, state, HAL_ERROR);
    int result = StatusIndicator_Set(indicator, state);
    TEST_ASSERT_EQUAL(-2, result);
}

void test_StatusIndicator_Set_InvalidIndicatorID_ShouldReturnError(void)
{
    Indicator_t indicator = (Indicator_t)99;
    IndicatorState_t state = INDICATOR_STATE_ON;
    int result = StatusIndicator_Set(indicator, state);
    TEST_ASSERT_EQUAL(-1, result);
}

void test_StatusIndicator_Set_NullSafeHandling_ShouldReturnError(void)
{
    StatusIndicator_t* indicator_ptr = NULL;
    int result = StatusIndicator_SetPtr(indicator_ptr, INDICATOR_STATE_ON);
    TEST_ASSERT_EQUAL(-3, result);
}
