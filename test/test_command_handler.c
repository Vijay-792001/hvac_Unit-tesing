/* test_command_handler.c */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

UART_HandleTypeDef huart2;

void setUp(void)
{
    mock_stm32f4xx_hal_Init();
}

void tearDown(void)
{
    mock_stm32f4xx_hal_Verify();
    mock_stm32f4xx_hal_Destroy();
}

void test_CommandHandler_PollCommand_Returns0_WhenCmdOutIsNULL_CH06(void)
{
    uint8_t result;
    result = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, result, "Should return 0 when cmd_out is NULL (CH-06)");
}

void test_CommandHandler_PollCommand_NoDataReceived_Returns0_CH01_CH02(void)
{
    uint8_t output;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &output, 1, 10, HAL_ERROR);

    uint8_t ret = CommandHandler_PollCommand(&output);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
}

void test_CommandHandler_PollCommand_InvalidCharacter_Returns0_CH03(void)
{
    uint8_t output = 0xAA;
    uint8_t temp_rx = (uint8_t)'x';

    HAL_UART_Receive_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&temp_rx,1);

    uint8_t ret = CommandHandler_PollCommand(&output);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
}

void test_CommandHandler_PollCommand_ReceivesZero_Returns1_SetsCmdOutTo0_CH04(void)
{
    uint8_t output = 0xAA;
    uint8_t temp_rx = (uint8_t)'0';

    HAL_UART_Receive_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&temp_rx,1);

    uint8_t ret = CommandHandler_PollCommand(&output);

    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(0, output);
}

void test_CommandHandler_PollCommand_ReceivesFive_Returns1_SetsCmdOutTo5_CH05(void)
{
    uint8_t output = 0xAA;
    uint8_t temp_rx = (uint8_t)'5';

    HAL_UART_Receive_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&temp_rx,1);

    uint8_t ret = CommandHandler_PollCommand(&output);

    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(5, output);
}

void test_CommandHandler_PollCommand_ReceivesThree_Returns1_SetsCmdOutTo3_CH07(void)
{
    uint8_t output = 0xFF;
    uint8_t temp_rx = (uint8_t)'3';

    HAL_UART_Receive_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&temp_rx,1);

    uint8_t ret = CommandHandler_PollCommand(&output);

    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(3, output);
}
