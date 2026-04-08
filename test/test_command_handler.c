/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* CH_01: Accept valid command '0': rx = '0', HAL_OK, expect return 1, *cmd_out = 0 */
void test_command_handler_CH_01(void)
{
    uint8_t cmd = 0xAA; /* deliberate non-zero initial value */
    uint8_t rx = '0';
    extern UART_HandleTypeDef huart2;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(0, cmd);
}

/* CH_02: Accept valid command '5' (upper bound): rx = '5', HAL_OK, expect return 1, *cmd_out = 5 */
void test_command_handler_CH_02(void)
{
    uint8_t cmd = 0xA5;
    uint8_t rx = '5';
    extern UART_HandleTypeDef huart2;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(5, cmd);
}

/* CH_03: Reject numeric out-of-range command: rx = '8', HAL_OK, expect return 0, *cmd_out unchanged */
void test_command_handler_CH_03(void)
{
    uint8_t initial_cmd = 0xC5;
    uint8_t cmd = initial_cmd;
    uint8_t rx = '8';
    extern UART_HandleTypeDef huart2;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(initial_cmd, cmd);
}

/* CH_04: Reject non-numeric command: rx = 'x', HAL_OK, expect return 0, *cmd_out unchanged */
void test_command_handler_CH_04(void)
{
    uint8_t initial_cmd = 0xC6;
    uint8_t cmd = initial_cmd;
    uint8_t rx = 'x';
    extern UART_HandleTypeDef huart2;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(initial_cmd, cmd);
}

/* CH_05: Reject UART receive failure: HAL_ERROR, expect return 0, *cmd_out unchanged */
void test_command_handler_CH_05(void)
{
    uint8_t initial_cmd = 0xFE;
    uint8_t cmd = initial_cmd;
    uint8_t rx = 0x00; /* don't care as receive will fail */
    extern UART_HandleTypeDef huart2;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_ERROR);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(initial_cmd, cmd);
}

/* CH-06: Handle NULL pointer safely: cmd_out = NULL, expect return 0 (no crash) */
void test_command_handler_CH_06(void)
{
    int ret = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL_UINT8(0, ret);
    /* No way to assert crash; test passes if function returns, as per plan */
}

/* CH_07: Do not modify output on invalid data: rx = '9', HAL_OK, *cmd_out unchanged */
void test_command_handler_CH_07(void)
{
    uint8_t initial_cmd = 0xB2;
    uint8_t cmd = initial_cmd;
    uint8_t rx = '9';
    extern UART_HandleTypeDef huart2;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(initial_cmd, cmd);
}

