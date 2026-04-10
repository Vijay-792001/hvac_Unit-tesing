/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* Test Case: CH_01 - Accept valid command '0' */
void test_command_handler_CH_01(void)
{
    uint8_t cmd = 77; // non-zero value to check overwrite
    uint8_t rx = '0';

    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(1, result);
    TEST_ASSERT_EQUAL(0, cmd);
}

/* Test Case: CH_02 - Accept valid command '5' (upper bound) */
void test_command_handler_CH_02(void)
{
    uint8_t cmd = 13;
    uint8_t rx = '5';

    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(1, result);
    TEST_ASSERT_EQUAL(5, cmd);
}

/* Test Case: CH_03 - Reject numeric out-of-range command */
void test_command_handler_CH_03(void)
{
    uint8_t cmd = 21;
    uint8_t rx = '8';

    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(21, cmd); // unchanged
}

/* Test Case: CH_04 - Reject non-numeric command */
void test_command_handler_CH_04(void)
{
    uint8_t cmd = 9;
    uint8_t rx = 'x';

    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(9, cmd); // unchanged
}

/* Test Case: CH_05 - Reject UART receive failure */
void test_command_handler_CH_05(void)
{
    uint8_t cmd = 44;

    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &cmd, 1, 10, HAL_ERROR);

    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(44, cmd); // unchanged
}

/* Test Case: CH-06 - Handle NULL pointer safely */
void test_command_handler_CH_06(void)
{
    int result = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL(0, result);
}

/* Test Case: CH_07 - Do not modify output on invalid data */
void test_command_handler_CH_07(void)
{
    uint8_t cmd = 42;
    uint8_t rx = '9';

    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(42, cmd); // unchanged
}
