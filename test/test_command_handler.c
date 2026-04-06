/* ===== test_command_handler.c ===== */

#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

UART_HandleTypeDef huart2;

void setUp(void)
{
    // Nothing needed yet
}

void tearDown(void)
{
    // Nothing needed yet
}

/* Test Case: CH_01 - Accept valid command '0' */
void test_CH_01_accept_valid_command_0(void)
{
    uint8_t cmd = 0xAA; // Known init value
    uint8_t rx = '0';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(1, result);
    TEST_ASSERT_EQUAL_UINT8(0, cmd);
}

/* Test Case: CH_02 - Accept valid command '5' (upper bound) */
void test_CH_02_accept_valid_command_5(void)
{
    uint8_t cmd = 0xAA; // Known init value
    uint8_t rx = '5';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(1, result);
    TEST_ASSERT_EQUAL_UINT8(5, cmd);
}

/* Test Case: CH_03 - Reject numeric out-of-range command */
void test_CH_03_reject_numeric_out_of_range_8(void)
{
    uint8_t cmd = 0xBB; // Known init value
    uint8_t cmd_old = cmd;
    uint8_t rx = '8';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_UINT8(cmd_old, cmd); // Unchanged
}

/* Test Case: CH_04 - Reject non-numeric command */
void test_CH_04_reject_non_numeric_x(void)
{
    uint8_t cmd = 0xBB; // Known init value
    uint8_t cmd_old = cmd;
    uint8_t rx = 'x';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_UINT8(cmd_old, cmd); // Unchanged
}

/* Test Case: CH_05 - Reject UART receive failure */
void test_CH_05_reject_uart_receive_failure(void)
{
    uint8_t cmd = 0xBB; // Known init value
    uint8_t cmd_old = cmd;
    uint8_t rx = '0'; // Value shouldn't matter, as HAL_UART_Receive will fail

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_ERROR);
    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_UINT8(cmd_old, cmd); // Unchanged
}

/* Test Case: CH-06 - Handle NULL pointer safely */
void test_CH_06_handle_null_pointer_safely(void)
{
    /* Should not attempt to call HAL_UART_Receive if cmd_out is NULL */
    int result = CommandHandler_PollCommand(NULL);

    TEST_ASSERT_EQUAL(0, result);
    /* Test passes if no crash (Unity/host will catch segfault) */
}

/* Test Case: CH_07 - Do not modify output on invalid data (e.g., rx = '9') */
void test_CH_07_does_not_modify_output_on_invalid_data_9(void)
{
    uint8_t cmd = 0xBC; // Known init value
    uint8_t cmd_old = cmd;
    uint8_t rx = '9';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_UINT8(cmd_old, cmd); // Unchanged
}
