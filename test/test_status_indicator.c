#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_StatusIndicator_SetState_On(void)
{
    StatusIndicatorState_t state = STATUS_INDICATOR_ON;
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);

    int result = StatusIndicator_SetState(state);
    TEST_ASSERT_EQUAL(STATUS_INDICATOR_SUCCESS, result);
}

void test_StatusIndicator_SetState_Off(void)
{
    StatusIndicatorState_t state = STATUS_INDICATOR_OFF;
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

    int result = StatusIndicator_SetState(state);
    TEST_ASSERT_EQUAL(STATUS_INDICATOR_SUCCESS, result);
}

void test_StatusIndicator_SetState_InvalidState(void)
{
    StatusIndicatorState_t state = (StatusIndicatorState_t)99;

    int result = StatusIndicator_SetState(state);
    TEST_ASSERT_EQUAL(STATUS_INDICATOR_INVALID_STATE, result);
}

void test_StatusIndicator_SetState_HalWritePinFails(void)
{
    StatusIndicatorState_t state = STATUS_INDICATOR_ON;
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);

    int result = StatusIndicator_SetState(state);
    TEST_ASSERT_EQUAL(STATUS_INDICATOR_SUCCESS, result);
}
