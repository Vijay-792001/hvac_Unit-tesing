/* ===== test_status_indicator.c ===== */
#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

extern GPIO_TypeDef GPIOC_inst;
#define LED_POWER_PORT GPIOC
#define LED_POWER_PIN  GPIO_PIN_0

static const uint16_t led_pos_pins[STATUS_INDICATOR_NUM] = {
    GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5
};

/* === SI_01 (SWE-REQ-025): Power LED turns ON after init === */
void test_StatusIndicator_Init_PowerLedOn_AllPositionLedsOff(void)
{
    GPIO_InitTypeDef dummy_init; // only for signature, content not checked

    // GPIO_Init called with full mask for all LEDs
    HAL_GPIO_Init_Expect(LED_POWER_PORT, &dummy_init);

    // Power LED ON, all others OFF in provided order
    HAL_GPIO_WritePin_Expect(LED_POWER_PORT, LED_POWER_PIN, GPIO_PIN_SET);
    for(uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(LED_POWER_PORT, led_pos_pins[i], GPIO_PIN_RESET);
    }

    StatusIndicator_Init();
}

/* === SI_02 (SWE-REQ-026): Position 0 shows no green LED === */
void test_StatusIndicator_Update_Pos0_AllPositionLedsOff(void)
{
    // All green LEDs OFF (RESET), none SET
    for(uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(LED_POWER_PORT, led_pos_pins[i], GPIO_PIN_RESET);
    }
    StatusIndicator_Update(1, 0);
}

/* === SI_03 (SWE-REQ-027): Display position 5 only === */
void test_StatusIndicator_Update_Pos5_OnlyPos5LedOn(void)
{
    // RESET all position LEDs first
    for(uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(LED_POWER_PORT, led_pos_pins[i], GPIO_PIN_RESET);
    }
    // SET only LED 5 (index 4) if valid and pos==5
    HAL_GPIO_WritePin_Expect(LED_POWER_PORT, GPIO_PIN_5, GPIO_PIN_SET);

    StatusIndicator_Update(1, 5);
}

/* === SI_04 (SWE-REQ-029): Invalid position → all OFF === */
void test_StatusIndicator_Update_Invalid_AllPositionLedsOff(void)
{
    // All green LEDs OFF (RESET), none SET
    for(uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(LED_POWER_PORT, led_pos_pins[i], GPIO_PIN_RESET);
    }
    StatusIndicator_Update(0, 0xFF);
}

/* === SI_05 (SWE-REQ-044): Power LED software control === */
void test_StatusIndicator_SetPowerLED_OffThenOn(void)
{
    // OFF
    HAL_GPIO_WritePin_Expect(LED_POWER_PORT, LED_POWER_PIN, GPIO_PIN_RESET);
    StatusIndicator_SetPowerLED(0);

    // ON
    HAL_GPIO_WritePin_Expect(LED_POWER_PORT, LED_POWER_PIN, GPIO_PIN_SET);
    StatusIndicator_SetPowerLED(1);
}

/* === SI_06 (SWE-REQ-024): Out-of-range pos → all LEDs off === */
void test_StatusIndicator_Update_OutOfRangePosition_AllLedsOff(void)
{
    // All green LEDs OFF (RESET), none SET
    for(uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(LED_POWER_PORT, led_pos_pins[i], GPIO_PIN_RESET);
    }
    StatusIndicator_Update(1, 6); // pos=6 is OOR
}
