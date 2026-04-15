/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* CH_01: Accept valid command '0' */
void test_command_handler_CH_01(void)
{
    uint8_t cmd_out = 255;
    uint8_t rx = '0';
    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&rx);
    uint8_t result = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL(1U, result);
    TEST_ASSERT_EQUAL(0U, cmd_out);
}

/* CH_02: Accept valid command '5' (upper bound) */
void test_command_handler_CH_02(void)
{
    uint8_t cmd_out = 255;
    uint8_t rx = '5';
    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&rx);
    uint8_t result = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL(1U, result);
    TEST_ASSERT_EQUAL(5U, cmd_out);
}

/* CH_03: Reject numeric out-of-range command */
void test_command_handler_CH_03(void)
{
    uint8_t cmd_out = 99;
    uint8_t before = cmd_out;
    uint8_t rx = '8';
    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&rx);
    uint8_t result = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL(0U, result);
    TEST_ASSERT_EQUAL(before, cmd_out);
}

/* CH_04: Reject non-numeric command */
void test_command_handler_CH_04(void)
{
    uint8_t cmd_out = 88;
    uint8_t before = cmd_out;
    uint8_t rx = 'x';
    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&rx);
    uint8_t result = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL(0U, result);
    TEST_ASSERT_EQUAL(before, cmd_out);
}

/* CH_05: Reject UART receive failure */
void test_command_handler_CH_05(void)
{
    uint8_t cmd_out = 77;
    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, UNITY_ANY, 1, 10, HAL_ERROR);
    uint8_t result = CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL(0U, result);
}

/* CH-06: Handle NULL pointer safely */
void test_command_handler_CH_06(void)
{
    /* Should not crash */
    uint8_t result = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL(0U, result);
}

/* CH_07: Do not modify output on invalid data */
void test_command_handler_CH_07(void)
{
    uint8_t cmd_out = 55;
    uint8_t before = cmd_out;
    uint8_t rx = '9';
    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&rx);
    CommandHandler_PollCommand(&cmd_out);
    TEST_ASSERT_EQUAL(before, cmd_out);
}
