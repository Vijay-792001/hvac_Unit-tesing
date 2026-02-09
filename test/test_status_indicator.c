#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/* Test ID: STIN-1 Turn on status indicator (green) */
void test_StatusIndicator_SetStatus_Green_ShouldSetPinHigh(void)
{
    StatusIndicator_SetStatus(STATUS_GREEN);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    StatusIndicator_Update();
}

/* Test ID: STIN-2 Turn off status indicator */
void test_StatusIndicator_SetStatus_Off_ShouldSetPinLow(void)
{
    StatusIndicator_SetStatus(STATUS_OFF);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    StatusIndicator_Update();
}

/* Test ID: STIN-3 Boundary: set invalid status */
void test_StatusIndicator_SetStatus_InvalidEnum_DoesNothing(void)
{
    StatusIndicator_SetStatus((StatusIndicator_State)0xFF);
    // No calls expected: indicator not updated for invalid
    StatusIndicator_Update();
}

/* Test ID: STIN-4 NULL pointer passed (API takes enum, not pointer) – N/A */

/* Test ID: STIN-5 Repeated update with same status (no redundant HAL calls) */
void test_StatusIndicator_RepeatUpdate_SameStatus_NoExtraCalls(void)
{
    StatusIndicator_SetStatus(STATUS_GREEN);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    StatusIndicator_Update();
    // Call update again with no status change; expect no calls
    StatusIndicator_Update();
}

/* Test ID: STIN-6 Change status from on to off */
void test_StatusIndicator_StatusChange_GreenToOff_ShouldSetLow(void)
{
    StatusIndicator_SetStatus(STATUS_GREEN);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    StatusIndicator_Update();
    StatusIndicator_SetStatus(STATUS_OFF);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    StatusIndicator_Update();
}
