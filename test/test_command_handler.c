/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* Test Case: CH_01 - Accept valid command '0' */
void test_Command_Handler_CH_01(void)
{
    uint8_t cmd = 0xFF;
    uint8_t rx = '0';
    /* Expect the UART receive to return '0' and HAL_OK */
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    int result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL(1, result);
    TEST_ASSERT_EQUAL_UINT8(0, cmd);
}

/* Test Case: CH_02 - Accept valid command '5' (upper bound) */
void test_Command_Handler_CH_02(void)
{
    uint8_t cmd = 0xFF;
    uint8_t rx = '5';
    /* Expect the UART receive to return '5' and HAL_OK */
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    int result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL(1, result);
    TEST_ASSERT_EQUAL_UINT8(5, cmd);
}

/* Test Case: CH_03 - Reject numeric out-of-range command */
void test_Command_Handler_CH_03(void)
{
    uint8_t cmd = 0xA2;
    uint8_t rx = '8';
    /* Expect the UART receive to return '8' and HAL_OK */
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    int result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_UINT8(0xA2, cmd);
}

/* Test Case: CH_04 - Reject non-numeric command */
void test_Command_Handler_CH_04(void)
{
    uint8_t cmd = 0x5A;
    uint8_t rx = 'x';
    /* Expect the UART receive to return 'x' and HAL_OK */
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    int result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_UINT8(0x5A, cmd);
}

/* Test Case: CH_05 - Reject UART receive failure */
void test_Command_Handler_CH_05(void)
{
    uint8_t cmd = 0x11;
    uint8_t rx = '2'; /* Doesn't matter which value, receive fails */
    /* Expect the UART receive to return HAL_ERROR */
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_ERROR);
    int result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_UINT8(0x11, cmd);
}

/* Test Case: CH-06 - Handle NULL pointer safely */
void test_Command_Handler_CH_06(void)
{
    /* Should not crash: just returns 0 on NULL pointer */
    int result = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL(0, result);
}

/* Test Case: CH_07 - Do not modify output on invalid data */
void test_Command_Handler_CH_07(void)
{
    uint8_t cmd = 0xBC;
    uint8_t rx = '9';
    /* Expect the UART receive to return '9' and HAL_OK */
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    (void)CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8(0xBC, cmd);
}
