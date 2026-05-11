/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_command_handler_CH_01(void)
{
    uint8_t cmd_out = CMD_INVALID;
    uint8_t rx_data = '0';
    HAL_UART_Receive_ExpectAndReturn(NULL, &rx_data, CMD_UART_BUF_LEN, 0, HAL_OK);
    int ret = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL_INT(1, ret);
    TEST_ASSERT_EQUAL_UINT8(0, cmd_out);
}

void test_command_handler_CH_02(void)
{
    uint8_t cmd_out = CMD_INVALID;
    uint8_t rx_data = '5';
    HAL_UART_Receive_ExpectAndReturn(NULL, &rx_data, CMD_UART_BUF_LEN, 0, HAL_OK);
    int ret = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL_INT(1, ret);
    TEST_ASSERT_EQUAL_UINT8(5, cmd_out);
}

void test_command_handler_CH_03(void)
{
    uint8_t cmd_out = CMD_INVALID;
    uint8_t rx_data = '8';
    HAL_UART_Receive_ExpectAndReturn(NULL, &rx_data, CMD_UART_BUF_LEN, 0, HAL_OK);
    int ret = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_UINT8(CMD_INVALID, cmd_out);
}

void test_command_handler_CH_04(void)
{
    uint8_t cmd_out = CMD_INVALID;
    uint8_t rx_data = 'x';
    HAL_UART_Receive_ExpectAndReturn(NULL, &rx_data, CMD_UART_BUF_LEN, 0, HAL_OK);
    int ret = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_UINT8(CMD_INVALID, cmd_out);
}

void test_command_handler_CH_05(void)
{
    uint8_t cmd_out = CMD_INVALID;
    HAL_UART_Receive_ExpectAndReturn(NULL, NULL, CMD_UART_BUF_LEN, 0, HAL_ERROR);
    int ret = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_UINT8(CMD_INVALID, cmd_out);
}

void test_command_handler_CH_06(void)
{
    HAL_UART_Receive_ExpectAndReturn(NULL, NULL, CMD_UART_BUF_LEN, 0, HAL_OK);
    int ret = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_command_handler_CH_07(void)
{
    uint8_t cmd_out = CMD_INVALID;
    uint8_t rx_data = '9';
    HAL_UART_Receive_ExpectAndReturn(NULL, &rx_data, CMD_UART_BUF_LEN, 0, HAL_OK);
    int ret = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL_UINT8(CMD_INVALID, cmd_out);
    TEST_ASSERT_EQUAL_INT(0, ret);
}