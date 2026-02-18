/* ===== test_status_indicator.c ===== */

#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

// Provide fake GPIOC instance symbol as required by production code.
GPIO_TypeDef GPIOC_inst;
GPIO_TypeDef *GPIOC = &GPIOC_inst;

void setUp(void)
{
    mock_stm32f4xx_hal_Init();
}

void tearDown(void)
{
    mock_stm32f4xx_hal_Verify();
    mock_stm32f4xx_hal_Destroy();
}

/*
 * SI_01: Power LED turns ON after init
 * Power LED ON, all position LEDs OFF
 * GPIO_PIN_0 SET once AND GPIO_PIN_1..GPIO_PIN_5 RESET
 */
void test_SI_01_StatusIndicator_Init_turns_power_led_on_and_position_leds_off(void)
{
    // Expect HAL_GPIO_Init: Will be called with GPIOC and a GPIO_InitTypeDef*
    HAL_GPIO_Init_ExpectAnyArgs();

    // Power LED ON (SET)
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);

    // All position LEDs OFF (RESET)
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Init();
}

/*
 * SI_02: Position 0 shows no green LED (all position LEDs OFF only)
 * Update(valid=1, pos=0)
 */
void test_SI_02_Update_with_position_0_turns_off_all_position_leds(void)
{
    // All five position LEDs OFF (RESET)
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Update(1, 0);
}

/*
 * SI_03: Display position 5, only that LED ON
 * Update(valid=1, pos=5): RESET all (1..5), SET GPIO_PIN_5
 */
void test_SI_03_Update_with_position_5_sets_only_LED_5(void)
{
    // All LEDs 1-5 RESET in order
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    // Then SET GPIO_PIN_5 because pos=5 (logical -> index 4)
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);

    StatusIndicator_Update(1, 5);
}

/*
 * SI_04: Invalid position → all OFF
 * Update(valid=0, pos=0xFF): all position LEDs OFF
 */
void test_SI_04_Update_with_invalid_position_turns_off_all_position_leds(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Update(0, 0xFF);
}

/*
 * SI_05: Power LED software control
 * SetPowerLED(0), then SetPowerLED(1): Power LED OFF, then ON
 */
void test_SI_05_SetPowerLED_0_off_then_1_on(void)
{
    // Power LED OFF, then ON
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    StatusIndicator_SetPowerLED(0);

    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    StatusIndicator_SetPowerLED(1);
}

/*
 * SI_06: Boundary out-of-range (pos=6) → all OFF
 * Update(valid=1, pos=6): all position LEDs OFF (no SET)
 */
void test_SI_06_Update_with_position_6_turns_off_all_position_leds(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Update(1, 6);
}

/* ===== end of test_status_indicator.c ===== */
