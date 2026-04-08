/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

UART_HandleTypeDef huart2;

void setUp(void) {}
void tearDown(void) {}

/* CH_01: Accept valid command '0' */
void test_command_handler_CH_01(void)
{
    uint8_t cmd = 0xAA;
    uint8_t rx = '0';
    /* Expect UART receive to succeed */
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    int result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_UINT8(0, cmd);
}

/* CH_02: Accept valid command '5' (upper bound) */
void test_command_handler_CH_02(void)
{
    uint8_t cmd = 0xAA;
    uint8_t rx = '5';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    int result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_UINT8(5, cmd);
}

/* CH_03: Reject numeric out-of-range command */
void test_command_handler_CH_03(void)
{
    uint8_t cmd = 0xAB;
    uint8_t rx = '8';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    int result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_UINT8(0xAB, cmd); // Value must remain unchanged
}

/* CH_04: Reject non-numeric command */
void test_command_handler_CH_04(void)
{
    uint8_t cmd = 0xAB;
    uint8_t rx = 'x';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    int result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_UINT8(0xAB, cmd);
}

/* CH_05: Reject UART receive failure */
void test_command_handler_CH_05(void)
{
    uint8_t cmd = 0xEE;
    /* Do not care about rx content as receive fails */
    HAL_UART_Receive_ExpectAndReturn(&huart2, (uint8_t*)UNITY_ANY, 1, 10, HAL_ERROR);
    int result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_UINT8(0xEE, cmd);
}

/* CH-06: Handle NULL pointer safely */
void test_command_handler_CH_06(void)
{
    int result = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL_INT(0, result);
}

/* CH_07: Do not modify output on invalid data (e.g. rx = '9') */
void test_command_handler_CH_07(void)
{
    uint8_t cmd = 0xAC;
    uint8_t rx = '9';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    int result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_UINT8(0xAC, cmd);
}
