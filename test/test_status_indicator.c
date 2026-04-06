/* ===== test_status_indicator.c ===== */
#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

GPIO_TypeDef GPIOC_inst;

void setUp(void) {}
void tearDown(void) {}

/* SI_01: Power LED turns ON after init */
void test_SI_01_Power_LED_turns_ON_after_init(void)
{
    HAL_GPIO_Init_Expect(GPIOC, (GPIO_InitTypeDef*)ANY);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    for(uint8_t i=1; i<=5; ++i)
        HAL_GPIO_WritePin_Expect(GPIOC, (1U << i), GPIO_PIN_RESET);

    StatusIndicator_Init();
}

/* SI_02: Position 0 shows no green LED */
void test_SI_02_Position_0_shows_no_green_led(void)
{
    for(uint8_t i=1; i<=5; ++i)
        HAL_GPIO_WritePin_Expect(GPIOC, (1U << i), GPIO_PIN_RESET);

    StatusIndicator_Update(1, 0);
}

/* SI_03: Display position 5 */
void test_SI_03_Display_position_5_led_only(void)
{
    for(uint8_t i=1; i<=5; ++i)
        HAL_GPIO_WritePin_Expect(GPIOC, (1U << i), GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);

    StatusIndicator_Update(1, 5);
}

/* SI_04: Invalid position → all OFF */
void test_SI_04_Invalid_position_turns_all_off(void)
{
    for(uint8_t i=1; i<=5; ++i)
        HAL_GPIO_WritePin_Expect(GPIOC, (1U << i), GPIO_PIN_RESET);

    StatusIndicator_Update(0, 0xFF);
}

/* SI_05: Power LED software control ON/OFF */
void test_SI_05_Power_LED_software_control(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    StatusIndicator_SetPowerLED(0);

    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    StatusIndicator_SetPowerLED(1);
}

/* SI_06: Boundary out-of-range (pos=6) → all OFF */
void test_SI_06_Boundary_out_of_range_all_off(void)
{
    for(uint8_t i=1; i<=5; ++i)
        HAL_GPIO_WritePin_Expect(GPIOC, (1U << i), GPIO_PIN_RESET);

    StatusIndicator_Update(1, 6);
}
