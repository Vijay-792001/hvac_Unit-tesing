/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h" // CMock-generated mock for HAL dependencies

// Dummy UART handle declared as extern in production code
UART_HandleTypeDef huart2;

// Helper for initial value of cmd_out in tests
#define INVALID_CMD_VALUE 0xA5

void setUp(void)
{
    // Optional: Clear or init anything here if needed.
    // (Not needed for current stateless Command Handler function.)
}

void tearDown(void)
{
    // Optional: Clean up after each test if needed.
}

/* CH_01: Accept valid command '0'
 * - Inputs: rx = '0', HAL_OK
 * - Expects: Return = 1 AND *cmd_out = 0
 */
void test_CH_01_Accept_valid_command_0(void)
{
    uint8_t cmd_out = INVALID_CMD_VALUE;
    uint8_t rx = '0';

    // HAL_UART_Receive is expected strictly as called in code.
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    // Simulate receiving '0' character; CMock will copy rx into the pointer.
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    uint8_t ret = CommandHandler_PollCommand(&cmd_out);

    TEST_ASSERT_EQUAL_UINT8(1U, ret);
    TEST_ASSERT_EQUAL_UINT8(0U, cmd_out);
}

/* CH_02: Accept valid command '5' (upper bound)
 * - Inputs: rx = '5', HAL_OK
 * - Expects: Return = 1 AND *cmd_out = 5
 */
void test_CH_02_Accept_valid_command_5(void)
{
    uint8_t cmd_out = INVALID_CMD_VALUE;
    uint8_t rx = '5';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    uint8_t ret = CommandHandler_PollCommand(&cmd_out);

    TEST_ASSERT_EQUAL_UINT8(1U, ret);
    TEST_ASSERT_EQUAL_UINT8(5U, cmd_out);
}

/* CH_03: Reject numeric out-of-range command
 * - Inputs: rx = '8', HAL_OK
 * - Expects: Return = 0, *cmd_out unchanged
 */
void test_CH_03_Reject_numeric_out_of_range_command(void)
{
    uint8_t cmd_out = INVALID_CMD_VALUE;
    uint8_t rx = '8';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    uint8_t ret = CommandHandler_PollCommand(&cmd_out);

    TEST_ASSERT_EQUAL_UINT8(0U, ret);
    TEST_ASSERT_EQUAL_UINT8(INVALID_CMD_VALUE, cmd_out);
}

/* CH_04: Reject non-numeric command
 * - Inputs: rx = 'x', HAL_OK
 * - Expects: Return = 0, *cmd_out unchanged
 */
void test_CH_04_Reject_non_numeric_command(void)
{
    uint8_t cmd_out = INVALID_CMD_VALUE;
    uint8_t rx = 'x';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    uint8_t ret = CommandHandler_PollCommand(&cmd_out);

    TEST_ASSERT_EQUAL_UINT8(0U, ret);
    TEST_ASSERT_EQUAL_UINT8(INVALID_CMD_VALUE, cmd_out);
}

/* CH_05: Reject UART receive failure
 * - Inputs: HAL_UART_Receive returns HAL_ERROR
 * - Expects: Return = 0, *cmd_out unchanged
 */
void test_CH_05_Reject_UART_receive_failure(void)
{
    uint8_t cmd_out = INVALID_CMD_VALUE;
    uint8_t rx = 0; // Value won't matter since receive will fail

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_ERROR);

    uint8_t ret = CommandHandler_PollCommand(&cmd_out);

    TEST_ASSERT_EQUAL_UINT8(0U, ret);
    TEST_ASSERT_EQUAL_UINT8(INVALID_CMD_VALUE, cmd_out);
}

/* CH-06: Handle NULL pointer safely
 * - Inputs: cmd_out = NULL
 * - Expects: Return = 0, no crash
 */
void test_CH_06_Handle_NULL_pointer_safely(void)
{
    // Should return 0 and not call HAL_UART_Receive
    uint8_t ret = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL_UINT8(0U, ret);
    // No further asserts: if test runner does not crash, test passes
}

/* CH_07: Do not modify output on invalid data
 * - Inputs: rx = '9', HAL_OK
 * - Expects: *cmd_out remains unchanged
 */
void test_CH_07_Do_not_modify_output_on_invalid_data(void)
{
    uint8_t cmd_out = INVALID_CMD_VALUE;
    uint8_t rx = '9';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    CommandHandler_PollCommand(&cmd_out);

    TEST_ASSERT_EQUAL_UINT8(INVALID_CMD_VALUE, cmd_out);
}
