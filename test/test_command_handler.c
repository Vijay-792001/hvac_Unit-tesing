/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

// External UART handle as in the production code
UART_HandleTypeDef huart2;

/* Helper: value for an unchanged command */
#define CMD_OUT_UNCHANGED 0xA5

void setUp(void)
{
    // Reset mock history and any global state before each test
    mock_stm32f4xx_hal_Init();
}

void tearDown(void)
{
    mock_stm32f4xx_hal_Verify();
    mock_stm32f4xx_hal_Destroy();
}

/* CH_01: Accept valid command '0'
   rx = '0', HAL_OK => returns 1, *cmd_out = 0
*/
void test_CH_01_Accept_valid_command_0(void)
{
    uint8_t cmd_out = CMD_OUT_UNCHANGED;
    uint8_t rx = '0';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd_out);

    TEST_ASSERT_EQUAL_INT(1, ret);
    TEST_ASSERT_EQUAL_HEX8(0, cmd_out);
}

/* CH_02: Accept valid command '5' (upper bound)
   rx = '5', HAL_OK => returns 1, *cmd_out = 5
*/
void test_CH_02_Accept_valid_command_5_upper_bound(void)
{
    uint8_t cmd_out = CMD_OUT_UNCHANGED;
    uint8_t rx = '5';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd_out);

    TEST_ASSERT_EQUAL_INT(1, ret);
    TEST_ASSERT_EQUAL_HEX8(5, cmd_out);
}

/* CH_03: Reject numeric out-of-range command
   rx = '8', HAL_OK => returns 0, *cmd_out unchanged
*/
void test_CH_03_Reject_numeric_out_of_range_command(void)
{
    uint8_t cmd_out = CMD_OUT_UNCHANGED;
    uint8_t rx = '8';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd_out);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_HEX8(CMD_OUT_UNCHANGED, cmd_out);
}

/* CH_04: Reject non-numeric command
   rx = 'x', HAL_OK => returns 0, *cmd_out unchanged
*/
void test_CH_04_Reject_non_numeric_command(void)
{
    uint8_t cmd_out = CMD_OUT_UNCHANGED;
    uint8_t rx = 'x';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd_out);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_HEX8(CMD_OUT_UNCHANGED, cmd_out);
}

/* CH_05: Reject UART receive failure
   HAL_UART_Receive returns HAL_ERROR => returns 0, *cmd_out unchanged
*/
void test_CH_05_Reject_UART_receive_failure(void)
{
    uint8_t cmd_out = CMD_OUT_UNCHANGED;
    uint8_t rx = 0;

    // Do not care value of rx, because receive fails
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_ERROR);

    int ret = CommandHandler_PollCommand(&cmd_out);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_HEX8(CMD_OUT_UNCHANGED, cmd_out);
}

/* CH_06: Handle NULL pointer safely
   cmd_out = NULL => returns 0, no crash
*/
void test_CH_06_Handle_NULL_pointer_safely(void)
{
    int ret = CommandHandler_PollCommand(NULL);

    TEST_ASSERT_EQUAL_INT(0, ret);
    // There is nothing else to check; test passes if no crash
}

/* CH_07: Do not modify output on invalid data (rx = '9')
   Should return 0, *cmd_out remains unchanged
*/
void test_CH_07_Do_not_modify_output_on_invalid_data_9(void)
{
    uint8_t cmd_out = CMD_OUT_UNCHANGED;
    uint8_t rx = '9';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    int ret = CommandHandler_PollCommand(&cmd_out);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_HEX8(CMD_OUT_UNCHANGED, cmd_out);
}
