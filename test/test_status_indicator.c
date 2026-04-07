/* ===== test_status_indicator.c ===== */
#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"
GPIO_TypeDef GPIOC_inst;
GPIO_TypeDef *GPIOC = &GPIOC_inst;
static const uint16_t s_led_pos_pins[STATUS_INDICATOR_NUM] = {
    GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5
};
void setUp(void) {}
void tearDown(void) {}
void test_SI_01_Power_LED_ON_and_position_LEDs_OFF_after_Init(void)
{
    GPIO_InitTypeDef expected_struct;
    expected_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    expected_struct.Mode = GPIO_MODE_OUTPUT_PP;
    expected_struct.Pull = GPIO_NOPULL;
    expected_struct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init_Expect(GPIOC, &expected_struct);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    for (uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(GPIOC, s_led_pos_pins[i], GPIO_PIN_RESET);
    }
    StatusIndicator_Init();
}
void test_SI_02_Position_0_shows_no_green_LED(void)
{
    for (uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(GPIOC, s_led_pos_pins[i], GPIO_PIN_RESET);
    }
    StatusIndicator_Update(1, 0);
}
void test_SI_03_Display_position_5_LED_only(void)
{
    for (uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(GPIOC, s_led_pos_pins[i], GPIO_PIN_RESET);
    }
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
    StatusIndicator_Update(1, 5);
}
void test_SI_04_Invalid_position_turns_all_LEDs_OFF(void)
{
    for (uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(GPIOC, s_led_pos_pins[i], GPIO_PIN_RESET);
    }
    StatusIndicator_Update(0, 0xFF);
}
void test_SI_05_Power_LED_software_control(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    StatusIndicator_SetPowerLED(0);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    StatusIndicator_SetPowerLED(1);
}
void test_SI_06_Boundary_out_of_range_turns_all_LEDs_OFF(void)
{
    for (uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(GPIOC, s_led_pos_pins[i], GPIO_PIN_RESET);
    }
    StatusIndicator_Update(1, 6);
}
// End of test_status_indicator.c
