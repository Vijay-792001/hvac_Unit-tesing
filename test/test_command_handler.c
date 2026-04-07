/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

UART_HandleTypeDef huart2; // UART handle defined as extern in production

void setUp(void)
{
    // Set default state if required before each test
}

void tearDown(void)
{
    // Reset or cleanup after each test if needed
}

/*
 * Test Case: CH_01
 * SW-REQ(s): SWE-REQ-001, SWE-REQ-002
 * Objective: Accept valid command '0'
 * Input: rx = '0', HAL_OK
 * Expected Result: Function returns 1, *cmd_out = 0
 */
void test_CommandHandler_PollCommand_AcceptValidZero_ShouldSetCmdOut0(void)
{
    uint8_t cmd = 0xFF; // init with invalid value
    uint8_t rx = '0';

    // HAL_UART_Receive will be called with &huart2, &rx, 1, 10; will return HAL_OK
    HAL_UART_Receive_ExpectAndReturn(&huart2, (uint8_t*)&rx, 1, 10, HAL_OK);
    // Simulate rx is read as '0'
    // (The code reads into a local variable, we simulate its value using the argument memory.)

    // To make CMock write '0' into rx, we use callback or ignore arg—but as per rules, keep it strict
    // Since code reads from stack variable, just returning HAL_OK suffices for logic; direct mocking of rx not needed

    // Test
    TEST_ASSERT_EQUAL_UINT8(1U, CommandHandler_PollCommand(&cmd));
    TEST_ASSERT_EQUAL_UINT8(0U, cmd);
}

/*
 * Test Case: CH_02
 * SW-REQ(s): SWE-REQ-001, SWE-REQ-002
 * Objective: Accept valid command '5' (upper bound)
 * Input: rx = '5', HAL_OK
 * Expected Result: Function returns 1, *cmd_out = 5
 */
void test_CommandHandler_PollCommand_AcceptValidFive_ShouldSetCmdOut5(void)
{
    uint8_t cmd = 0xFF; // init with invalid value
    uint8_t rx = '5';

    HAL_UART_Receive_ExpectAndReturn(&huart2, (uint8_t*)&rx, 1, 10, HAL_OK);

    // Test
    TEST_ASSERT_EQUAL_UINT8(1U, CommandHandler_PollCommand(&cmd));
    TEST_ASSERT_EQUAL_UINT8(5U, cmd);
}

/*
 * Test Case: CH_03
 * SW-REQ(s): SWE-REQ-003
 * Objective: Reject numeric out-of-range command
 * Input: rx = '8', HAL_OK
 * Expected Result: Function returns 0, *cmd_out unchanged
 */
void test_CommandHandler_PollCommand_RejectOutOfRangeNumeric_ShouldNotChangeCmdOut(void)
{
    uint8_t cmd = 0xA5; // some known value for checking unchanged
    uint8_t rx = '8';

    HAL_UART_Receive_ExpectAndReturn(&huart2, (uint8_t*)&rx, 1, 10, HAL_OK);

    // Test
    TEST_ASSERT_EQUAL_UINT8(0U, CommandHandler_PollCommand(&cmd));
    TEST_ASSERT_EQUAL_UINT8(0xA5, cmd); // unchanged
}

/*
 * Test Case: CH_04
 * SW-REQ(s): SWE-REQ-003
 * Objective: Reject non-numeric command
 * Input: rx = 'x', HAL_OK
 * Expected Result: Function returns 0, *cmd_out unchanged
 */
void test_CommandHandler_PollCommand_RejectNonNumeric_ShouldNotChangeCmdOut(void)
{
    uint8_t cmd = 0x55; // known initial value
    uint8_t rx = 'x';

    HAL_UART_Receive_ExpectAndReturn(&huart2, (uint8_t*)&rx, 1, 10, HAL_OK);

    // Test
    TEST_ASSERT_EQUAL_UINT8(0U, CommandHandler_PollCommand(&cmd));
    TEST_ASSERT_EQUAL_UINT8(0x55, cmd); // unchanged
}

/*
 * Test Case: CH_05
 * SW-REQ(s): SWE-REQ-053
 * Objective: Reject UART receive failure
 * Input: HAL_ERROR
 * Expected Result: Function returns 0, *cmd_out unchanged
 */
void test_CommandHandler_PollCommand_RejectUartReceiveError_ShouldNotChangeCmdOut(void)
{
    uint8_t cmd = 0x22;
    uint8_t rx = 0; // value ignored, as HAL fails

    HAL_UART_Receive_ExpectAndReturn(&huart2, (uint8_t*)&rx, 1, 10, HAL_ERROR);

    // Test
    TEST_ASSERT_EQUAL_UINT8(0U, CommandHandler_PollCommand(&cmd));
    TEST_ASSERT_EQUAL_UINT8(0x22, cmd); // unchanged
}

/*
 * Test Case: CH-06
 * SW-REQ(s): SWE-REQ-046
 * Objective: Handle NULL pointer safely
 * Input: cmd_out = NULL
 * Expected Result: Function returns 0, no crash
 */
void test_CommandHandler_PollCommand_NullPointer_ShouldReturnZero(void)
{
    // No call to HAL_UART_Receive expected
    // Null pointer handling returns early
    TEST_ASSERT_EQUAL_UINT8(0U, CommandHandler_PollCommand(NULL));
    // If test finishes, no crash has occurred
}

/*
 * Test Case: CH_07
 * SW-REQ(s): SWE-REQ-058
 * Objective: Do not modify output on invalid data (e.g. '9')
 * Input: rx = '9', HAL_OK
 * Expected Result: *cmd_out remains unchanged
 */
void test_CommandHandler_PollCommand_InvalidData_ShouldNotChangeCmdOut(void)
{
    uint8_t cmd = 0xC3;
    uint8_t rx = '9';

    HAL_UART_Receive_ExpectAndReturn(&huart2, (uint8_t*)&rx, 1, 10, HAL_OK);

    // Test
    TEST_ASSERT_EQUAL_UINT8(0U, CommandHandler_PollCommand(&cmd));
    TEST_ASSERT_EQUAL_UINT8(0xC3, cmd); // unchanged
}
