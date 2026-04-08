/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

extern UART_HandleTypeDef huart2;

void setUp(void) {}
void tearDown(void) {}

/* CH_01: Accept valid command '0' */
void test_command_handler_CH_01(void)
{
    uint8_t cmd = 255; // Set to nonzero to ensure function writes it
    uint8_t rx_val = '0';

    // HAL_UART_Receive returns '0' and HAL_OK
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_EQUAL_UINT8(0, cmd);
}

/* CH_02: Accept valid command '5' (upper bound) */
void test_command_handler_CH_02(void)
{
    uint8_t cmd = 255;
    uint8_t rx_val = '5';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_EQUAL_UINT8(5, cmd);
}

/* CH_03: Reject numeric out-of-range command */
void test_command_handler_CH_03(void)
{
    uint8_t cmd = 99; // Should stay unchanged
    uint8_t cmd_backup = cmd;
    uint8_t rx_val = '8';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(cmd_backup, cmd);
}

/* CH_04: Reject non-numeric command */
void test_command_handler_CH_04(void)
{
    uint8_t cmd = 99; // Should stay unchanged
    uint8_t cmd_backup = cmd;
    uint8_t rx_val = 'x';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(cmd_backup, cmd);
}

/* CH_05: Reject UART receive failure */
void test_command_handler_CH_05(void)
{
    uint8_t cmd = 42; // Should remain unchanged

    // HAL_UART_Receive returns HAL_ERROR, rx can be any value
    HAL_UART_Receive_ExpectAndReturn(&huart2, (uint8_t*)UNITY_ANY, 1, 10, HAL_ERROR);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(42, cmd);
}

/* CH-06: Handle NULL pointer safely */
void test_command_handler_CH_06(void)
{
    // No mock expectations; function should not call UART
    uint8_t result = CommandHandler_PollCommand(NULL);

    TEST_ASSERT_EQUAL_UINT8(0, result);
}

/* CH_07: Do not modify output on invalid data ('9') */
void test_command_handler_CH_07(void)
{
    uint8_t cmd = 64;
    uint8_t cmd_backup = cmd;
    uint8_t rx_val = '9';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(cmd_backup, cmd);
}
