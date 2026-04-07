/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

/* Simulate external UART_HandleTypeDef from source */
UART_HandleTypeDef huart2;

void setUp(void)
{
    /* CMock handles mock init/reset automatically */
}

void tearDown(void)
{
}

/* ----------- Test Cases from Testplan_Command_Handler ----------- */

/* CH_01: Accept valid command '0'
 * rx = '0', HAL_OK
 * Expect: Return = 1 AND cmd = 0
 */
void test_CH_01_Accept_valid_command_0_returns_1_and_cmd_0(void)
{
    uint8_t cmd = 99; /* deliberately != 0 */
    uint8_t rx = '0';

    /* HAL_UART_Receive will be called with args: &huart2, <any>, 1, 10 */
    HAL_UART_Receive_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_UART_Receive_IgnoreArg_pData();
    HAL_UART_Receive_ReturnThruPtr_pData(&rx);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_EQUAL_UINT8(0, cmd);
}

/* CH_02: Accept valid command '5' (upper bound)
 * rx = '5', HAL_OK
 * Expect: Return = 1 AND cmd = 5
 */
void test_CH_02_Accept_valid_command_5_returns_1_and_cmd_5(void)
{
    uint8_t cmd = 99; /* deliberately != 5 */
    uint8_t rx = '5';

    HAL_UART_Receive_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_UART_Receive_IgnoreArg_pData();
    HAL_UART_Receive_ReturnThruPtr_pData(&rx);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_EQUAL_UINT8(5, cmd);
}

/* CH_03: Reject numeric out-of-range command ('8')
 * rx = '8', HAL_OK
 * Expect: Return = 0, cmd unchanged
 */
void test_CH_03_Reject_out_of_range_command_8_returns_0_and_cmd_unchanged(void)
{
    uint8_t cmd = 42; /* known value */
    uint8_t cmd_orig = cmd;
    uint8_t rx = '8';

    HAL_UART_Receive_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_UART_Receive_IgnoreArg_pData();
    HAL_UART_Receive_ReturnThruPtr_pData(&rx);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(cmd_orig, cmd);
}

/* CH_04: Reject non-numeric command ('x')
 * rx = 'x', HAL_OK
 * Expect: Return = 0, cmd unchanged
 */
void test_CH_04_Reject_non_numeric_command_x_returns_0_and_cmd_unchanged(void)
{
    uint8_t cmd = 15;
    uint8_t cmd_orig = cmd;
    uint8_t rx = 'x';

    HAL_UART_Receive_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_UART_Receive_IgnoreArg_pData();
    HAL_UART_Receive_ReturnThruPtr_pData(&rx);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(cmd_orig, cmd);
}

/* CH_05: Reject UART receive failure
 * HAL_UART_Receive returns HAL_ERROR
 * Expect: Return = 0, cmd unchanged
 */
void test_CH_05_Reject_UART_receive_failure_returns_0_and_cmd_unchanged(void)
{
    uint8_t cmd = 17;
    uint8_t cmd_orig = cmd;

    HAL_UART_Receive_ExpectAnyArgsAndReturn(HAL_ERROR);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(cmd_orig, cmd);
}

/* CH-06: Handle NULL pointer safely
 * cmd_out = NULL
 * Expect: Return = 0, no segfault (test passes if no crash)
 */
void test_CH_06_Handle_NULL_pointer_cmd_out_safely_returns_0(void)
{
    uint8_t result = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL_UINT8(0, result);
}

/* CH_07: Do not modify output on invalid data (e.g. rx = '9')
 * rx = '9', HAL_OK
 * *cmd_out remains unchanged
 */
void test_CH_07_Do_not_modify_output_on_invalid_data_9_cmd_unchanged(void)
{
    uint8_t cmd = 99;
    uint8_t cmd_orig = cmd;
    uint8_t rx = '9';

    HAL_UART_Receive_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_UART_Receive_IgnoreArg_pData();
    HAL_UART_Receive_ReturnThruPtr_pData(&rx);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(cmd_orig, cmd);
}

/* ----------- End of Test Cases ----------- */
