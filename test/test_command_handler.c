#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

#include <string.h>
#include <stdint.h>

UART_HandleTypeDef huart2;

#define INIT_UNCHANGED_UINT8 ((uint8_t)0xA5)

void setUp(void)
{
    memset(&huart2, 0, sizeof(huart2));
}

void tearDown(void)
{
}

void test_CH_01_PollCommand_accepts_valid_command_0(void)
{
    uint8_t ret;
    uint8_t cmd = INIT_UNCHANGED_UINT8;
    uint8_t rx = '0';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    ret = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(1U, ret, "Return value should be 1");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0U, cmd, "cmd_out should be 0");
}

void test_CH_02_PollCommand_accepts_valid_command_5(void)
{
    uint8_t ret;
    uint8_t cmd = INIT_UNCHANGED_UINT8;
    uint8_t rx = '5';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    ret = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(1U, ret, "Return value should be 1");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(5U, cmd, "cmd_out should be 5");
}

void test_CH_03_PollCommand_rejects_out_of_range_numeric_command(void)
{
    uint8_t ret;
    uint8_t cmd = INIT_UNCHANGED_UINT8;
    uint8_t rx = '8';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    ret = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0U, ret, "Return value should be 0 for out-of-range numeric");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(INIT_UNCHANGED_UINT8, cmd, "*cmd_out should remain unchanged");
}

void test_CH_04_PollCommand_rejects_non_numeric_command(void)
{
    uint8_t ret;
    uint8_t cmd = INIT_UNCHANGED_UINT8;
    uint8_t rx = 'x';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    ret = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0U, ret, "Return value should be 0 for non-numeric");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(INIT_UNCHANGED_UINT8, cmd, "*cmd_out should remain unchanged");
}

void test_CH_05_PollCommand_rejects_UART_receive_failure(void)
{
    uint8_t ret;
    uint8_t cmd = INIT_UNCHANGED_UINT8;
    uint8_t rx = INIT_UNCHANGED_UINT8;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_ERROR);
    ret = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0U, ret, "Return value should be 0 when UART receive fails");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(INIT_UNCHANGED_UINT8, cmd, "*cmd_out should remain unchanged after UART fail");
}

void test_CH_06_PollCommand_handles_null_pointer_safely(void)
{
    uint8_t ret = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0U, ret, "Return value should be 0 if cmd_out == NULL");
}

void test_CH_07_PollCommand_does_not_modify_output_on_invalid_data(void)
{
    uint8_t ret;
    uint8_t cmd = INIT_UNCHANGED_UINT8;
    uint8_t rx = '9';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    ret = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0U, ret, "Return value should be 0 for rx='9'");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(INIT_UNCHANGED_UINT8, cmd, "*cmd_out should be unchanged for invalid data");
}
