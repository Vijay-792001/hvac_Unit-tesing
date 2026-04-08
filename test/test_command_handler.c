/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* Test Case: CH_01 */
void test_command_handler_CH_01(void)
{
    uint8_t cmd = 99;
    UART_HandleTypeDef huart2_dummy;
    extern UART_HandleTypeDef huart2;
    huart2 = huart2_dummy;

    /* Expectation: HAL_UART_Receive returns HAL_OK, rx = '0' */
    uint8_t rx_value = '0';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_value, 1, 10, HAL_OK);

    uint8_t result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_EQUAL_UINT8(0, cmd);
}

/* Test Case: CH_02 */
void test_command_handler_CH_02(void)
{
    uint8_t cmd = 99;
    UART_HandleTypeDef huart2_dummy;
    extern UART_HandleTypeDef huart2;
    huart2 = huart2_dummy;

    /* Expectation: HAL_UART_Receive returns HAL_OK, rx = '5' */
    uint8_t rx_value = '5';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_value, 1, 10, HAL_OK);

    uint8_t result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_EQUAL_UINT8(5, cmd);
}

/* Test Case: CH_03 */
void test_command_handler_CH_03(void)
{
    uint8_t cmd = 55;
    UART_HandleTypeDef huart2_dummy;
    extern UART_HandleTypeDef huart2;
    huart2 = huart2_dummy;

    /* Expectation: HAL_UART_Receive returns HAL_OK, rx = '8' (out of range) */
    uint8_t rx_value = '8';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_value, 1, 10, HAL_OK);

    uint8_t result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(55, cmd); // unchanged
}

/* Test Case: CH_04 */
void test_command_handler_CH_04(void)
{
    uint8_t cmd = 44;
    UART_HandleTypeDef huart2_dummy;
    extern UART_HandleTypeDef huart2;
    huart2 = huart2_dummy;

    /* Expectation: HAL_UART_Receive returns HAL_OK, rx = non-numeric 'x' */
    uint8_t rx_value = 'x';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_value, 1, 10, HAL_OK);

    uint8_t result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(44, cmd); // unchanged
}

/* Test Case: CH_05 */
void test_command_handler_CH_05(void)
{
    uint8_t cmd = 11;
    UART_HandleTypeDef huart2_dummy;
    extern UART_HandleTypeDef huart2;
    huart2 = huart2_dummy;

    /* Expectation: HAL_UART_Receive returns HAL_ERROR */
    uint8_t rx_value = 0x00;
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_value, 1, 10, HAL_ERROR);

    uint8_t result = CommandHandler_PollCommand(&cmd);
    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(11, cmd); // unchanged
}

/* Test Case: CH-06 */
void test_command_handler_CH_06(void)
{
    /* NULL pointer test for cmd_out */
    UART_HandleTypeDef huart2_dummy;
    extern UART_HandleTypeDef huart2;
    huart2 = huart2_dummy;

    uint8_t result = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL_UINT8(0, result);
}

/* Test Case: CH_07 */
void test_command_handler_CH_07(void)
{
    uint8_t cmd = 99;
    UART_HandleTypeDef huart2_dummy;
    extern UART_HandleTypeDef huart2;
    huart2 = huart2_dummy;

    /* Expectation: HAL_UART_Receive returns HAL_OK, rx = '9' (invalid/out-of-range) */
    uint8_t rx_value = '9';
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_value, 1, 10, HAL_OK);

    uint8_t previous_cmd = cmd;
    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(previous_cmd, cmd); // unchanged
}
