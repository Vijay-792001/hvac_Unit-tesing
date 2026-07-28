#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

UART_HandleTypeDef huart2;

void setUp(void) {}
void tearDown(void) {}

void test_command_handler_CH_01(void)
{
    uint8_t cmd_out = CMD_INVALID;
    uint8_t rx = '0';
    uint8_t result;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    result = CommandHandler_PollCommand(&cmd_out);

    TEST_ASSERT_EQUAL_UINT8(1U, result);
    TEST_ASSERT_EQUAL_UINT8(0U, cmd_out);
}
