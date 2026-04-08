/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_command_handler_CH_01(void)
{
    uint8_t cmd = 0xFF;
    uint8_t rx = '0';
    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_EQUAL_UINT8(0, cmd); // '0' - '0' = 0
}

void test_command_handler_CH_02(void)
{
    uint8_t cmd = 0xFF;
    uint8_t rx = '5';
    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_EQUAL_UINT8(5, cmd); // '5' - '0' = 5
}

void test_command_handler_CH_03(void)
{
    uint8_t cmd = 42; // unchanged value
    uint8_t rx = '8';
    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(42, cmd); // unchanged
}

void test_command_handler_CH_04(void)
{
    uint8_t cmd = 55; // unchanged value
    uint8_t rx = 'x';
    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(55, cmd); // unchanged
}

void test_command_handler_CH_05(void)
{
    uint8_t cmd = 99; // unchanged value
    uint8_t rx = 0x00;
    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_ERROR);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(99, cmd); // unchanged
}

void test_command_handler_CH_06(void)
{
    uint8_t result = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL_UINT8(0, result);
}

void test_command_handler_CH_07(void)
{
    uint8_t cmd = 7; // initial value
    uint8_t rx = '9';
    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    (void)CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(7, cmd); // unchanged
}
