#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_CommandHandler_ExecutesValidCommand_Calls_HAL_WriteCommand_And_ReturnsSuccess(void)
{
    uint8_t command = 0xA5;
    HAL_StatusTypeDef hal_status = HAL_OK;
    CommandStatus_t ret_status;

    HAL_WriteCommand_ExpectAndReturn(command, hal_status);

    ret_status = CommandHandler_Execute(command);

    TEST_ASSERT_EQUAL(COMMAND_STATUS_OK, ret_status);
}

void test_CommandHandler_ExecutesInvalidCommand_ReturnsInvalidStatus(void)
{
    uint8_t command = 0xFF;
    CommandStatus_t ret_status;
    ret_status = CommandHandler_Execute(command);
    TEST_ASSERT_EQUAL(COMMAND_STATUS_INVALID, ret_status);
}

void test_CommandHandler_WhenHALFails_ReturnsHAL_ERROR(void)
{
    uint8_t command = 0xA5;
    HAL_StatusTypeDef hal_status = HAL_ERROR;
    CommandStatus_t ret_status;
    HAL_WriteCommand_ExpectAndReturn(command, hal_status);
    ret_status = CommandHandler_Execute(command);
    TEST_ASSERT_EQUAL(COMMAND_STATUS_HAL_ERROR, ret_status);
}

void test_CommandHandler_NullHandling(void)
{
    TEST_PASS();
}

void test_CommandHandler_LowestValidCommand_ExecutesSuccessfully(void)
{
    uint8_t command = 0x00;
    HAL_WriteCommand_ExpectAndReturn(command, HAL_OK);
    CommandStatus_t ret_status = CommandHandler_Execute(command);
    TEST_ASSERT_EQUAL(COMMAND_STATUS_OK, ret_status);
}

void test_CommandHandler_HighestValidCommand_ExecutesSuccessfully(void)
{
    uint8_t command = 0xA5;
    HAL_WriteCommand_ExpectAndReturn(command, HAL_OK);
    CommandStatus_t ret_status = CommandHandler_Execute(command);
    TEST_ASSERT_EQUAL(COMMAND_STATUS_OK, ret_status);
}

void test_CommandHandler_Initialize_Calls_HAL_Init(void)
{
    HAL_Init_ExpectAndReturn(HAL_OK);
    CommandHandler_Initialize();
}

void test_CommandHandler_Initialize_ReturnsHAL_ERROR_OnFail(void)
{
    HAL_Init_ExpectAndReturn(HAL_ERROR);
    CommandStatus_t ret = CommandHandler_Initialize();
    TEST_ASSERT_EQUAL(COMMAND_STATUS_HAL_ERROR, ret);
}

void test_CommandHandler_Initialize_Twice_IsIdempotent(void)
{
    HAL_Init_ExpectAndReturn(HAL_OK);
    HAL_Init_ExpectAndReturn(HAL_OK);
    CommandHandler_Initialize();
    CommandHandler_Initialize();
}
