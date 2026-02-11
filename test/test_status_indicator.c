/* ===== test_status_indicator.c ===== */
#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/* TC-SI-01: Test StatusIndicator_Init calls HAL GPIO config */
void test_StatusIndicator_Init_HalGpioInitCalled(void)
{
    HAL_GPIO_Init_ExpectAnyArgs();
    StatusIndicator_Init();
}

/* TC-SI-02: Test StatusIndicator_Set calls HAL_GPIO_WritePin for ON */
void test_StatusIndicator_Set_On_CallsHalGpioWriteHigh(void)
{
    HAL_GPIO_WritePin_ExpectAnyArgs();
    StatusIndicator_Set(STATUS_ON);
}

/* TC-SI-03: Test StatusIndicator_Set calls HAL_GPIO_WritePin for OFF */
void test_StatusIndicator_Set_Off_CallsHalGpioWriteLow(void)
{
    HAL_GPIO_WritePin_ExpectAnyArgs();
    StatusIndicator_Set(STATUS_OFF);
}

/* TC-SI-04: Test StatusIndicator_Blink blinks LED through HAL */
void test_StatusIndicator_Blink_CallsHalGpioSequence(void)
{
    HAL_GPIO_WritePin_ExpectAnyArgs(); // ON
    HAL_Delay_ExpectAnyArgs();
    HAL_GPIO_WritePin_ExpectAnyArgs(); // OFF
    HAL_Delay_ExpectAnyArgs();
    StatusIndicator_Blink();
}

/* TC-SI-05: Test StatusIndicator_Set ignores invalid argument */
void test_StatusIndicator_Set_InvalidArgument_NoAction(void)
{
    // Should not call HAL
    StatusIndicator_Set(99); // Unknown state
}

/* TC-SI-06: Test StatusIndicator_Blink handles HAL failures gracefully */
void test_StatusIndicator_Blink_HalError_Ignored(void)
{
    HAL_GPIO_WritePin_ExpectAnyArgs();
    HAL_Delay_ExpectAnyArgsAndReturn(HAL_ERROR);
    HAL_GPIO_WritePin_ExpectAnyArgs();
    HAL_Delay_ExpectAnyArgsAndReturn(HAL_ERROR);
    StatusIndicator_Blink();
}
