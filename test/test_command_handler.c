/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

UART_HandleTypeDef huart2;

void setUp(void)
{
    // Reset and initialize any global state if needed before each test
}

void tearDown(void)
{
    // Cleanup after each test
}

/* CH_01: Accept valid command '0'
   Preconditions: UART mock enabled
   Input: rx = '0', HAL_OK
   Expected Result: Function returns 1, *cmd_out = 0 */
void test_CH_01_Accept_valid_command_0(void)
{
    uint8_t cmd = 0xFF;
    uint8_t rx_val = '0';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);

    // To simulate actual receive, the mock will assign '0' to *pData. In ceedling, we can simulate via Hook or wrapper.
    uint8_t actual_rx = rx_val;
    // Place value in rx_val, as CMock cannot "write" to pData directly without custom callback.

    // Test
    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, result, "Should return 1 for valid command '0'");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, cmd, "cmd_out should be set to 0");
}

/* CH_02: Accept valid command '5' (upper bound)
   Preconditions: UART mock enabled
   Input: rx = '5', HAL_OK
   Expected Result: Function returns 1, *cmd_out = 5 */
void test_CH_02_Accept_valid_command_5_upper_bound(void)
{
    uint8_t cmd = 0xFF;
    uint8_t rx_val = '5';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, result, "Should return 1 for valid command '5'");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(5, cmd, "cmd_out should be set to 5");
}

/* CH_03: Reject numeric out-of-range command
   Preconditions: UART mock enabled
   Input: rx = '8', HAL_OK
   Expected Result: Function returns 0, *cmd_out unchanged */
void test_CH_03_Reject_numeric_out_of_range_command(void)
{
    uint8_t cmd = 0xA5; // sentinel previous value
    uint8_t prev_cmd = cmd;
    uint8_t rx_val = '8';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, result, "Should return 0 for out-of-range numeric command '8'");
    TEST_ASSERT_EQUAL_UINT8(prev_cmd, cmd); // Should remain unchanged
}

/* CH_04: Reject non-numeric command
   Preconditions: UART mock enabled
   Input: rx = 'x', HAL_OK
   Expected Result: Function returns 0, *cmd_out unchanged */
void test_CH_04_Reject_non_numeric_command(void)
{
    uint8_t cmd = 0xBB;
    uint8_t prev_cmd = cmd;
    uint8_t rx_val = 'x';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, result, "Should return 0 for non-numeric command 'x'");
    TEST_ASSERT_EQUAL_UINT8(prev_cmd, cmd); // cmd_out unchanged
}

/* CH_05: Reject UART receive failure
   Preconditions: UART mock enabled
   Input: HAL_ERROR
   Expected Result: Function returns 0, *cmd_out unchanged */
void test_CH_05_Reject_UART_receive_failure(void)
{
    uint8_t cmd = 0x42;
    uint8_t prev_cmd = cmd;
    // Don't care *pData, but must call receive and return error
    HAL_UART_Receive_ExpectAndReturn(&huart2, (uint8_t *)ANY, 1, 10, HAL_ERROR);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, result, "Should return 0 for UART receive error");
    TEST_ASSERT_EQUAL_UINT8(prev_cmd, cmd); // unchanged
}

/* CH-06: Handle NULL pointer safely
   Preconditions: UART mock enabled
   Input: cmd_out = NULL
   Expected Result: No crash (Test passes if no segmentation fault) */
void test_CH_06_Handle_NULL_pointer_safely(void)
{
    // Should not attempt to call HAL_UART_Receive at all
    uint8_t result = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, result, "Should return 0 if cmd_out pointer is NULL");
}

/* CH_07: Do not modify output on invalid data
   Preconditions: UART mock enabled
   Input: rx = '9', HAL_OK
   Expected Result: *cmd_out remains unchanged */
void test_CH_07_Do_not_modify_output_on_invalid_data(void)
{
    uint8_t cmd = 0x33;
    uint8_t prev_cmd = cmd;
    uint8_t rx_val = '9';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, result, "Should return 0 for invalid command '9'");
    TEST_ASSERT_EQUAL_UINT8(prev_cmd, cmd); // cmd_out unchanged
}
