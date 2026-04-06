/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

UART_HandleTypeDef huart2;

/* Required by Ceedling */
void setUp(void) {}
void tearDown(void) {}

/* CH_01: Accept valid command '0' */
void test_CH_01_Accept_valid_command_0(void)
{
    uint8_t cmd;
    uint8_t rx = '0';

    /* Mock HAL_UART_Receive: Returns HAL_OK, sets rx = '0' */
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    uint8_t ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1U, ret);
    TEST_ASSERT_EQUAL_UINT8(0U, cmd);
}

/* CH_02: Accept valid command '5' (upper bound) */
void test_CH_02_Accept_valid_command_5_upper_bound(void)
{
    uint8_t cmd;
    uint8_t rx = '5';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    uint8_t ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1U, ret);
    TEST_ASSERT_EQUAL_UINT8(5U, cmd);
}

/* CH_03: Reject numeric out-of-range command ('8') */
void test_CH_03_Reject_numeric_out_of_range_command(void)
{
    uint8_t cmd = 0xAA; /* initial value */
    uint8_t cmd_before = cmd;
    uint8_t rx = '8';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    uint8_t ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0U, ret);
    TEST_ASSERT_EQUAL_UINT8(cmd_before, cmd);
}

/* CH_04: Reject non-numeric command ('x') */
void test_CH_04_Reject_non_numeric_command(void)
{
    uint8_t cmd = 0x11;
    uint8_t cmd_before = cmd;
    uint8_t rx = 'x';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    uint8_t ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0U, ret);
    TEST_ASSERT_EQUAL_UINT8(cmd_before, cmd);
}

/* CH_05: Reject UART receive failure */
void test_CH_05_Reject_UART_receive_failure(void)
{
    uint8_t cmd = 0xDE;
    uint8_t cmd_before = cmd;

    HAL_UART_Receive_ExpectAnyArgsAndReturn(HAL_ERROR);

    uint8_t ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0U, ret);
    TEST_ASSERT_EQUAL_UINT8(cmd_before, cmd);
}

/* CH_06: Handle NULL pointer safely */
void test_CH_06_Handle_NULL_pointer_safely(void)
{
    /* Should return 0 and not call HAL_UART_Receive */
    uint8_t ret = CommandHandler_PollCommand(NULL);

    TEST_ASSERT_EQUAL_UINT8(0U, ret);
    /* No assert for output; test passes if there is no crash. */
}

/* CH_07: Do not modify output on invalid data ('9') */
void test_CH_07_No_modify_output_on_invalid_data(void)
{
    uint8_t cmd = 0x69;
    uint8_t cmd_before = cmd;
    uint8_t rx = '9';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    uint8_t ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0U, ret);
    TEST_ASSERT_EQUAL_UINT8(cmd_before, cmd); /* Output should not be modified */
}