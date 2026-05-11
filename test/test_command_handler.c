#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_command_handler_CH_01(void) { uint8_t cmd = 123; uint8_t rx = '0'; extern UART_HandleTypeDef huart2;

HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

uint8_t result = CommandHandler_PollCommand(&cmd);

TEST_ASSERT_EQUAL_UINT8(1U, result);
TEST_ASSERT_EQUAL_UINT8(0U, cmd);
}

void test_command_handler_CH_02(void) { uint8_t cmd = 123; uint8_t rx = '5'; extern UART_HandleTypeDef huart2;

HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

uint8_t result = CommandHandler_PollCommand(&cmd);

TEST_ASSERT_EQUAL_UINT8(1U, result);
TEST_ASSERT_EQUAL_UINT8(5U, cmd);
}

void test_command_handler_CH_03(void) { uint8_t cmd = 99; uint8_t rx = '8'; extern UART_HandleTypeDef huart2;

HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

uint8_t old_cmd = cmd;
uint8_t result = CommandHandler_PollCommand(&cmd);

TEST_ASSERT_EQUAL_UINT8(0U, result);
TEST_ASSERT_EQUAL_UINT8(old_cmd, cmd);
}

void test_command_handler_CH_04(void) { uint8_t cmd = 42; uint8_t rx = 'x'; extern UART_HandleTypeDef huart2;

HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

uint8_t old_cmd = cmd;
uint8_t result = CommandHandler_PollCommand(&cmd);

TEST_ASSERT_EQUAL_UINT8(0U, result);
TEST_ASSERT_EQUAL_UINT8(old_cmd, cmd);
}

void test_command_handler_CH_05(void) { uint8_t cmd = 11; uint8_t rx = 0; extern UART_HandleTypeDef huart2;

HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_ERROR);

uint8_t old_cmd = cmd;
uint8_t result = CommandHandler_PollCommand(&cmd);

TEST_ASSERT_EQUAL_UINT8(0U, result);
TEST_ASSERT_EQUAL_UINT8(old_cmd, cmd);
}

void test_command_handler_CH_06(void) { uint8_t result = CommandHandler_PollCommand(NULL);

TEST_ASSERT_EQUAL_UINT8(0U, result);
}

void test_command_handler_CH_07(void) { uint8_t cmd = 55; uint8_t rx = '9'; extern UART_HandleTypeDef huart2;

HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

uint8_t old_cmd = cmd;
CommandHandler_PollCommand(&cmd);

TEST_ASSERT_EQUAL_UINT8(old_cmd, cmd);
}