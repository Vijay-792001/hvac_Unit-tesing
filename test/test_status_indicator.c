/* test_status_indicator.c */
#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
    mock_stm32f4xx_hal_Init();
}

void tearDown(void)
{
    mock_stm32f4xx_hal_Verify();
    mock_stm32f4xx_hal_Destroy();
}

void test_StatusIndicator_Init_InitializesOutputsAndSetsPowerLED_SI01(void)
{
    GPIO_InitTypeDef expected_gpio;
    expected_gpio.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    expected_gpio.Mode = GPIO_MODE_OUTPUT_PP;
    expected_gpio.Pull = GPIO_NOPULL;
    expected_gpio.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init_Expect(GPIOC, &expected_gpio);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);

    for(uint8_t i=1; i<=5; ++i)
    {
        HAL_GPIO_WritePin_Expect(GPIOC, 1U << i, GPIO_PIN_RESET);
    }

    StatusIndicator_Init();
}

void test_StatusIndicator_Update_PositionNotValid_AllOff_SI02(void)
{
    for(uint8_t i=0; i<STATUS_INDICATOR_NUM; ++i)
    {
        HAL_GPIO_WritePin_Expect(GPIOC, 1U << (i+1), GPIO_PIN_RESET);
    }

    StatusIndicator_Update(0, 3);
}

void test_StatusIndicator_Update_ValidPosition3_LightsLED2_SI03(void)
{
    for(uint8_t i=0; i<STATUS_INDICATOR_NUM; ++i)
    {
        HAL_GPIO_WritePin_Expect(GPIOC, 1U << (i+1), GPIO_PIN_RESET);
    }
    HAL_GPIO_WritePin_Expect(GPIOC, 1U << 3, GPIO_PIN_SET);

    StatusIndicator_Update(1, 3);
}

void test_StatusIndicator_Update_ValidPosition0_AllOff_SI04(void)
{
    for(uint8_t i=0; i<STATUS_INDICATOR_NUM; ++i)
    {
        HAL_GPIO_WritePin_Expect(GPIOC, 1U << (i+1), GPIO_PIN_RESET);
    }

    StatusIndicator_Update(1, 0);
}

void test_StatusIndicator_Update_LogicalPositionTooHigh_AllOff_SI05(void)
{
    for(uint8_t i=0; i<STATUS_INDICATOR_NUM; ++i)
    {
        HAL_GPIO_WritePin_Expect(GPIOC, 1U << (i+1), GPIO_PIN_RESET);
    }

    StatusIndicator_Update(1, STATUS_INDICATOR_NUM + 1);
}

void test_StatusIndicator_SetPowerLED_On_SI06(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    StatusIndicator_SetPowerLED(1);
}

void test_StatusIndicator_SetPowerLED_Off_SI07(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    StatusIndicator_SetPowerLED(0);
}
