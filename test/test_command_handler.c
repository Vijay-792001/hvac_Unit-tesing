/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

UART_HandleTypeDef huart2;

void setUp(void) {}
void tearDown(void) {}

/* CH_01: Accept valid command '0' */
void test_command_handler_CH_01(void)
{
    uint8_t cmd = CMD_INVALID;
    uint8_t rx = '0';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1U, ret);
    TEST_ASSERT_EQUAL_UINT8(0U, cmd);
}

/* CH_02: Accept valid command '5' (upper bound) */
void test_command_handler_CH_02(void)
{
    uint8_t cmd = CMD_INVALID;
    uint8_t rx = '5';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1U, ret);
    TEST_ASSERT_EQUAL_UINT8(5U, cmd);
}

/* CH_03: Reject numeric out-of-range command */
void test_command_handler_CH_03(void)
{
    uint8_t cmd = CMD_INVALID;
    uint8_t rx = '8';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0U, ret);
    TEST_ASSERT_EQUAL_UINT8(CMD_INVALID, cmd);
}

/* CH_04: Reject non-numeric command */
void test_command_handler_CH_04(void)
{
    uint8_t cmd = CMD_INVALID;
    uint8_t rx = 'x';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0U, ret);
    TEST_ASSERT_EQUAL_UINT8(CMD_INVALID, cmd);
}

/* CH_05: Reject UART receive failure */
void test_command_handler_CH_05(void)
{
    uint8_t cmd = CMD_INVALID;
    uint8_t rx = '0';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_ERROR);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0U, ret);
    TEST_ASSERT_EQUAL_UINT8(CMD_INVALID, cmd);
}

/* CH_06: Handle NULL pointer safely */
void test_command_handler_CH_06(void)
{
    uint8_t rx = '0';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(NULL);

    TEST_ASSERT_EQUAL_UINT8(0U, ret);
}

/* CH_07: Do not modify output on invalid data */
void test_command_handler_CH_07(void)
{
    uint8_t cmd = CMD_INVALID;
    uint8_t rx = '9';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0U, ret);
    TEST_ASSERT_EQUAL_UINT8(CMD_INVALID, cmd);
}