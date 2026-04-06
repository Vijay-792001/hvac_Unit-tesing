/* ===== test_status_indicator.c ===== */
#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"
#include <string.h>

/* Provide fake instances if required externally by the module */
GPIO_TypeDef GPIOC_inst;
GPIO_TypeDef *GPIOC = &GPIOC_inst;

/* Test fixture: Unity/Ceedling basic hooks */
void setUp(void)
{
    mock_stm32f4xx_hal_Init();
}

void tearDown(void)
{
}

/* --- SI_01: Power LED turns ON after init ---
   Power LED ON, all position LEDs OFF.
   Expects:
     - HAL_GPIO_Init (GPIOC, &GPIO_InitStruct)
     - HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET)
     - HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET)
     - HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET)
     - HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET)
     - HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET)
     - HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET)
 */
void test_SI_01_StatusIndicator_Init_powers_ON_LED_and_clears_position_leds(void)
{
    GPIO_InitTypeDef expected_init;
    expected_init.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    expected_init.Mode  = GPIO_MODE_OUTPUT_PP;
    expected_init.Pull  = GPIO_NOPULL;
    expected_init.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init_Expect(GPIOC, &expected_init);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Init();
}

/* --- SI_02: Position 0 shows no green LED ---
   valid=1, pos=0
   Expects only RESET for GPIO_PIN_1..GPIO_PIN_5 called, no SET.
 */
void test_SI_02_StatusIndicator_Update_position_0_all_position_leds_OFF(void)
{
    /* All position LEDs RESET in order, no SETs. */
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Update(1, 0);
}

/* --- SI_03: Display position 5 ---
   valid=1, pos=5; Only LED for position 5 ON.
   RESET all 1..5 first, then SET for 5 (== PIN_5)
 */
void test_SI_03_StatusIndicator_Update_display_position_5_leds(void)
{
    /* Off then on: RESET all, then SET last */
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
    /* Only SET for logical_position==5 (means s_led_pos_pins[4] = GPIO_PIN_5) */
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);

    StatusIndicator_Update(1, 5);
}

/* --- SI_04: Invalid position -> all OFF ---
   valid=0, pos=0xFF; should reset all position LEDs, no SETs.
 */
void test_SI_04_StatusIndicator_Update_invalid_position_turns_all_leds_OFF(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Update(0, 0xFF);
}

/* --- SI_05: Power LED software control ---
   SetPowerLED(0) then SetPowerLED(1)
   Expects OFF then ON (RESET then SET) for GPIO_PIN_0
 */
void test_SI_05_StatusIndicator_SetPowerLED_OFF_then_ON(void)
{
    /* Off, then on */
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    StatusIndicator_SetPowerLED(0);

    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    StatusIndicator_SetPowerLED(1);
}

/* --- SI_06: Boundary pos=6 -> all LEDs OFF ---
   valid=1, pos=6; Out-of-bounds, so all position LEDs OFF (RESET all 1..5), no SET.
 */
void test_SI_06_StatusIndicator_Update_boundary_pos_6_all_position_leds_OFF(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Update(1, 6);
}
