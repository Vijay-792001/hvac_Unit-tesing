/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

UART_HandleTypeDef huart2;

void setUp(void)
{
    // Reset the mock for every test
    mock_stm32f4xx_hal_Init();
}

void tearDown(void)
{
}

//
// Test Case: CH_01
// SWE-REQ-001,002: Accept valid command '0'
// Expect: Returns 1, *cmd_out = 0
//
void test_CommandHandler_Accepts_Valid_Command_0_CH_01(void)
{
    uint8_t cmd = 99; // set to non-zero to check modification
    uint8_t rx = '0';
    // HAL_UART_Receive should be called as per control flow
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    uint8_t result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_EQUAL_UINT8(0, cmd);
}

//
// Test Case: CH_02
// SWE-REQ-001,002: Accept valid command '5' (upper bound)
// Expect: Returns 1, *cmd_out = 5
//
void test_CommandHandler_Accepts_Valid_Command_5_CH_02(void)
{
    uint8_t cmd = 42;
    uint8_t rx = '5';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    uint8_t result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_EQUAL_UINT8(5, cmd);
}

//
// Test Case: CH_03
// SWE-REQ-003: Reject numeric out-of-range command ('8')
// Expect: Returns 0, *cmd_out unchanged
//
void test_CommandHandler_Rejects_OutOfRange_Command_8_CH_03(void)
{
    uint8_t cmd = 7;
    uint8_t cmd_initial = cmd;
    uint8_t rx = '8';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    uint8_t result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(cmd_initial, cmd);
}

//
// Test Case: CH_04
// SWE-REQ-003: Reject non-numeric command ('x')
// Expect: Returns 0, *cmd_out unchanged
//
void test_CommandHandler_Rejects_NonNumeric_Command_x_CH_04(void)
{
    uint8_t cmd = 2;
    uint8_t cmd_initial = cmd;
    uint8_t rx = 'x';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    uint8_t result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(cmd_initial, cmd);
}

//
// Test Case: CH_05
// SWE-REQ-053: Reject UART receive failure (HAL_ERROR)
// Expect: Returns 0, *cmd_out unchanged
//
void test_CommandHandler_Rejects_UART_Receive_Failure_CH_05(void)
{
    uint8_t cmd = 3;
    uint8_t cmd_initial = cmd;
    // Note: rx is not relevant as UART fails
    HAL_UART_Receive_ExpectAndReturn(&huart2, (uint8_t*)UnityAnyPtr(), 1, 10, HAL_ERROR);
    uint8_t result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(cmd_initial, cmd);
}

//
// Test Case: CH-06
// SWE-REQ-046: Handle NULL pointer safely
// Expect: No crash, returns 0
//
void test_CommandHandler_Handles_Null_Pointer_CH_06(void)
{
    // Should return 0 and not call HAL_UART_Receive
    uint8_t result = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL_UINT8(0, result);
    // Ensure no HAL_UART_Receive call was made (CMock enforces strict ordering)
}

//
// Test Case: CH_07
// SWE-REQ-058: Do not modify output on invalid data ('9')
// Expect: *cmd_out remains unchanged
//
void test_CommandHandler_Does_Not_Modify_Output_On_Invalid_Data_CH_07(void)
{
    uint8_t cmd = 9;
    uint8_t cmd_initial = cmd;
    uint8_t rx = '9';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    uint8_t result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(cmd_initial, cmd);
}
