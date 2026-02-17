#include "unity.h"
#include "command_handler.h"
#include "mock_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_CommandHandler_DispatchesValidCommand_ShouldInvokeExpectedActions(void)
{
    Command_t command = CMD_TURN_ON;
    mock_HAL_SendCommand_ExpectAndReturn(command, HAL_OK);
    int result = CommandHandler_Dispatch(command);
    TEST_ASSERT_EQUAL(0, result);
}

void test_CommandHandler_DispatchesInvalidCommand_ShouldReturnError(void)
{
    Command_t command = 99;
    int result = CommandHandler_Dispatch(command);
    TEST_ASSERT_EQUAL(-1, result);
}

void test_CommandHandler_Dispatch_HAL_Failure_ShouldReturnError(void)
{
    Command_t command = CMD_TURN_OFF;
    mock_HAL_SendCommand_ExpectAndReturn(command, HAL_ERROR);
    int result = CommandHandler_Dispatch(command);
    TEST_ASSERT_EQUAL(-2, result);
}

void test_CommandHandler_Dispatch_NullCommandPointer_ShouldHandleGracefully(void)
{
    Command_t* cmd_ptr = NULL;
    int result = CommandHandler_DispatchPtr(cmd_ptr);
    TEST_ASSERT_EQUAL(-3, result);
}

void test_CommandHandler_Dispatch_BoundaryCommand_ShouldSucceed(void)
{
    Command_t command = CMD_MAX_DEFINED;
    mock_HAL_SendCommand_ExpectAndReturn(command, HAL_OK);
    int result = CommandHandler_Dispatch(command);
    TEST_ASSERT_EQUAL(0, result);
}
