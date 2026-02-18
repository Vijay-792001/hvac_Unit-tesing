#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_StatusIndicator_Set_Success(void) {
    HAL_GPIO_WritePin_ExpectAndReturn(GPIO_STATUS_PORT, GPIO_PIN_STATUS_RUNNING, GPIO_PIN_SET, HAL_OK);
    int result = status_indicator_set(STATUS_RUNNING);
    TEST_ASSERT_EQUAL(0, result);
}

void test_StatusIndicator_Set_Stopped(void) {
    HAL_GPIO_WritePin_ExpectAndReturn(GPIO_STATUS_PORT, GPIO_PIN_STATUS_RUNNING, GPIO_PIN_RESET, HAL_OK);
    int result = status_indicator_set(STATUS_STOPPED);
    TEST_ASSERT_EQUAL(0, result);
}

void test_StatusIndicator_Set_InvalidStatus(void) {
    int result = status_indicator_set((status_t)99);
    TEST_ASSERT_EQUAL(-1, result);
}

void test_StatusIndicator_Set_HALFailure(void) {
    HAL_GPIO_WritePin_ExpectAndReturn(GPIO_STATUS_PORT, GPIO_PIN_STATUS_RUNNING, GPIO_PIN_SET, HAL_ERROR);
    int result = status_indicator_set(STATUS_RUNNING);
    TEST_ASSERT_EQUAL(-2, result);
}
