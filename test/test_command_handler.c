/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_command_handler_CH_01(void)
{
    /* Accept valid command '0' */
    uint8_t cmd = 0xFF;
    extern UART_HandleTypeDef huart2;
    uint8_t rx_val = '0';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_EQUAL_UINT8(0, cmd);
}

void test_command_handler_CH_02(void)
{
    /* Accept valid command '5' (upper bound) */
    uint8_t cmd = 0xFF;
    extern UART_HandleTypeDef huart2;
    uint8_t rx_val = '5';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_EQUAL_UINT8(5, cmd);
}

void test_command_handler_CH_03(void)
{
    /* Reject numeric out-of-range command */
    uint8_t cmd = 0xA5; /* some nonzero, should not change */
    extern UART_HandleTypeDef huart2;
    uint8_t rx_val = '8';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);

    uint8_t old_cmd = cmd;
    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(old_cmd, cmd);
}

void test_command_handler_CH_04(void)
{
    /* Reject non-numeric command */
    uint8_t cmd = 0xBB;
    extern UART_HandleTypeDef huart2;
    uint8_t rx_val = 'x';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);

    uint8_t old_cmd = cmd;
    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(old_cmd, cmd);
}

void test_command_handler_CH_05(void)
{
    /* Reject UART receive failure */
    uint8_t cmd = 0xCD;
    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &cmd, 1, 10, HAL_ERROR);

    uint8_t old_cmd = cmd;
    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(old_cmd, cmd);
}

void test_command_handler_CH_06(void)
{
    /* Handle NULL pointer safely */
    uint8_t result = CommandHandler_PollCommand(NULL);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    /* No crash – test passes if reached here */
}

void test_command_handler_CH_07(void)
{
    /* Do not modify output on invalid data (e.g., rx = '9') */
    uint8_t cmd = 0x55;
    extern UART_HandleTypeDef huart2;
    uint8_t rx_val = '9';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);

    uint8_t old_cmd = cmd;
    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(old_cmd, cmd);
}

