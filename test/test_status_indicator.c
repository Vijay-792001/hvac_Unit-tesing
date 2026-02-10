/* ===== test_status_indicator.c ===== */
#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/*
 * TestPlan: Status Indicator
 * SI_TC01: Power LED turns ON after init, all pos LEDs OFF
 * SI_TC02: Position 0 shows no green LED (all OFF)
 * SI_TC03: Display position 5 (only 5 ON, rest OFF)
 * SI_TC04: Invalid position—all LEDs OFF
 * SI_TC05: Power LED software control OFF/ON
 * SI_TC06: Boundary out-of-range (pos=6) - all LEDs OFF
 */

static const uint16_t s_led_pos_pins[STATUS_INDICATOR_NUM] = {
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5
};

void test_SI_TC01_PowerLEDonAfterInit(void)
{
    HAL_GPIO_Init_Ignore();
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    for (uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(GPIOC, s_led_pos_pins[i], GPIO_PIN_RESET);
    }
    StatusIndicator_Init();
}

void test_SI_TC02_Position0_NoGreenLED(void)
{
    for (uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(GPIOC, s_led_pos_pins[i], GPIO_PIN_RESET);
    }
    StatusIndicator_Update(1, 0);
}

void test_SI_TC03_DisplayPosition5(void)
{
    for (uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(GPIOC, s_led_pos_pins[i], GPIO_PIN_RESET);
    }
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
    StatusIndicator_Update(1, 5);
}

void test_SI_TC04_InvalidPosition_AllLEDsOff(void)
{
    for (uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(GPIOC, s_led_pos_pins[i], GPIO_PIN_RESET);
    }
    StatusIndicator_Update(0, 0xFF);
}

void test_SI_TC05_PowerLEDSoftwareControl_Off_On(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    StatusIndicator_SetPowerLED(0);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    StatusIndicator_SetPowerLED(1);
}

void test_SI_TC06_PositionOutOfRange_AllLEDsOff(void)
{
    for (uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(GPIOC, s_led_pos_pins[i], GPIO_PIN_RESET);
    }
    StatusIndicator_Update(1, 6);
}
