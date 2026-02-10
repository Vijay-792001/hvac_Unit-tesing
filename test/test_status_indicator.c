#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_Status_Indicator_Init_should_Configure_GPIO(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
    Status_Indicator_Init();
}

void test_Status_Indicator_On_should_Set_GPIO_High(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
    Status_Indicator_On();
}

void test_Status_Indicator_Off_should_Set_GPIO_Low(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
    Status_Indicator_Off();
}

void test_Status_Indicator_Toggle_should_Change_GPIO_State(void)
{
    HAL_GPIO_TogglePin_Expect(GPIOC, GPIO_PIN_5);
    Status_Indicator_Toggle();
}

void test_Status_Indicator_On_should_Handle_Null(void)
{
}
