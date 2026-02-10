#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void test_CommandHandler_ValidCommand_ShouldCallUartReceiveAndProcess(void)
{
    uint8_t rxData = 0x42;
    HAL_UART_Receive_ExpectAndReturn(NULL, &rxData, 1, 100, HAL_OK);
    int result = CommandHandler_ProcessNext();
    TEST_ASSERT_EQUAL(0, result);
}

void test_CommandHandler_NullCommand_ShouldReturnError(void)
{
    HAL_UART_Receive_ExpectAndReturn(NULL, NULL, 1, 100, HAL_ERROR);
    int result = CommandHandler_ProcessNext();
    TEST_ASSERT_EQUAL(-1, result);
}

void test_CommandHandler_UartTimeout_ShouldReturnError(void)
{
    HAL_UART_Receive_ExpectAndReturn(NULL, NULL, 1, 100, HAL_TIMEOUT);
    int result = CommandHandler_ProcessNext();
    TEST_ASSERT_EQUAL(-2, result);
}

void test_CommandHandler_UnknownCommand_ShouldIgnore(void)
{
    uint8_t rxData = 0xFE;
    HAL_UART_Receive_ExpectAndReturn(NULL, &rxData, 1, 100, HAL_OK);
    int result = CommandHandler_ProcessNext();
    TEST_ASSERT_EQUAL(1, result);
}

void test_CommandHandler_NullUartHandle_ShouldReturnError(void)
{
    HAL_UART_Receive_ExpectAndReturn(NULL, NULL, 1, 100, HAL_ERROR);
    int result = CommandHandler_ProcessNext();
    TEST_ASSERT_EQUAL(-1, result);
}
