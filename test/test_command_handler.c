/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* CH_01: Accept valid command '0' */
void test_command_handler_CH_01(void)
{
    uint8_t cmd = 0xAA;
    uint8_t rx = '0';
    extern UART_HandleTypeDef huart2;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    uint8_t ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(0, cmd);
}

/* CH_02: Accept valid command '5' (upper bound) */
void test_command_handler_CH_02(void)
{
    uint8_t cmd = 0xAA;
    uint8_t rx = '5';
    extern UART_HandleTypeDef huart2;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    uint8_t ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(5, cmd);
}

/* CH_03: Reject numeric out-of-range command */
void test_command_handler_CH_03(void)
{
    uint8_t cmd = 0xAB;
    uint8_t prev_cmd = cmd;
    uint8_t rx = '8';
    extern UART_HandleTypeDef huart2;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    uint8_t ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(prev_cmd, cmd);
}

/* CH_04: Reject non-numeric command */
void test_command_handler_CH_04(void)
{
    uint8_t cmd = 0xAB;
    uint8_t prev_cmd = cmd;
    uint8_t rx = 'x';
    extern UART_HandleTypeDef huart2;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    uint8_t ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(prev_cmd, cmd);
}

/* CH_05: Reject UART receive failure */
void test_command_handler_CH_05(void)
{
    uint8_t cmd = 0xAB;
    extern UART_HandleTypeDef huart2;

    /* HAL_UART_Receive returns HAL_ERROR, data pointer value does not matter */
    HAL_UART_Receive_ExpectAndReturn(&huart2, (uint8_t*)UNITY_ANY, 1, 10, HAL_ERROR);

    uint8_t prev_cmd = cmd;
    uint8_t ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(prev_cmd, cmd);
}

/* CH-06: Handle NULL pointer safely */
void test_command_handler_CH_06(void)
{
    /* Should not crash, should return 0 */
    uint8_t ret = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL_UINT8(0, ret);
}

/* CH_07: Do not modify output on invalid data */
void test_command_handler_CH_07(void)
{
    uint8_t cmd = 0xAC;
    uint8_t prev_cmd = cmd;
    uint8_t rx = '9';
    extern UART_HandleTypeDef huart2;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    uint8_t ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(prev_cmd, cmd);
}

