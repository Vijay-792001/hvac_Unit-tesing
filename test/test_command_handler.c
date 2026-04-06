/* ===== test_command_handler.c ===== */

#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"
#include <string.h>

/* External UART handle defined in the module */
UART_HandleTypeDef huart2;

void setUp(void)
{
    /* By default no special setup required */
}

void tearDown(void)
{
    /* By default no special teardown required */
}


/*
 * CH_01: Accept valid command '0'
 * Input: rx = '0', HAL_OK
 * Expects: Return = 1; *cmd_out = 0
 */
void test_CH_01_Accept_valid_command_0(void)
{
    uint8_t out = 0xAA; /* Any value different from '0' */
    uint8_t rx = '0';
    /* HAL_UART_Receive will be called with &huart2, pointer to rx, size=1, timeout=10 */
    HAL_UART_Receive_ExpectWithArrayAndReturn(&huart2, &rx, 1, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&out);

    TEST_ASSERT_EQUAL_INT(1, ret);
    TEST_ASSERT_EQUAL_UINT8(0, out);
}

/*
 * CH_02: Accept valid command '5' (upper bound)
 * Input: rx = '5', HAL_OK
 * Expects: Return = 1; *cmd_out = 5
 */
void test_CH_02_Accept_valid_command_5(void)
{
    uint8_t out = 0xAA; /* Dummy value not 5 */
    uint8_t rx = '5';
    HAL_UART_Receive_ExpectWithArrayAndReturn(&huart2, &rx, 1, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&out);

    TEST_ASSERT_EQUAL_INT(1, ret);
    TEST_ASSERT_EQUAL_UINT8(5, out);
}

/*
 * CH_03: Reject numeric out-of-range command
 * Input: rx = '8', HAL_OK
 * Expects: Return = 0, *cmd_out unchanged
 */
void test_CH_03_Reject_numeric_out_of_range_command(void)
{
    uint8_t out = 0x77; /* Should remain unchanged */
    uint8_t out_before = out;
    uint8_t rx = '8';

    HAL_UART_Receive_ExpectWithArrayAndReturn(&huart2, &rx, 1, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&out);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_UINT8(out_before, out);
}

/*
 * CH_04: Reject non-numeric command
 * Input: rx = 'x', HAL_OK
 * Expects: Return = 0, *cmd_out unchanged
 */
void test_CH_04_Reject_non_numeric_command(void)
{
    uint8_t out = 0x33; /* Should remain unchanged */
    uint8_t out_before = out;
    uint8_t rx = 'x';

    HAL_UART_Receive_ExpectWithArrayAndReturn(&huart2, &rx, 1, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&out);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_UINT8(out_before, out);
}

/*
 * CH_05: Reject UART receive failure
 * Input: HAL_ERROR
 * Expects: Return = 0, *cmd_out unchanged
 */
void test_CH_05_Reject_UART_receive_failure(void)
{
    uint8_t out = 0x23; /* Should remain unchanged */
    uint8_t out_before = out;
    uint8_t rx = 0x00;  /* Doesn't matter -- will not be updated (HAL_ERROR) */

    HAL_UART_Receive_ExpectWithArrayAndReturn(&huart2, &rx, 1, 1, 10, HAL_ERROR);

    int ret = CommandHandler_PollCommand(&out);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_UINT8(out_before, out);
}

/*
 * CH-06: Handle NULL pointer safely (should return 0, not call HAL_UART_Receive, not crash)
 */
void test_CH_06_Handle_null_pointer_safely(void)
{
    /* HAL_UART_Receive must NOT be called */
    int ret = CommandHandler_PollCommand(NULL);

    TEST_ASSERT_EQUAL_INT(0, ret);
}

/*
 * CH_07: Do not modify output on invalid data (e.g., rx = '9')
 * Input: rx = '9', HAL_OK
 * Expects: *cmd_out remains unchanged
 */
void test_CH_07_Do_not_modify_output_on_invalid_data_rx9(void)
{
    uint8_t out = 0x44;
    uint8_t out_before = out;
    uint8_t rx = '9';

    HAL_UART_Receive_ExpectWithArrayAndReturn(&huart2, &rx, 1, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&out);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_UINT8(out_before, out);
}
