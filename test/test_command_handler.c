#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_HandleCommand_ValidCommand_ExecutesAction(void)
{
    Command_t cmd = CMD_START;
    HAL_StatusTypeDef expectedStatus = HAL_OK;
    HAL_GPIO_WritePin_ExpectAndReturn(GPIOA, GPIO_PIN_0, GPIO_PIN_SET, expectedStatus);
    TEST_ASSERT_EQUAL(HANDLER_OK, HandleCommand(&cmd));
}

void test_HandleCommand_InvalidCommand_ReturnsError(void)
{
    Command_t cmd = (Command_t)999;
    TEST_ASSERT_EQUAL(HANDLER_INVALID_COMMAND, HandleCommand(&cmd));
}

void test_HandleCommand_NullPointer_ReturnsError(void)
{
    TEST_ASSERT_EQUAL(HANDLER_NULL_PTR, HandleCommand(NULL));
}

void test_HandleCommand_BoundaryStartCommand_Processes(void)
{
    Command_t cmd = CMD_START;
    HAL_GPIO_WritePin_ExpectAndReturn(GPIOA, GPIO_PIN_0, GPIO_PIN_SET, HAL_OK);
    TEST_ASSERT_EQUAL(HANDLER_OK, HandleCommand(&cmd));
}

void test_HandleCommand_BoundaryStopCommand_Processes(void)
{
    Command_t cmd = CMD_STOP;
    HAL_GPIO_WritePin_ExpectAndReturn(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET, HAL_OK);
    TEST_ASSERT_EQUAL(HANDLER_OK, HandleCommand(&cmd));
}

void test_HandleCommand_HALWriteFails_ReturnsError(void)
{
    Command_t cmd = CMD_START;
    HAL_GPIO_WritePin_ExpectAndReturn(GPIOA, GPIO_PIN_0, GPIO_PIN_SET, HAL_ERROR);
    TEST_ASSERT_EQUAL(HANDLER_HW_FAIL, HandleCommand(&cmd));
}
