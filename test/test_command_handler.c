/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_command_handler_CH_01(void)
{
    uint8_t cmd = 0xEE;
    uint8_t expected_rx = '0';
    HAL_UART_Receive_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&expected_rx);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL_UINT8(0, cmd);
}

void test_command_handler_CH_02(void)
{
    uint8_t cmd = 0xEE;
    uint8_t expected_rx = '5';
    HAL_UART_Receive_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&expected_rx);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL_UINT8(5, cmd);
}

void test_command_handler_CH_03(void)
{
    uint8_t cmd = 0xAB;
    uint8_t expected_rx = '8';
    HAL_UART_Receive_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&expected_rx);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0xAB, cmd);
}

void test_command_handler_CH_04(void)
{
    uint8_t cmd = 0x55;
    uint8_t expected_rx = 'x';
    HAL_UART_Receive_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&expected_rx);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0x55, cmd);
}

void test_command_handler_CH_05(void)
{
    uint8_t cmd = 0x12;
    HAL_UART_Receive_ExpectAnyArgsAndReturn(HAL_ERROR);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0x12, cmd);
}

void test_command_handler_CH_06(void)
{
    CommandHandler_PollCommand(NULL);

    TEST_ASSERT_TRUE(1);
}

void test_command_handler_CH_07(void)
{
    uint8_t cmd = 0xCD;
    uint8_t expected_rx = '9';
    HAL_UART_Receive_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_UART_Receive_ReturnThruPtr_pData(&expected_rx);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0xCD, cmd);
}
