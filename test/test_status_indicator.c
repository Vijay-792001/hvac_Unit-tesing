/* ===== test_status_indicator.c ===== */
#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_status_indicator_SI01(void)
{
    GPIO_InitTypeDef gpio_init;
    gpio_init.Pin = GPIO_PIN_0;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init_Expect(GPIOC, &gpio_init);
    StatusIndicator_Init();
}

void test_status_indicator_SI02(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    StatusIndicator_SetLED(0, 1);
}

void test_status_indicator_SI03(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    StatusIndicator_SetLED(0, 0);
}

void test_status_indicator_SI04(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
    StatusIndicator_SetLED(1, 1);
}

void test_status_indicator_SI05(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    StatusIndicator_SetLED(1, 0);
}

void test_status_indicator_SI06(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
    StatusIndicator_SetLED(2, 1);
}

void test_status_indicator_SI07(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    StatusIndicator_SetLED(2, 0);
}

void test_status_indicator_SI08(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
    StatusIndicator_SetLED(3, 1);
}

void test_status_indicator_SI09(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    StatusIndicator_SetLED(3, 0);
}

void test_status_indicator_SI10(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
    StatusIndicator_SetLED(4, 1);
}

void test_status_indicator_SI11(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    StatusIndicator_SetLED(4, 0);
}

void test_status_indicator_SI12(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
    StatusIndicator_SetLED(5, 1);
}

void test_status_indicator_SI13(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
    StatusIndicator_SetLED(5, 0);
}