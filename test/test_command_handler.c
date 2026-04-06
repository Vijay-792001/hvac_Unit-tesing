/* ===== test_command_handler.c ===== */

#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

UART_HandleTypeDef huart2;

/* Helper macro: set up a dummy variable with recognizable value */
#define INIT_CMD_OUT()    uint8_t cmd = 0xA5; uint8_t old_cmd = cmd

void setUp(void)
{
    // Reset/clear any persistent resources before each test
}

void tearDown(void)
{
    // Clean up after each test
}

/* Test Case CH_01: Accept valid command '0' 
   Input: rx = '0', HAL_UART_Receive returns HAL_OK
   Expect: Function returns 1, *cmd_out = 0
*/
void test_CH_01_Accept_valid_command_0(void)
{
    INIT_CMD_OUT();

    uint8_t rx = '0';
    // HAL_UART_Receive expects to be called and to return HAL_OK,
    // and should put '0' in pData.
    HAL_UART_Receive_ExpectAndReturn(&huart2, NULL, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_UINT8(0, cmd);
}

/* Test Case CH_02: Accept valid command '5' (upper bound)
   Input: rx = '5', HAL_UART_Receive returns HAL_OK
   Expect: Function returns 1, *cmd_out = 5
*/
void test_CH_02_Accept_valid_command_5(void)
{
    INIT_CMD_OUT();

    uint8_t rx = '5';
    HAL_UART_Receive_ExpectAndReturn(&huart2, NULL, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_UINT8(5, cmd);
}

/* Test Case CH_03: Reject numeric out-of-range command
   Input: rx = '8', HAL_UART_Receive returns HAL_OK
   Expect: Function returns 0, *cmd_out is unchanged
*/
void test_CH_03_Reject_out_of_range_command(void)
{
    INIT_CMD_OUT();

    uint8_t rx = '8';
    HAL_UART_Receive_ExpectAndReturn(&huart2, NULL, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_INT(0, result);
    // cmd should not be changed
    TEST_ASSERT_EQUAL_UINT8(old_cmd, cmd);
}

/* Test Case CH_04: Reject non-numeric command
   Input: rx = 'x', HAL_UART_Receive returns HAL_OK
   Expect: Function returns 0, *cmd_out is unchanged
*/
void test_CH_04_Reject_non_numeric_command(void)
{
    INIT_CMD_OUT();

    uint8_t rx = 'x';
    HAL_UART_Receive_ExpectAndReturn(&huart2, NULL, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_UINT8(old_cmd, cmd);
}

/* Test Case CH_05: Reject UART receive failure
   Input: HAL_UART_Receive returns HAL_ERROR
   Expect: Function returns 0, *cmd_out is unchanged
*/
void test_CH_05_Reject_UART_receive_failure(void)
{
    INIT_CMD_OUT();

    /* HAL_UART_Receive will not populate rx because error is returned */
    HAL_UART_Receive_ExpectAndReturn(&huart2, NULL, 1, 10, HAL_ERROR);

    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_UINT8(old_cmd, cmd);
}

/* Test Case CH_06: Handle NULL pointer safely
   Input: cmd_out = NULL
   Expect: Function returns 0; no segmentation fault
*/
void test_CH_06_Handle_NULL_pointer_safely(void)
{
    /* No expectation for HAL_UART_Receive -- should not be called */
    int result = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL_INT(0, result);
    /* Test passes if no crash occurs */
}

/* Test Case CH_07: Do not modify output on invalid data (e.g., rx = '9')
   Input: rx = '9', HAL_UART_Receive returns HAL_OK
   Expect: *cmd_out unchanged */
void test_CH_07_No_modify_output_on_invalid_data(void)
{
    INIT_CMD_OUT();

    uint8_t rx = '9';
    HAL_UART_Receive_ExpectAndReturn(&huart2, NULL, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx, 1);

    int result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_UINT8(old_cmd, cmd);
}
