/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

UART_HandleTypeDef huart2;

void setUp(void) {}
void tearDown(void) {}

void test_command_handler_CH_01(void) { uint8_t result; uint8_t cmd = 0xAA; uint8_t rx = '0'; HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK); result = CommandHandler_PollCommand(&cmd); TEST_ASSERT_EQUAL_UINT8(1U, result); TEST_ASSERT_EQUAL_UINT8(0U, cmd); }
void test_command_handler_CH_02(void) { uint8_t result; uint8_t cmd = 0xAA; uint8_t rx = '5'; HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK); result = CommandHandler_PollCommand(&cmd); TEST_ASSERT_EQUAL_UINT8(1U, result); TEST_ASSERT_EQUAL_UINT8(5U, cmd); }
void test_command_handler_CH_03(void) { uint8_t result; uint8_t cmd = 0xAB; uint8_t rx = '8'; HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK); result = CommandHandler_PollCommand(&cmd); TEST_ASSERT_EQUAL_UINT8(0U, result); TEST_ASSERT_EQUAL_UINT8(0xAB, cmd); }
void test_command_handler_CH_04(void) { uint8_t result; uint8_t cmd = 0xAB; uint8_t rx = 'x'; HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK); result = CommandHandler_PollCommand(&cmd); TEST_ASSERT_EQUAL_UINT8(0U, result); TEST_ASSERT_EQUAL_UINT8(0xAB, cmd); }
void test_command_handler_CH_05(void) { uint8_t result; uint8_t cmd = 0xFE; HAL_UART_Receive_ExpectAndReturn(&huart2, &cmd, 1, 10, HAL_ERROR); result = CommandHandler_PollCommand(&cmd); TEST_ASSERT_EQUAL_UINT8(0U, result); TEST_ASSERT_EQUAL_UINT8(0xFE, cmd); }
void test_command_handler_CH_06(void) { uint8_t result; result = CommandHandler_PollCommand(NULL); TEST_ASSERT_EQUAL_UINT8(0U, result); }
void test_command_handler_CH_07(void) { uint8_t result; uint8_t cmd = 0xBC; uint8_t rx = '9'; HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK); result = CommandHandler_PollCommand(&cmd); TEST_ASSERT_EQUAL_UINT8(0U, result); TEST_ASSERT_EQUAL_UINT8(0xBC, cmd); }
