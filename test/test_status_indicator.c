#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_StatusIndicator_Set_StatusOK_SetsGPIOHigh(void)
{
    HAL_GPIO_WritePin_ExpectAndReturn(GPIOB, GPIO_PIN_5, GPIO_PIN_SET, HAL_OK);
    TEST_ASSERT_EQUAL(SI_OK, StatusIndicator_Set(SI_STATUS_OK));
}

void test_StatusIndicator_Set_StatusFault_SetsGPIOLow(void)
{
    HAL_GPIO_WritePin_ExpectAndReturn(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET, HAL_OK);
    TEST_ASSERT_EQUAL(SI_OK, StatusIndicator_Set(SI_STATUS_FAULT));
}

void test_StatusIndicator_Set_InvalidStatus_ReturnsError(void)
{
    TEST_ASSERT_EQUAL(SI_INVALID_PARAM, StatusIndicator_Set((StatusIndicator_State_t)99));
}

void test_StatusIndicator_Set_HALFails_ReturnsError(void)
{
    HAL_GPIO_WritePin_ExpectAndReturn(GPIOB, GPIO_PIN_5, GPIO_PIN_SET, HAL_ERROR);
    TEST_ASSERT_EQUAL(SI_HW_FAIL, StatusIndicator_Set(SI_STATUS_OK));
}
