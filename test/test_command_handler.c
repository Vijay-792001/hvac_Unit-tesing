#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void test_CommandHandler_ExecutesValidCommand_ShouldReturnSuccess(void)
{
    Command_t command = CMD_START;
    HAL_StatusTypeDef expected_hal_status = HAL_OK;
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_Delay_Expect(10);
    int result = CommandHandler_Execute(&command);
    TEST_ASSERT_EQUAL_INT(COMMAND_HANDLER_SUCCESS, result);
}

void test_CommandHandler_ExecutesUnknownCommand_ShouldReturnError(void)
{
    Command_t command = (Command_t)999; // Invalid/unknown command
    int result = CommandHandler_Execute(&command);
    TEST_ASSERT_EQUAL_INT(COMMAND_HANDLER_ERROR_UNKNOWN_CMD, result);
}

void test_CommandHandler_Execute_NullPointer_ShouldReturnError(void)
{
    int result = CommandHandler_Execute(NULL);
    TEST_ASSERT_EQUAL_INT(COMMAND_HANDLER_ERROR_NULL, result);
}

void test_CommandHandler_StopCommand_ShouldDeactivateGPIO(void)
{
    Command_t command = CMD_STOP;
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
    int result = CommandHandler_Execute(&command);
    TEST_ASSERT_EQUAL_INT(COMMAND_HANDLER_SUCCESS, result);
}

void test_CommandHandler_Execute_BoundaryCommand_ShouldHandleProperly(void)
{
    Command_t command = CMD_MAX; // Assuming CMD_MAX as boundary
    int result = CommandHandler_Execute(&command);
    TEST_ASSERT_EQUAL_INT(COMMAND_HANDLER_ERROR_UNKNOWN_CMD, result);
}
