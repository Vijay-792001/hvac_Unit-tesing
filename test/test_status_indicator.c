/* ===== test_status_indicator.c ===== */

#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

// GPIOC extern in HAL header
GPIO_TypeDef GPIOC_inst;

void setUp(void) {}
void tearDown(void) {}

/* SI_01: SWE-REQ-025 - Power LED turns ON after init */
void test_SI_01_Power_LED_turns_ON_after_init(void)
{
    GPIO_InitTypeDef expected_struct = {
        .Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5,
        .Mode  = GPIO_MODE_OUTPUT_PP,
        .Pull  = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW
    };

    HAL_GPIO_Init_Expect(&GPIOC_inst, &expected_struct);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Init();
}

/* SI_02: SWE-REQ-026 - Position 0 shows no green LED (all position LEDs OFF, no SETs) */
void test_SI_02_Position_0_shows_no_green_LED(void)
{
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Update(1, 0);
}

/* SI_03: SWE-REQ-027 - Display position 5 */
void test_SI_03_Display_position_5_shows_only_LED_5(void)
{
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_5, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_5, GPIO_PIN_SET);

    StatusIndicator_Update(1, 5);
}

/* SI_04: SWE-REQ-029 - Invalid position → all OFF */
void test_SI_04_Invalid_position_all_LEDs_OFF(void)
{
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Update(0, 0xFF);
}

/* SI_05: SWE-REQ-044 - Power LED software control */
void test_SI_05_Power_LED_software_control(void)
{
    // SetPowerLED(0): OFF
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_0, GPIO_PIN_RESET);
    StatusIndicator_SetPowerLED(0);

    // SetPowerLED(1): ON
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_0, GPIO_PIN_SET);
    StatusIndicator_SetPowerLED(1);
}

/* SI_06: SWE-REQ-024 - Boundary out-of-range (pos=6) → all OFF, no SET */
void test_SI_06_Boundary_out_of_range_pos6_all_LEDs_OFF(void)
{
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(&GPIOC_inst, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Update(1, 6);
}
