/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_command_handler_CH_01(void)
{
    UART_HandleTypeDef dummy_uart;
    extern UART_HandleTypeDef huart2;
    huart2 = dummy_uart;
    uint8_t output = 99;
    uint8_t expected = 0;
    uint8_t rx = '0';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&output);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL_UINT8(0, output);
}

void test_command_handler_CH_02(void)
{
    UART_HandleTypeDef dummy_uart;
    extern UART_HandleTypeDef huart2;
    huart2 = dummy_uart;
    uint8_t output = 99;
    uint8_t expected = 5;
    uint8_t rx = '5';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&output);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL_UINT8(5, output);
}

void test_command_handler_CH_03(void)
{
    UART_HandleTypeDef dummy_uart;
    extern UART_HandleTypeDef huart2;
    huart2 = dummy_uart;
    uint8_t output = 42;
    uint8_t rx = '8';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&output);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT8(42, output);
}

void test_command_handler_CH_04(void)
{
    UART_HandleTypeDef dummy_uart;
    extern UART_HandleTypeDef huart2;
    huart2 = dummy_uart;
    uint8_t output = 7;
    uint8_t rx = 'x';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&output);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT8(7, output);
}

void test_command_handler_CH_05(void)
{
    UART_HandleTypeDef dummy_uart;
    extern UART_HandleTypeDef huart2;
    huart2 = dummy_uart;
    uint8_t output = 9;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &output, 1, 10, HAL_ERROR);

    int ret = CommandHandler_PollCommand(&output);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT8(9, output);
}

void test_command_handler_CH_06(void)
{
    // CH-06: cmd_out = NULL, ensure no crash
    int ret = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL(0, ret);
}

void test_command_handler_CH_07(void)
{
    UART_HandleTypeDef dummy_uart;
    extern UART_HandleTypeDef huart2;
    huart2 = dummy_uart;
    uint8_t output = 77;
    uint8_t rx = '9';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&output);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT8(77, output);
}
