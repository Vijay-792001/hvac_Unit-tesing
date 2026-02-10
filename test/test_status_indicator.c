#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void test_StatusIndicator_SetOn_ShouldWriteHighToPin(void)
{
    HAL_GPIO_WritePin_Expect(GPIO_INDICATOR_PORT, GPIO_INDICATOR_PIN, GPIO_PIN_SET);
    int result = StatusIndicator_SetOn();
    TEST_ASSERT_EQUAL(0, result);
}

void test_StatusIndicator_SetOff_ShouldWriteLowToPin(void)
{
    HAL_GPIO_WritePin_Expect(GPIO_INDICATOR_PORT, GPIO_INDICATOR_PIN, GPIO_PIN_RESET);
    int result = StatusIndicator_SetOff();
    TEST_ASSERT_EQUAL(0, result);
}

void test_StatusIndicator_NullPort_ShouldReturnError(void)
{
    StatusIndicator_InjectPortPin(NULL, GPIO_INDICATOR_PIN);
    int result = StatusIndicator_SetOn();
    TEST_ASSERT_EQUAL(-1, result);
}

void test_StatusIndicator_Toggle_ShouldTogglePin(void)
{
    HAL_GPIO_TogglePin_Expect(GPIO_INDICATOR_PORT, GPIO_INDICATOR_PIN);
    int result = StatusIndicator_Toggle();
    TEST_ASSERT_EQUAL(0, result);
}
