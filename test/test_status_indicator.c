#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

static Status_t stat;

void setUp(void)
{
    memset(&stat, 0, sizeof(stat));
}

void tearDown(void)
{
}

void test_StatusIndicator_SetStatus_ON(void)
{
    stat.val = STATUS_ON;

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);

    int result = StatusIndicator_SetStatus(&stat, STATUS_ON);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(STATUS_ON, stat.val);
}

void test_StatusIndicator_SetStatus_OFF(void)
{
    stat.val = STATUS_OFF;

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);

    int result = StatusIndicator_SetStatus(&stat, STATUS_OFF);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(STATUS_OFF, stat.val);
}

void test_StatusIndicator_SetStatus_Invalid(void)
{
    int result = StatusIndicator_SetStatus(&stat, 99);

    TEST_ASSERT_EQUAL(-1, result);
}

void test_StatusIndicator_SetStatus_NullPtr(void)
{
    int result = StatusIndicator_SetStatus(NULL, STATUS_ON);

    TEST_ASSERT_EQUAL(-2, result);
}
