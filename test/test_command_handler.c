/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

#include <string.h> // for memset if needed

// The CommandHandler module uses an external UART handle
UART_HandleTypeDef huart2;

void setUp(void)
{
    // No set up required for these tests (stateless function)
}

void tearDown(void)
{
    // No tear down required
}

/* Test Case: CH_01 (SWE-REQ-001,002)
 * Accept valid command '0'
 * Input: rx = '0', HAL_OK
 * Expect: returns 1, *cmd_out = 0
 */
void test_CH_01_AcceptValidCommand_0_ShouldReturn1_AndCmdOut0(void)
{
    uint8_t cmd = 99; // Nonzero start
    uint8_t rx = '0';

    // Strict ordering: HAL_UART_Receive called once
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&rx);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_EQUAL_UINT8(0, cmd);
}

/* Test Case: CH_02 (SWE-REQ-001,002)
 * Accept valid command '5' (upper bound)
 * Input: rx = '5', HAL_OK
 * Expect: returns 1, *cmd_out = 5
 */
void test_CH_02_AcceptValidCommand_5_ShouldReturn1_AndCmdOut5(void)
{
    uint8_t cmd = 123;
    uint8_t rx = '5';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&rx);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_EQUAL_UINT8(5, cmd);
}

/* Test Case: CH_03 (SWE-REQ-003)
 * Reject numeric out-of-range command
 * Input: rx = '8', HAL_OK
 * Expect: returns 0, *cmd_out unchanged
 */
void test_CH_03_RejectOutOfRangeCommand_ShouldReturn0_CmdUnchanged(void)
{
    uint8_t orig_cmd = 42;
    uint8_t cmd = orig_cmd;
    uint8_t rx = '8';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&rx);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(orig_cmd, cmd); // Unchanged
}

/* Test Case: CH_04 (SWE-REQ-003)
 * Reject non-numeric command
 * Input: rx = 'x', HAL_OK
 * Expect: returns 0, *cmd_out unchanged
 */
void test_CH_04_RejectNonNumericCommand_ShouldReturn0_CmdUnchanged(void)
{
    uint8_t orig_cmd = 99;
    uint8_t cmd = orig_cmd;
    uint8_t rx = 'x';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&rx);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(orig_cmd, cmd);
}

/* Test Case: CH_05 (SWE-REQ-053)
 * Reject UART receive failure
 * Input: HAL_ERROR
 * Expect: returns 0, *cmd_out unchanged
 */
void test_CH_05_RejectUartReceiveFailure_ShouldReturn0_CmdUnchanged(void)
{
    uint8_t orig_cmd = 55;
    uint8_t cmd = orig_cmd;
    uint8_t rx_dontcare = 0xAB;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_dontcare, 1, 10, HAL_ERROR);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(orig_cmd, cmd);
}

/* Test Case: CH-06 (SWE-REQ-046)
 * Handle NULL pointer safely
 * Input: cmd_out = NULL
 * Expect: does not crash, returns 0
 */
void test_CH_06_HandleNullPointer_NoCrash_Return0(void)
{
    // Should survive with NULL and not call HAL_UART_Receive
    uint8_t result = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL_UINT8(0, result);
    // Nothing else to check; absence of crash is test pass
}

/* Test Case: CH_07 (SWE-REQ-058)
 * Do not modify output on invalid data
 * Input: rx = '9', HAL_OK
 * Expect: *cmd_out remains unchanged
 */
void test_CH_07_InvalidDataDoesNotModifyCmdOut(void)
{
    uint8_t orig_cmd = 7;
    uint8_t cmd = orig_cmd;
    uint8_t rx = '9';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&rx);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(orig_cmd, cmd);
}
