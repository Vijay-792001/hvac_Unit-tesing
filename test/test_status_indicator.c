/* ===== test_status_indicator.c ===== */
#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

GPIO_TypeDef GPIOC_inst;

void setUp(void)
{
    mock_stm32f4xx_hal_Init();
}
void tearDown(void)
{
    mock_stm32f4xx_hal_Verify();
}

/* === SI_01: Power LED turns ON after init === */
void test_StatusIndicator_Init_power_led_on_pos_leds_off_SI_01(void)
{
    GPIO_InitTypeDef init_args = {0};
    init_args.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    init_args.Mode = GPIO_MODE_OUTPUT_PP;
    init_args.Pull = GPIO_NOPULL;
    init_args.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init_Expect(GPIOC, &init_args);

    // Power LED ON
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    // All position LEDs OFF
    for (uint8_t i = 1; i <= 5; i++)
        HAL_GPIO_WritePin_Expect(GPIOC, 1U << i, GPIO_PIN_RESET);

    StatusIndicator_Init();
}

/* === SI_02: Position 0 shows no green LED === */
void test_StatusIndicator_Update_no_green_led_for_position_0_SI_02(void)
{
    // All position LEDs OFF
    for (uint8_t i = 1; i <= 5; ++i)
        HAL_GPIO_WritePin_Expect(GPIOC, 1U << i, GPIO_PIN_RESET);

    StatusIndicator_Update(1, 0);
}

/* === SI_03: Display position 5 (only pos 5 led ON after all OFF) === */
void test_StatusIndicator_Update_lights_only_position_5_SI_03(void)
{
    // All OFF
    for (uint8_t i = 1; i <= 5; ++i)
        HAL_GPIO_WritePin_Expect(GPIOC, 1U << i, GPIO_PIN_RESET);
    // Only LED 5 ON
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);

    StatusIndicator_Update(1, 5);
}

/* === SI_04: Invalid position (valid=0) → all OFF === */
void test_StatusIndicator_Update_all_leds_off_when_invalid_SI_04(void)
{
    // All OFF
    for (uint8_t i = 1; i <= 5; ++i)
        HAL_GPIO_WritePin_Expect(GPIOC, 1U << i, GPIO_PIN_RESET);

    StatusIndicator_Update(0, 0xFF);
}

/* === SI_05: Power LED software control === */
void test_StatusIndicator_SetPowerLED_turns_off_then_on_SI_05(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    StatusIndicator_SetPowerLED(0);

    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    StatusIndicator_SetPowerLED(1);
}

/* === SI_06: Out-of-range logical_position (pos=6) → all LEDs OFF === */
void test_StatusIndicator_Update_out_of_range_pos_all_leds_off_SI_06(void)
{
    // All OFF
    for (uint8_t i = 1; i <= 5; ++i)
        HAL_GPIO_WritePin_Expect(GPIOC, 1U << i, GPIO_PIN_RESET);

    StatusIndicator_Update(1, 6);
}
