/* ===== test_status_indicator.c ===== */
#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

// SI_01: Power LED turns ON after init
void test_SI_01_Power_LED_turns_ON_after_init_all_position_LEDs_OFF(void)
{
    GPIO_InitTypeDef dummy;
    HAL_GPIO_Init_Expect(GPIOC, &dummy);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    for(uint8_t i=1;i<=5;i++) {
        HAL_GPIO_WritePin_Expect(GPIOC, (1U << i), GPIO_PIN_RESET);
    }
    StatusIndicator_Init();
}

// SI_02: Position 0 shows all position LEDs OFF
void test_SI_02_Position_0_shows_no_position_LED_on(void)
{
    for(uint8_t i=0;i<5;i++)
        HAL_GPIO_WritePin_Expect(GPIOC, (1U << (i+1)), GPIO_PIN_RESET);
    StatusIndicator_Update(1, 0);
}

// SI_03: Display position 5 (LED5 ON only)
void test_SI_03_Display_position_5_sets_only_LED5_ON(void)
{
    for(uint8_t i=0;i<5;i++)
        HAL_GPIO_WritePin_Expect(GPIOC, (1U << (i+1)), GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
    StatusIndicator_Update(1, 5);
}

// SI_04: Invalid position → all OFF
void test_SI_04_Invalid_position_turns_all_position_LEDs_OFF(void)
{
    for(uint8_t i=0;i<5;i++)
        HAL_GPIO_WritePin_Expect(GPIOC, (1U << (i+1)), GPIO_PIN_RESET);
    StatusIndicator_Update(0, 0xFF);
}

// SI_05: Power LED software control test
void test_SI_05_Power_LED_control_OFF_then_ON(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    StatusIndicator_SetPowerLED(0);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    StatusIndicator_SetPowerLED(1);
}

// SI_06: Boundary out-of-range logical_position=6 → all OFF
void test_SI_06_Boundary_out_of_range_pos_6_all_LEDs_OFF(void)
{
    for(uint8_t i=0;i<5;i++)
        HAL_GPIO_WritePin_Expect(GPIOC, (1U << (i+1)), GPIO_PIN_RESET);
    StatusIndicator_Update(1, 6);
}
