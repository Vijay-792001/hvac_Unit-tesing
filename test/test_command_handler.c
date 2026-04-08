/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* CH_01: Accept valid command '0' */
void test_command_handler_CH_01(void)
{
    UART_HandleTypeDef dummy_huart2;
    extern UART_HandleTypeDef huart2;
    huart2 = dummy_huart2;
    uint8_t cmd = 0xFF;
    uint8_t rx = '0';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1U, result);
    TEST_ASSERT_EQUAL_UINT8(0U, cmd);
}

/* CH_02: Accept valid command '5' (upper bound) */
void test_command_handler_CH_02(void)
{
    UART_HandleTypeDef dummy_huart2;
    extern UART_HandleTypeDef huart2;
    huart2 = dummy_huart2;
    uint8_t cmd = 0xFF;
    uint8_t rx = '5';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1U, result);
    TEST_ASSERT_EQUAL_UINT8(5U, cmd);
}

/* CH_03: Reject numeric out-of-range command */
void test_command_handler_CH_03(void)
{
    UART_HandleTypeDef dummy_huart2;
    extern UART_HandleTypeDef huart2;
    huart2 = dummy_huart2;
    uint8_t cmd_before = 0xAA;
    uint8_t cmd = cmd_before;
    uint8_t rx = '8';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0U, result);
    TEST_ASSERT_EQUAL_UINT8(cmd_before, cmd);
}

/* CH_04: Reject non-numeric command */
void test_command_handler_CH_04(void)
{
    UART_HandleTypeDef dummy_huart2;
    extern UART_HandleTypeDef huart2;
    huart2 = dummy_huart2;
    uint8_t cmd_before = 0x55;
    uint8_t cmd = cmd_before;
    uint8_t rx = 'x';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0U, result);
    TEST_ASSERT_EQUAL_UINT8(cmd_before, cmd);
}

/* CH_05: Reject UART receive failure */
void test_command_handler_CH_05(void)
{
    UART_HandleTypeDef dummy_huart2;
    extern UART_HandleTypeDef huart2;
    huart2 = dummy_huart2;
    uint8_t cmd_before = 0xEF;
    uint8_t cmd = cmd_before;
    uint8_t rx = '0';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_ERROR);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0U, result);
    TEST_ASSERT_EQUAL_UINT8(cmd_before, cmd);
}

/* CH-06: Handle NULL pointer safely */
void test_command_handler_CH_06(void)
{
    uint8_t result = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL_UINT8(0U, result);
}

/* CH_07: Do not modify output on invalid data */
void test_command_handler_CH_07(void)
{
    UART_HandleTypeDef dummy_huart2;
    extern UART_HandleTypeDef huart2;
    huart2 = dummy_huart2;
    uint8_t cmd_before = 0x7C;
    uint8_t cmd = cmd_before;
    uint8_t rx = '9';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0U, result);
    TEST_ASSERT_EQUAL_UINT8(cmd_before, cmd);
}
