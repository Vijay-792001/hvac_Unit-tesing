/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_command_handler_CH_01(void)
{
    // Accept valid command '0': rx = '0', HAL_OK -> return 1, *cmd_out = 0
    uint8_t cmd = 0xAA;
    uint8_t expected = 0;

    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, (uint8_t*)&cmd, 1, 10, HAL_OK);
    HAL_UART_Receive_IgnoreArg_pData();
    HAL_UART_Receive_ReturnArrayThruPtr_pData((uint8_t[]){'0'}, 1);

    uint8_t cmd_out = 0xAA;
    int ret = CommandHandler_PollCommand(&cmd_out);

    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL_UINT8(expected, cmd_out);
}

void test_command_handler_CH_02(void)
{
    // Accept valid command '5' (upper bound): rx = '5', HAL_OK -> return 1, *cmd_out = 5
    uint8_t dummy = 0;
    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, (uint8_t*)&dummy, 1, 10, HAL_OK);
    HAL_UART_Receive_IgnoreArg_pData();
    HAL_UART_Receive_ReturnArrayThruPtr_pData((uint8_t[]){'5'}, 1);

    uint8_t cmd_out = 0x00;
    int ret = CommandHandler_PollCommand(&cmd_out);

    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL_UINT8(5, cmd_out);
}

void test_command_handler_CH_03(void)
{
    // Reject numeric out-of-range command: rx = '8', HAL_OK -> return 0, *cmd_out unchanged
    uint8_t dummy = 0;
    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, (uint8_t*)&dummy, 1, 10, HAL_OK);
    HAL_UART_Receive_IgnoreArg_pData();
    HAL_UART_Receive_ReturnArrayThruPtr_pData((uint8_t[]){'8'}, 1);

    uint8_t cmd_out = 0xAB;
    int ret = CommandHandler_PollCommand(&cmd_out);

    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0xAB, cmd_out);
}

void test_command_handler_CH_04(void)
{
    // Reject non-numeric command: rx = 'x', HAL_OK -> return 0, *cmd_out unchanged
    uint8_t dummy = 0;
    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, (uint8_t*)&dummy, 1, 10, HAL_OK);
    HAL_UART_Receive_IgnoreArg_pData();
    HAL_UART_Receive_ReturnArrayThruPtr_pData((uint8_t[]){'x'}, 1);

    uint8_t cmd_out = 0xBB;
    int ret = CommandHandler_PollCommand(&cmd_out);

    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0xBB, cmd_out);
}

void test_command_handler_CH_05(void)
{
    // Reject UART receive failure: HAL_ERROR -> return 0, *cmd_out unchanged
    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, NULL, 1, 10, HAL_ERROR);
    HAL_UART_Receive_IgnoreArg_pData();

    uint8_t cmd_out = 0xCC;
    int ret = CommandHandler_PollCommand(&cmd_out);

    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0xCC, cmd_out);
}

void test_command_handler_CH_06(void)
{
    // Handle NULL pointer safely: cmd_out = NULL, should return 0, no crash
    int ret = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL(0, ret);
    // No assertion for crash; test passes if no crash occurs
}

void test_command_handler_CH_07(void)
{
    // Do not modify output on invalid data: rx='9', HAL_OK -> *cmd_out unchanged
    uint8_t dummy = 0;
    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_ExpectAndReturn(&huart2, (uint8_t*)&dummy, 1, 10, HAL_OK);
    HAL_UART_Receive_IgnoreArg_pData();
    HAL_UART_Receive_ReturnArrayThruPtr_pData((uint8_t[]){'9'}, 1);

    uint8_t cmd_out = 0xDD;
    int ret = CommandHandler_PollCommand(&cmd_out);

    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0xDD, cmd_out);
}
