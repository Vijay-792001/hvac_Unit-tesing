#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* CH_01: Accept valid command '0' */
void test_CH_01_Accept_valid_command_0(void)
{
    UART_HandleTypeDef dummy_uart;
    uint8_t cmd_out = 88; // Initialize changed if valid
    uint8_t input = '0';
    HAL_UART_Receive_ExpectAndReturn(&dummy_uart, &input, 1, 100, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL_UINT8(0, cmd_out);
}

/* CH_02: Accept valid command '5' (upper bound) */
void test_CH_02_Accept_valid_command_5(void)
{
    UART_HandleTypeDef dummy_uart;
    uint8_t cmd_out = 99;
    uint8_t input = '5';
    HAL_UART_Receive_ExpectAndReturn(&dummy_uart, &input, 1, 100, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL_UINT8(5, cmd_out);
}

/* CH_03: Reject numeric out-of-range command */
void test_CH_03_Reject_numeric_out_of_range_command(void)
{
    UART_HandleTypeDef dummy_uart;
    uint8_t cmd_out = 0xA4;
    uint8_t input = '8';
    HAL_UART_Receive_ExpectAndReturn(&dummy_uart, &input, 1, 100, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0xA4, cmd_out); // Should not change
}

/* CH_04: Reject non-numeric command */
void test_CH_04_Reject_non_numeric_command(void)
{
    UART_HandleTypeDef dummy_uart;
    uint8_t cmd_out = 0xBB;
    uint8_t input = 'x';
    HAL_UART_Receive_ExpectAndReturn(&dummy_uart, &input, 1, 100, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0xBB, cmd_out);
}

/* CH_05: Reject UART receive failure */
void test_CH_05_Reject_UART_receive_failure(void)
{
    UART_HandleTypeDef dummy_uart;
    uint8_t cmd_out = 0xCD;
    uint8_t input = '2';
    HAL_UART_Receive_ExpectAndReturn(&dummy_uart, &input, 1, 100, HAL_ERROR);

    int ret = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0xCD, cmd_out);
}

/* CH_06: Handle NULL pointer safely */
void test_CH_06_Handle_NULL_pointer_safely(void)
{
    UART_HandleTypeDef dummy_uart;
    uint8_t input = '1';
    HAL_UART_Receive_ExpectAndReturn(&dummy_uart, &input, 1, 100, HAL_OK);

    // Should not crash if NULL
    (void)CommandHandler_PollCommand(NULL);
    // No assertion—test passes if no crash
}

/* CH_07: Do not modify output on invalid data */
void test_CH_07_Do_not_modify_output_on_invalid_data(void)
{
    UART_HandleTypeDef dummy_uart;
    uint8_t cmd_out = 0x5A;
    uint8_t input = '9';
    HAL_UART_Receive_ExpectAndReturn(&dummy_uart, &input, 1, 100, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0x5A, cmd_out);
}
