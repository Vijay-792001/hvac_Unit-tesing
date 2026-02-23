/* ===== test_status_indicator.c ===== */
#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

static GPIO_TypeDef dummy_GPIOB;
GPIO_TypeDef *const LED_GPIO_Port = &dummy_GPIOB;

void setUp(void)
{
    // No module state persistence to reset
}

void tearDown(void)
{
}

/* SI_01: Indicator_On turns the indicator on (sets GPIO_PIN_SET) */
void test_SI_01_Indicator_On_sets_GPIO_HIGH(void)
{
    HAL_GPIO_WritePin_Expect(LED_GPIO_Port, STATUS_INDICATOR_PIN, GPIO_PIN_SET);
    StatusIndicator_On();
}

/* SI_02: Indicator_Off turns the indicator off (sets GPIO_PIN_RESET) */
void test_SI_02_Indicator_Off_sets_GPIO_LOW(void)
{
    HAL_GPIO_WritePin_Expect(LED_GPIO_Port, STATUS_INDICATOR_PIN, GPIO_PIN_RESET);
    StatusIndicator_Off();
}

/* SI_03: Indicator_Toggle toggles current indicator state */
void test_SI_03_Indicator_Toggle_performs_GPIO_Toggle(void)
{
    HAL_GPIO_TogglePin_Expect(LED_GPIO_Port, STATUS_INDICATOR_PIN);
    StatusIndicator_Toggle();
}

/* SI_04: Indicator_Blink calls On, then Off with a delay between */
void test_SI_04_Indicator_Blink_On_Off_Delay(void)
{
    HAL_GPIO_WritePin_Expect(LED_GPIO_Port, STATUS_INDICATOR_PIN, GPIO_PIN_SET);
    HAL_Delay_Expect(STATUS_INDICATOR_BLINK_DELAY_MS);
    HAL_GPIO_WritePin_Expect(LED_GPIO_Port, STATUS_INDICATOR_PIN, GPIO_PIN_RESET);
    StatusIndicator_Blink();
}

/* SI_05: Indicator_On is idempotent */
void test_SI_05_Indicator_On_idempotent(void)
{
    // Should simply set pin high each call
    HAL_GPIO_WritePin_Expect(LED_GPIO_Port, STATUS_INDICATOR_PIN, GPIO_PIN_SET);
    StatusIndicator_On();

    HAL_GPIO_WritePin_Expect(LED_GPIO_Port, STATUS_INDICATOR_PIN, GPIO_PIN_SET);
    StatusIndicator_On();
}

/* SI_06: Indicator_Off is idempotent */
void test_SI_06_Indicator_Off_idempotent(void)
{
    // Should simply set pin low each call
    HAL_GPIO_WritePin_Expect(LED_GPIO_Port, STATUS_INDICATOR_PIN, GPIO_PIN_RESET);
    StatusIndicator_Off();

    HAL_GPIO_WritePin_Expect(LED_GPIO_Port, STATUS_INDICATOR_PIN, GPIO_PIN_RESET);
    StatusIndicator_Off();
}

/* SI_07: Indicator_On/Off with NULL port pointer should not call HAL */
void test_SI_07_Indicator_OnOff_NULL_Port_pointer_does_not_call_HAL(void)
{
    // Change LED_GPIO_Port to NULL and call, expect no call to HAL_GPIO_WritePin
    GPIO_TypeDef *const orig_port = LED_GPIO_Port;

    // Test On
    *((GPIO_TypeDef **)&LED_GPIO_Port) = NULL;
    StatusIndicator_On();

    // Test Off
    StatusIndicator_Off();

    // Restore pointer for subsequent tests
    *((GPIO_TypeDef **)&LED_GPIO_Port) = orig_port;
}

/* SI_08: Indicator_Toggle with NULL port pointer should not call HAL */
void test_SI_08_Indicator_Toggle_NULL_Port_pointer_does_not_call_HAL(void)
{
    GPIO_TypeDef *const orig_port = LED_GPIO_Port;

    *((GPIO_TypeDef **)&LED_GPIO_Port) = NULL;
    StatusIndicator_Toggle();

    // Restore pointer for subsequent tests
    *((GPIO_TypeDef **)&LED_GPIO_Port) = orig_port;
}

/* SI_09: Indicator_Blink with NULL port pointer should not call HAL or Delay */
void test_SI_09_Indicator_Blink_NULL_Port_pointer_does_not_call_HAL(void)
{
    GPIO_TypeDef *const orig_port = LED_GPIO_Port;

    *((GPIO_TypeDef **)&LED_GPIO_Port) = NULL;
    StatusIndicator_Blink();

    *((GPIO_TypeDef **)&LED_GPIO_Port) = orig_port;
}

/* SI_10: Blink idempotent, multiple rapid calls behave as expected */
void test_SI_10_Indicator_Blink_multiple_calls_okay(void)
{
    // Two successive calls: On, Delay, Off (x2)
    HAL_GPIO_WritePin_Expect(LED_GPIO_Port, STATUS_INDICATOR_PIN, GPIO_PIN_SET);
    HAL_Delay_Expect(STATUS_INDICATOR_BLINK_DELAY_MS);
    HAL_GPIO_WritePin_Expect(LED_GPIO_Port, STATUS_INDICATOR_PIN, GPIO_PIN_RESET);

    HAL_GPIO_WritePin_Expect(LED_GPIO_Port, STATUS_INDICATOR_PIN, GPIO_PIN_SET);
    HAL_Delay_Expect(STATUS_INDICATOR_BLINK_DELAY_MS);
    HAL_GPIO_WritePin_Expect(LED_GPIO_Port, STATUS_INDICATOR_PIN, GPIO_PIN_RESET);

    StatusIndicator_Blink();
    StatusIndicator_Blink();
}
