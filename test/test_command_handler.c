/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* CH_01: Accept valid command '0' */
void test_command_handler_CH_01(void)
{
    UART_HandleTypeDef huart2;
    uint8_t cmd = 99;
    uint8_t rx = '0';
    /* Expect HAL_UART_Receive to succeed and store '0' in rx */
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&rx, 1);
    uint8_t ret = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(0, cmd);
}

/* CH_02: Accept valid command '5' (upper bound) */
void test_command_handler_CH_02(void)
{
    UART_HandleTypeDef huart2;
    uint8_t cmd = 99;
    uint8_t rx = '5';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&rx, 1);
    uint8_t ret = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(5, cmd);
}

/* CH_03: Reject numeric out-of-range command */
void test_command_handler_CH_03(void)
{
    UART_HandleTypeDef huart2;
    uint8_t cmd = 7;
    uint8_t expected = cmd;
    uint8_t rx = '8';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&rx, 1);
    uint8_t ret = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(expected, cmd);
}

/* CH_04: Reject non-numeric command */
void test_command_handler_CH_04(void)
{
    UART_HandleTypeDef huart2;
    uint8_t cmd = 3;
    uint8_t expected = cmd;
    uint8_t rx = 'x';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&rx, 1);
    uint8_t ret = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(expected, cmd);
}

/* CH_05: Reject UART receive failure */
void test_command_handler_CH_05(void)
{
    UART_HandleTypeDef huart2;
    uint8_t cmd = 4;
    uint8_t expected = cmd;
    uint8_t rx = '1'; // Value doesn't matter, receive will fail
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_ERROR);
    uint8_t ret = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(expected, cmd);
}

/* CH-06: Handle NULL pointer safely */
void test_command_handler_CH_06(void)
{
    uint8_t ret = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL_UINT8(0, ret);
}

/* CH_07: Do not modify output on invalid data */
void test_command_handler_CH_07(void)
{
    UART_HandleTypeDef huart2;
    uint8_t cmd = 42;
    uint8_t expected = cmd;
    uint8_t rx = '9';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&rx, 1);
    uint8_t ret = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(expected, cmd);
}
