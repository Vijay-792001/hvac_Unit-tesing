#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/* TC_SI_001: Set OK status should set LED pin high */
void test_StatusIndicator_Set_OK_Sets_LED_Pin_High(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
    StatusIndicator_Set(STATUS_OK);
}

/* TC_SI_002: Set ERROR status should set LED pin low */
void test_StatusIndicator_Set_ERROR_Sets_LED_Pin_Low(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    StatusIndicator_Set(STATUS_ERROR);
}

/* TC_SI_003: Get returns the last set status */
void test_StatusIndicator_Get_Returns_Last_Status(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
    StatusIndicator_Set(STATUS_OK);
    Status_t st = StatusIndicator_Get();
    TEST_ASSERT_EQUAL(STATUS_OK, st);

    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    StatusIndicator_Set(STATUS_ERROR);
    st = StatusIndicator_Get();
    TEST_ASSERT_EQUAL(STATUS_ERROR, st);
}

/* TC_SI_004: Set with invalid status input handles gracefully */
void test_StatusIndicator_Set_Invalid_Status_Does_Nothing(void)
{
    StatusIndicator_Set(12345);
}

/* TC_SI_005: StatusIndicator_Get returns default on startup */
void test_StatusIndicator_Get_Returns_Default_On_Startup(void)
{
    Status_t st = StatusIndicator_Get();
    TEST_ASSERT_EQUAL(STATUS_OK, st);
}
