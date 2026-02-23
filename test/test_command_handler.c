/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

static UART_HandleTypeDef dummy_huart2;
extern UART_HandleTypeDef huart2;

void setUp(void)
{
    /* Arrange: required for global UART_HandleTypeDef usage */
    huart2 = dummy_huart2;
}

void tearDown(void)
{
}

/*
CH_01: Accept valid command '0'
Objective: Function returns 1, *cmd_out = 0
Input: rx = '0', HAL_OK
*/
void test_CH_01_AcceptsValidCommand0(void)
{
    uint8_t cmd_result = 123; // any value, expect to be overwritten
    uint8_t rx = '0';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    // HAL will set rx = '0', so mock out typing not necessary
    uint8_t ret = CommandHandler_PollCommand(&cmd_result);
    TEST_ASSERT_EQUAL_UINT8(1U, ret);
    TEST_ASSERT_EQUAL_UINT8(0U, cmd_result);
}

/*
CH_02: Accept valid command '5' (upper bound)
Objective: Function returns 1, *cmd_out = 5
Input: rx = '5', HAL_OK
*/
void test_CH_02_AcceptsValidCommand5(void)
{
    uint8_t cmd_result = 222; // any value, expect to be overwritten
    uint8_t rx = '5';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    uint8_t ret = CommandHandler_PollCommand(&cmd_result);
    TEST_ASSERT_EQUAL_UINT8(1U, ret);
    TEST_ASSERT_EQUAL_UINT8(5U, cmd_result);
}

/*
CH_03: Reject numeric out-of-range command (e.g. '8')
Objective: Function returns 0, *cmd_out unchanged
Input: rx = '8', HAL_OK
*/
void test_CH_03_RejectsNumericOutOfRangeCommand(void)
{
    uint8_t cmd_result = 44; // should remain unchanged
    uint8_t rx = '8';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    uint8_t before = cmd_result;
    uint8_t ret = CommandHandler_PollCommand(&cmd_result);
    TEST_ASSERT_EQUAL_UINT8(0U, ret);
    TEST_ASSERT_EQUAL_UINT8(before, cmd_result);
}

/*
CH_04: Reject non-numeric command (e.g. 'x')
Objective: Function returns 0, *cmd_out unchanged
Input: rx = 'x', HAL_OK
*/
void test_CH_04_RejectsNonNumericCommand(void)
{
    uint8_t cmd_result = 85; // should remain unchanged
    uint8_t rx = 'x';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    uint8_t before = cmd_result;
    uint8_t ret = CommandHandler_PollCommand(&cmd_result);
    TEST_ASSERT_EQUAL_UINT8(0U, ret);
    TEST_ASSERT_EQUAL_UINT8(before, cmd_result);
}

/*
CH_05: Reject UART receive failure
Objective: Function returns 0, *cmd_out unchanged
Input: HAL_ERROR
*/
void test_CH_05_RejectsUARTFailure(void)
{
    uint8_t cmd_result = 73; // should remain unchanged
    uint8_t rx_dummy = 0; // not used
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_dummy, 1, 10, HAL_ERROR);
    uint8_t before = cmd_result;
    uint8_t ret = CommandHandler_PollCommand(&cmd_result);
    TEST_ASSERT_EQUAL_UINT8(0U, ret);
    TEST_ASSERT_EQUAL_UINT8(before, cmd_result);
}

/*
CH-06: Handle NULL pointer safely
Objective: No crash, return 0 on NULL
*/
void test_CH_06_HandleNullPointer(void)
{
    /* This test just ensures the function returns 0 and does not crash */
    uint8_t ret = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL_UINT8(0U, ret);
}

/*
CH_07: Do not modify output on invalid data (e.g. '9')
Objective: *cmd_out remains unchanged, function returns 0
Input: rx = '9', HAL_OK
*/
void test_CH_07_DoesNotModifyOutputOnInvalidData(void)
{
    uint8_t cmd_result = 0xA4; // Should not change
    uint8_t rx = '9';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    uint8_t before = cmd_result;
    uint8_t ret = CommandHandler_PollCommand(&cmd_result);
    TEST_ASSERT_EQUAL_UINT8(0U, ret);
    TEST_ASSERT_EQUAL_UINT8(before, cmd_result);
}
