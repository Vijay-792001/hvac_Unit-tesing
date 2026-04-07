/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

#include <string.h> // For memset if needed

// External hardware handle referenced by command_handler.c
UART_HandleTypeDef huart2;

// setUp and tearDown are required by Unity,
// even if not needed by these tests at the moment.
void setUp(void)
{
    // Reset or initialize anything as needed before each test
    memset(&huart2, 0, sizeof(huart2));
}

void tearDown(void)
{
    // Clean up after each test (not needed here)
}

/* ====================
 * Test Cases from Testplan_Command_Handler.txt
 * ====================
 */

/*
Test Case ID: CH_01
Objective: Accept valid command '0'
SW-REQ: SWE-REQ-001,002
Description: When UART receives '0' (ASCII 48), function returns 1, *cmd_out = 0.
*/
void test_CH_01_Accept_valid_command_0(void)
{
    uint8_t cmd = 0xAA; // Unusual default for detection
    uint8_t rx = '0';

    // Expectation: HAL_UART_Receive called, returns HAL_OK, and writes '0' to rx pointer.
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    // CMock will NOT transfer variable values, we must simulate as if rx got '0'

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_INT(1, ret);
    TEST_ASSERT_EQUAL_UINT8(0, cmd); // Should parse '0' -> 0
}

/*
Test Case ID: CH_02
Objective: Accept valid command '5' (upper bound)
SW-REQ: SWE-REQ-001,002
Description: When UART receives '5', function returns 1, *cmd_out = 5.
*/
void test_CH_02_Accept_valid_command_5_upper_bound(void)
{
    uint8_t cmd = 0xAA;
    uint8_t rx = '5';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_INT(1, ret);
    TEST_ASSERT_EQUAL_UINT8(5, cmd);
}

/*
Test Case ID: CH_03
Objective: Reject numeric out-of-range command (e.g., '8')
SW-REQ: SWE-REQ-003
Description: UART receives '8', function must return 0 and *cmd_out is unchanged.
*/
void test_CH_03_Reject_out_of_range_command_8(void)
{
    uint8_t cmd = 0x55; // Sentinel value
    uint8_t rx = '8';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0x55, cmd); // Should remain unchanged
}

/*
Test Case ID: CH_04
Objective: Reject non-numeric command
SW-REQ: SWE-REQ-003
Description: UART receives 'x', function must return 0 and *cmd_out is unchanged.
*/
void test_CH_04_Reject_non_numeric_command_x(void)
{
    uint8_t cmd = 0x22;
    uint8_t rx = 'x';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0x22, cmd); // Should not be modified
}

/*
Test Case ID: CH_05
Objective: Reject UART receive failure (HAL_ERROR)
SW-REQ: SWE-REQ-053
Description: If HAL_UART_Receive returns HAL_ERROR, function must return 0 and leave *cmd_out unchanged.
*/
void test_CH_05_Reject_UART_receive_failure(void)
{
    uint8_t cmd = 0x33;

    // rx pointer value does not matter since UART receive fails
    HAL_UART_Receive_ExpectAndReturn(&huart2, (uint8_t *)UNITY_ANY, 1, 10, HAL_ERROR);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0x33, cmd);
}

/*
Test Case ID: CH_06
Objective: NULL pointer safety
SW-REQ: SWE-REQ-046
Description: If cmd_out is NULL, function must return 0. Should not call HAL_UART_Receive. (No crash.)
*/
void test_CH_06_Handle_NULL_pointer_safely(void)
{
    // No expectations on HAL_UART_Receive at all
    int ret = CommandHandler_PollCommand(NULL);

    TEST_ASSERT_EQUAL_INT(0, ret);
    // Test passes if no segfault; Unity handles this just by running to completion
}

/*
Test Case ID: CH_07
Objective: Do not modify output on invalid data (e.g., rx = '9')
SW-REQ: SWE-REQ-058
Description: Should not change *cmd_out for rx outside '0'..'5'
*/
void test_CH_07_Do_not_modify_output_on_invalid_data_9(void)
{
    uint8_t cmd = 0xAB;
    uint8_t rx = '9';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0xAB, cmd);
}
// End of test_command_handler.c
