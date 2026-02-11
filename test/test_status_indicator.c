#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void test_StatusIndicator_Set_LED_ON_ShouldSetGPIOPin(void)
{
    HAL_GPIO_WritePin_Expect(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_SET);
    int result = StatusIndicator_Set(STATUS_INDICATOR_ON);
    TEST_ASSERT_EQUAL_INT(STATUS_INDICATOR_SUCCESS, result);
}

void test_StatusIndicator_Set_LED_OFF_ShouldResetGPIOPin(void)
{
    HAL_GPIO_WritePin_Expect(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_RESET);
    int result = StatusIndicator_Set(STATUS_INDICATOR_OFF);
    TEST_ASSERT_EQUAL_INT(STATUS_INDICATOR_SUCCESS, result);
}

void test_StatusIndicator_Set_InvalidState_ShouldReturnError(void)
{
    int result = StatusIndicator_Set((StatusIndicatorState_t)99);
    TEST_ASSERT_EQUAL_INT(STATUS_INDICATOR_ERROR_INVALID_STATE, result);
}

void test_StatusIndicator_Toggle_On_ShouldTurnOff(void)
{
    HAL_GPIO_TogglePin_Expect(LED_GPIO_PORT);
    StatusIndicatorState_t initial = STATUS_INDICATOR_ON;
    int result = StatusIndicator_Toggle(&initial);
    TEST_ASSERT_EQUAL_INT(STATUS_INDICATOR_SUCCESS, result);
}

void test_StatusIndicator_Toggle_NullPointer_ShouldReturnError(void)
{
    int result = StatusIndicator_Toggle(NULL);
    TEST_ASSERT_EQUAL_INT(STATUS_INDICATOR_ERROR_NULL, result);
}
