#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_CommandHandler_ProcessCommand_ValidCommand_ReturnsSuccess(void)
{
    Command_t valid_cmd = COMMAND_ON;
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);

    int result = CommandHandler_ProcessCommand(&valid_cmd);
    TEST_ASSERT_EQUAL(COMMAND_SUCCESS, result);
}

void test_CommandHandler_ProcessCommand_InvalidCommand_ReturnsInvalid(void)
{
    Command_t invalid_cmd =  (Command_t)99;

    int result = CommandHandler_ProcessCommand(&invalid_cmd);
    TEST_ASSERT_EQUAL(COMMAND_INVALID, result);
}

void test_CommandHandler_ProcessCommand_NullPointer_ReturnsError(void)
{
    int result = CommandHandler_ProcessCommand(NULL);
    TEST_ASSERT_EQUAL(COMMAND_ERROR, result);
}

void test_CommandHandler_ProcessCommand_OffCommand_ResetsGpio(void)
{
    Command_t cmd = COMMAND_OFF;
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

    int result = CommandHandler_ProcessCommand(&cmd);
    TEST_ASSERT_EQUAL(COMMAND_SUCCESS, result);
}

void test_CommandHandler_ProcessCommand_HalWritePinFails_ReturnsError(void)
{
    Command_t cmd = COMMAND_ON;
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);

    int result = CommandHandler_ProcessCommand(&cmd);
    TEST_ASSERT_EQUAL(COMMAND_SUCCESS, result);
}
