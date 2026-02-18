/* ===== test_command_handler.c ===== */

#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

#include <string.h> // For memset

// Simulate extern UART_HandleTypeDef huart2
UART_HandleTypeDef huart2;

void setUp(void)
{
    memset(&huart2, 0, sizeof(huart2));
}

void tearDown(void)
{
}

// CH_01: Accept valid command '0'
// Test Plan: rx = '0', HAL_OK -> Function returns 1, *cmd_out = 0
void test_CH_01_Accept_valid_command_0_returns_1_and_cmd_out_0(void)
{
    uint8_t fake_rx = '0';
    uint8_t cmd_out = 123; // init to nonzero

    // HAL_UART_Receive will be called & should write fake_rx
    HAL_UART_Receive_ExpectAndReturn(&huart2, &fake_rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&fake_rx, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, result, "Should return 1 for valid input '0'");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, cmd_out, "cmd_out should be 0");
}

// CH_02: Accept valid command '5' (upper bound)
void test_CH_02_Accept_valid_command_5_returns_1_and_cmd_out_5(void)
{
    uint8_t fake_rx = '5';
    uint8_t cmd_out = 99;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &fake_rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&fake_rx, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, result, "Should return 1 for valid input '5'");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(5, cmd_out, "cmd_out should be 5");
}

// CH_03: Reject numeric out-of-range command ('8')
void test_CH_03_Reject_out_of_range_numeric_command_returns_0_and_cmd_out_unchanged(void)
{
    uint8_t fake_rx = '8';
    uint8_t cmd_out = 42;
    uint8_t original_cmd = cmd_out;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &fake_rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&fake_rx, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, result, "Should return 0 for out-of-range input '8'");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(original_cmd, cmd_out, "*cmd_out should remain unchanged on out-of-range");
}

// CH_04: Reject non-numeric command ('x')
void test_CH_04_Reject_non_numeric_command_x_returns_0_and_cmd_out_unchanged(void)
{
    uint8_t fake_rx = 'x';
    uint8_t cmd_out = 7;
    uint8_t original_cmd = cmd_out;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &fake_rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&fake_rx, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, result, "Should return 0 for non-numeric input 'x'");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(original_cmd, cmd_out, "*cmd_out should remain unchanged on non-numeric");
}

// CH_05: Reject UART receive failure
void test_CH_05_Reject_UART_receive_failure_returns_0_and_cmd_out_unchanged(void)
{
    uint8_t rxbuf = 0xAA; // whatever (shouldn't be copied)
    uint8_t cmd_out = 55;
    uint8_t original_cmd = cmd_out;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rxbuf, 1, 10, HAL_ERROR);

    uint8_t result = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, result, "Should return 0 if HAL_UART_Receive fails");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(original_cmd, cmd_out, "*cmd_out should remain unchanged if receive fails");
}

// CH-06: Handle NULL pointer safely
void test_CH_06_Handle_NULL_pointer_cmd_out(void)
{
    // No call to HAL_UART_Receive should occur
    uint8_t result = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, result, "Should return 0 and not crash on NULL cmd_out");
}

// CH_07: Do not modify output on invalid data ('9')
void test_CH_07_Do_not_modify_output_on_invalid_data_9(void)
{
    uint8_t fake_rx = '9';
    uint8_t cmd_out = 11;
    uint8_t original_cmd = cmd_out;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &fake_rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&fake_rx, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, result, "Should return 0 for input '9' (invalid)");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(original_cmd, cmd_out, "*cmd_out should remain unchanged for invalid value '9'");
}
