/* ===== test_status_indicator.c ===== */
#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* Test Plan ID: TC01 */
void test_status_indicator_TC01(void)
{
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    int result = status_indicator_on();
    TEST_ASSERT_EQUAL(0, result);
}

/* Test Plan ID: TC02 */
void test_status_indicator_TC02(void)
{
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    int result = status_indicator_off();
    TEST_ASSERT_EQUAL(0, result);
}

/* Test Plan ID: TC03 */
void test_status_indicator_TC03(void)
{
    HAL_GPIO_TogglePin_Expect(GPIOA, GPIO_PIN_5);
    int result = status_indicator_toggle();
    TEST_ASSERT_EQUAL(0, result);
}

/* Test Plan ID: TC04 */
void test_status_indicator_TC04(void)
{
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    int result = status_indicator_set_state(1);
    TEST_ASSERT_EQUAL(0, result);
}

/* Test Plan ID: TC05 */
void test_status_indicator_TC05(void)
{
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    int result = status_indicator_set_state(0);
    TEST_ASSERT_EQUAL(0, result);
}