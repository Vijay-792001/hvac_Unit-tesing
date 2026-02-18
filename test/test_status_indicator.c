/* ===== test_status_indicator.c ===== */

#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

// Provide a dummy global GPIOC instance for correct linkage
GPIO_TypeDef GPIOC_inst;
GPIO_TypeDef *GPIOC = &GPIOC_inst;

// StatusIndicator uses 5 LEDs for position (GPIO_PIN_1...GPIO_PIN_5) and 1 Power LED (GPIO_PIN_0)
#define LED_POWER_PIN   GPIO_PIN_0
#define LED_POS_PINS    {GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5}

void setUp(void)
{
    // No persistent state to reset (handled in module).
}

void tearDown(void)
{
}

/* SI_01: Power LED turns ON after init
   Expect: Power LED ON (PIN0 SET), all position LEDs OFF (PIN1..5 RESET) */
void test_SI_01_StatusIndicator_Init_sets_power_led_on_and_position_leds_off(void)
{
    GPIO_InitTypeDef any_struct;

    // GPIO_init called with correct arguments (not checked strictly here)
    HAL_GPIO_Init_Expect(GPIOC, (GPIO_InitTypeDef*)&any_struct);

    // Power LED ON
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);

    // All five position LEDs OFF, in order
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Init();
}

/* SI_02: Position 0 shows no green LED
   Input: valid=1, pos=0. All position LEDs must be OFF (PIN1..5 RESET, no SET) */
void test_SI_02_StatusIndicator_Update_with_position_0_shows_no_position_led_on(void)
{
    // All position LEDs OFF (RESET)
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Update(1, 0); // valid=1, position=0
}

/* SI_03: Display position 5
   Input: valid=1, pos=5. Only pos LED5 (PIN5) ON; all OFF, then PIN5 SET. */
void test_SI_03_StatusIndicator_Update_displays_position_5_only(void)
{
    // All position LEDs OFF (RESET)
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    // Only position 5 LED SET (logical_pos=5  LED4/index=4, GPIO_PIN_5)
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);

    StatusIndicator_Update(1, 5); // valid=1, position=5
}

/* SI_04: Invalid position  all LEDs OFF
   Input: valid=0, pos=0xFF. All position LEDs must be OFF (PIN1..5 RESET), no SET. */
void test_SI_04_StatusIndicator_Update_invalid_position_all_leds_off(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Update(0, 0xFF);
}

/* SI_05: Power LED software control.
   Sequence: SetPowerLED(0) then SetPowerLED(1). PIN0 RESET then PIN0 SET. */
void test_SI_05_StatusIndicator_SetPowerLED_off_then_on(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    StatusIndicator_SetPowerLED(0);

    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    StatusIndicator_SetPowerLED(1);
}

/* SI_06: Boundary out-of-range (pos=6)  all OFF.
   Input: valid=1, pos=6. All position LEDs must be OFF (PIN1..5 RESET), no SET. */
void test_SI_06_StatusIndicator_Update_out_of_range_pos_6_all_leds_off(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Update(1, 6); // out-of-range position
}
