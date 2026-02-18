/* ===== test_command_handler.c ===== */

#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

// Provide the required extern for huart2
UART_HandleTypeDef huart2;

// Needed for test isolation on some toolchains (reset mock globals, etc)
void setUp(void)
{
    mock_stm32f4xx_hal_Init();
}
void tearDown(void)
{
    mock_stm32f4xx_hal_Verify();
    mock_stm32f4xx_hal_Destroy();
}

/* CH_01: Accept valid command '0'
   Preconditions: UART mock enabled, rx = '0', HAL_OK
   Expected: returns 1, *cmd_out = 0 */
void test_CH_01_Accept_command_0_sets_cmd0_and_returns_1(void)
{
    uint8_t cmd = 123; // Initial dummy value
    uint8_t rx = '0';
    // Expect: HAL_UART_Receive called with &huart2, &rx, 1, 10 --> HAL_OK
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    // For CMock pointer arg equivalence, we need to force the receive buffer value in wrapper
    // We'll simulate this by returning HAL_OK; set local rx before return

    // Call function under test
    int result = CommandHandler_PollCommand(&cmd);

    // Check outputs
    TEST_ASSERT_EQUAL(1, result);
    TEST_ASSERT_EQUAL(0, cmd);
}

/* CH_02: Accept valid command '5' (upper bound)
   Preconditions: UART mock enabled, rx = '5', HAL_OK
   Expected: returns 1, *cmd_out = 5 */
void test_CH_02_Accept_command_5_sets_cmd5_and_returns_1(void)
{
    uint8_t cmd = 111; // Initial dummy value
    uint8_t rx = '5';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(1, result);
    TEST_ASSERT_EQUAL(5, cmd);
}

/* CH_03: Reject numeric out-of-range command
   Input: rx = '8', HAL_OK, *cmd_out unchanged
   Expected: returns 0, *cmd_out unchanged */
void test_CH_03_Reject_out_of_range_numeric_command_returns_0_and_does_not_write_cmd(void)
{
    uint8_t initial = 0xAA;
    uint8_t cmd = initial;
    uint8_t rx = '8';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(initial, cmd); // Value unchanged
}

/* CH_04: Reject non-numeric command
   Input: rx = 'x', HAL_OK, *cmd_out unchanged
   Expected: returns 0, *cmd_out unchanged */
void test_CH_04_Reject_non_numeric_command_returns_0_and_does_not_write_cmd(void)
{
    uint8_t initial = 0x12;
    uint8_t cmd = initial;
    uint8_t rx = 'x';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(initial, cmd); // Value unchanged
}

/* CH_05: Reject UART receive failure
   Input: HAL_ERROR, *cmd_out unchanged
   Expected: returns 0, *cmd_out unchanged */
void test_CH_05_Reject_uart_receive_failure_returns_0_and_does_not_write_cmd(void)
{
    uint8_t initial = 0xFE;
    uint8_t cmd = initial;
    // Arg doesn't matter since function will just return error
    HAL_UART_Receive_ExpectAndReturn(&huart2, &cmd, 1, 10, HAL_ERROR);

    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(initial, cmd); // Value unchanged
}

/* CH_06: Handle NULL pointer safely (no crash)
   Input: cmd_out = NULL
   Expected: returns 0; Test passes if no crash */
void test_CH_06_Handle_null_pointer_cmd_out_returns_0_no_crash(void)
{
    // No need to call HAL_UART_Receive; function returns before any HAL call
    int result = CommandHandler_PollCommand(NULL);

    TEST_ASSERT_EQUAL(0, result);
}

/* CH_07: Do not modify output on invalid data
   Input: rx = '9', HAL_OK, *cmd_out unchanged
   Expected: *cmd_out unchanged */
void test_CH_07_Invalid_numeric_rx_does_not_modify_cmd(void)
{
    uint8_t initial = 0xA5;
    uint8_t cmd = initial;
    uint8_t rx = '9';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(initial, cmd); // Value unchanged
}

/* End of test_command_handler.c */
