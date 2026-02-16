#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/*
Test Plan ID: TC_SI_01
Description: Test StatusIndicator_Init initializes HAL GPIO.
*/
void test_StatusIndicator_Init_should_initialize_GPIO_TC_SI_01(void)
{
    HAL_GPIO_Init_ExpectAndReturn(HAL_OK);

    TEST_ASSERT_EQUAL(STATUS_INDICATOR_OK, StatusIndicator_Init());
}

/*
Test Plan ID: TC_SI_02
Description: Test StatusIndicator_Init handles HAL failure.
*/
void test_StatusIndicator_Init_should_return_error_on_HAL_failure_TC_SI_02(void)
{
    HAL_GPIO_Init_ExpectAndReturn(HAL_ERROR);

    TEST_ASSERT_EQUAL(STATUS_INDICATOR_ERROR, StatusIndicator_Init());
}

/*
Test Plan ID: TC_SI_03
Description: Test StatusIndicator_Set calls HAL to turn on indicator.
*/
void test_StatusIndicator_Set_should_set_indicator_on_TC_SI_03(void)
{
    Status_t status = STATUS_ON;
    HAL_GPIO_WritePin_ExpectAndReturn(STATUS_INDICATOR_GPIO_PORT, STATUS_INDICATOR_PIN, GPIO_PIN_SET, HAL_OK);

    TEST_ASSERT_EQUAL(STATUS_INDICATOR_OK, StatusIndicator_Set(status));
}

/*
Test Plan ID: TC_SI_04
Description: Test StatusIndicator_Set calls HAL to turn off indicator.
*/
void test_StatusIndicator_Set_should_set_indicator_off_TC_SI_04(void)
{
    Status_t status = STATUS_OFF;
    HAL_GPIO_WritePin_ExpectAndReturn(STATUS_INDICATOR_GPIO_PORT, STATUS_INDICATOR_PIN, GPIO_PIN_RESET, HAL_OK);

    TEST_ASSERT_EQUAL(STATUS_INDICATOR_OK, StatusIndicator_Set(status));
}

/*
Test Plan ID: TC_SI_05
Description: Test StatusIndicator_Set handles invalid status value.
*/
void test_StatusIndicator_Set_should_return_error_on_unknown_status_TC_SI_05(void)
{
    Status_t status = (Status_t)99; // Invalid enum

    TEST_ASSERT_EQUAL(STATUS_INDICATOR_INVALID_PARAM, StatusIndicator_Set(status));
}

/*
Test Plan ID: TC_SI_06
Description: Test StatusIndicator_Set handles HAL write failure.
*/
void test_StatusIndicator_Set_should_return_error_on_HAL_failure_TC_SI_06(void)
{
    Status_t status = STATUS_ON;
    HAL_GPIO_WritePin_ExpectAndReturn(STATUS_INDICATOR_GPIO_PORT, STATUS_INDICATOR_PIN, GPIO_PIN_SET, HAL_ERROR);

    TEST_ASSERT_EQUAL(STATUS_INDICATOR_ERROR, StatusIndicator_Set(status));
}

/*
Test Plan ID: TC_SI_07
Description: Test indicator functions with boundary pin definitions.
*/
void test_StatusIndicator_Set_should_handle_boundary_GPIO_pins_TC_SI_07(void)
{
    Status_t status = STATUS_ON;

    // Assumes special macro for minimum pin number exists for test
    HAL_GPIO_WritePin_ExpectAndReturn(STATUS_INDICATOR_GPIO_PORT, 0, GPIO_PIN_SET, HAL_OK);
    TEST_ASSERT_EQUAL(STATUS_INDICATOR_OK, StatusIndicator_Set(status));

    // For maximum (simulate if macro available)
    HAL_GPIO_WritePin_ExpectAndReturn(STATUS_INDICATOR_GPIO_PORT, 15, GPIO_PIN_SET, HAL_OK);
    TEST_ASSERT_EQUAL(STATUS_INDICATOR_OK, StatusIndicator_Set(status));
}
