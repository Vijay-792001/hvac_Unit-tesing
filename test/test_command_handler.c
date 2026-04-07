/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

extern UART_HandleTypeDef huart2;

// Testplan Reference: CH_01
// Accept valid command '0' (SWE-REQ-001,002)
void test_CH_01_Accept_valid_command_0_should_return_1_and_cmd_out_is_0(void)
{
    uint8_t cmd = 100; // Init to nonsensical value to check override
    uint8_t rx = '0';

    // Strict mock: HAL_UART_Receive expected with rx output, returns HAL_OK
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL_UINT8(0, cmd);
}

// Testplan Reference: CH_02
// Accept valid command '5' (upper bound) (SWE-REQ-001,002)
void test_CH_02_Accept_valid_command_5_should_return_1_and_cmd_out_is_5(void)
{
    uint8_t cmd = 99;
    uint8_t rx = '5';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL_UINT8(5, cmd);
}

// Testplan Reference: CH_03
// Reject numeric out-of-range command (SWE-REQ-003)
void test_CH_03_Reject_out_of_range_command_8_should_return_0_and_cmd_out_unchanged(void)
{
    uint8_t cmd_before = 2;
    uint8_t cmd = cmd_before;
    uint8_t rx = '8';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT8(cmd_before, cmd); // cmd_out unchanged
}

// Testplan Reference: CH_04
// Reject non-numeric command 'x' (SWE-REQ-003)
void test_CH_04_Reject_non_numeric_command_x_should_return_0_and_cmd_out_unchanged(void)
{
    uint8_t cmd_before = 4;
    uint8_t cmd = cmd_before;
    uint8_t rx = 'x';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT8(cmd_before, cmd); // cmd_out unchanged
}

// Testplan Reference: CH_05
// Reject UART receive failure (SWE-REQ-053)
void test_CH_05_UART_receive_failure_should_return_0_and_cmd_out_unchanged(void)
{
    uint8_t cmd_before = 3;
    uint8_t cmd = cmd_before;
    uint8_t rx = 0xAB;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_ERROR);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT8(cmd_before, cmd); // cmd_out unchanged
}

// Testplan Reference: CH-06
// Handle NULL pointer safely (SWE-REQ-046)
void test_CH_06_NULL_pointer_should_return_0_and_not_crash(void)
{
    // No mock expectations. The function must guard against NULL.
    int ret = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL(0, ret);
    // No crash/segfault: Passes if Unity test runner completes
}

// Testplan Reference: CH_07
// Do not modify output on invalid data ('9'), rx='9' (SWE-REQ-058)
void test_CH_07_Invalid_command_9_should_not_modify_cmd_out(void)
{
    uint8_t cmd_before = 5;
    uint8_t cmd = cmd_before;
    uint8_t rx = '9';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT8(cmd_before, cmd); // cmd_out unchanged
}
