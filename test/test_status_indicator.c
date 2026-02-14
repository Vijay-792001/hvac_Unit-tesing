#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_StatusIndicator_Set_OK(void)
{
    HAL_GPIO_WritePin_Expect(STATUS_PORT, STATUS_PIN_OK, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(STATUS_PORT, STATUS_PIN_ERR, GPIO_PIN_RESET);

    StatusIndicator_Set(STATUS_OK);
}

void test_StatusIndicator_Set_ERROR(void)
{
    HAL_GPIO_WritePin_Expect(STATUS_PORT, STATUS_PIN_OK, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(STATUS_PORT, STATUS_PIN_ERR, GPIO_PIN_SET);

    StatusIndicator_Set(STATUS_ERROR);
}

void test_StatusIndicator_Set_BUSY(void)
{
    HAL_GPIO_WritePin_Expect(STATUS_PORT, STATUS_PIN_OK, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(STATUS_PORT, STATUS_PIN_ERR, GPIO_PIN_RESET);

    StatusIndicator_Set(STATUS_BUSY);
}

void test_StatusIndicator_Set_Invalid_Status(void)
{
    StatusIndicator_Set(0x99);
}
