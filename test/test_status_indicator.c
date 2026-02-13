#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_StatusIndicator_Update_SetIdle_LightsProperLED(void)
{
    HAL_GPIO_WritePin_Expect(LED_PORT, LED_GREEN_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(LED_PORT, LED_YELLOW_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(LED_PORT, LED_RED_PIN, GPIO_PIN_RESET);
    StatusIndicator_Update(STATUS_IDLE);
}

void test_StatusIndicator_Update_SetBusy_LightsProperLED(void)
{
    HAL_GPIO_WritePin_Expect(LED_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(LED_PORT, LED_YELLOW_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(LED_PORT, LED_RED_PIN, GPIO_PIN_RESET);
    StatusIndicator_Update(STATUS_BUSY);
}

void test_StatusIndicator_Update_SetError_LightsProperLED(void)
{
    HAL_GPIO_WritePin_Expect(LED_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(LED_PORT, LED_YELLOW_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(LED_PORT, LED_RED_PIN, GPIO_PIN_SET);
    StatusIndicator_Update(STATUS_ERROR);
}

void test_StatusIndicator_Update_BadState_LightsNone(void)
{
    Status_t bad_status = (Status_t)99;
    StatusIndicator_Update(bad_status);
}
