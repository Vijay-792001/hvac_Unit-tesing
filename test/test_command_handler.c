/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

#include <string.h>

/* ----------- Test fixture and external mocks/objects ----------- */
UART_HandleTypeDef huart2;

/* ----------- Test Setup/Teardown (Ceedling/Unity hooks) ----------- */
void setUp(void)
{
    /* Reset all mocks before each test if using global state */
    mock_stm32f4xx_hal_Init();
    memset(&huart2, 0, sizeof(huart2));
}

void tearDown(void)
{
}

/* ----------------- CH_01: Valid command '0' --------------------- */
/* CH_01: Accept valid command '0' (rx='0', HAL_OK), expect return=1, *cmd_out=0 */
void test_CH_01_Accept_valid_command_0(void)
{
    uint8_t cmd = 0xFF;
    uint8_t rx_val = '0';

    /* Expect HAL_UART_Receive with correct params, return HAL_OK, place rx='0' */
    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx_val, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, result, "Function should accept command and return 1");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, cmd, "cmd_out value should be 0");
}

/* ----------------- CH_02: Valid command '5' (upper bound) --------------- */
/* CH_02: Accept valid command '5' (rx='5', HAL_OK), expect return=1, *cmd_out=5 */
void test_CH_02_Accept_valid_command_5_upper_bound(void)
{
    uint8_t cmd = 0xFF;
    uint8_t rx_val = '5';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx_val, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, result, "Should accept upper bound '5'");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(5, cmd, "cmd_out should be set to 5");
}

/* ----------------- CH_03: Reject out-of-range numeric command --------------- */
/* CH_03: Reject numeric out-of-range command (rx='8', HAL_OK). Expect return=0, *cmd_out unchanged. */
void test_CH_03_Reject_out_of_range_command(void)
{
    uint8_t original_cmd = 0x11;
    uint8_t cmd = original_cmd;
    uint8_t rx_val = '8';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx_val, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, result, "Should reject command '8'");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(original_cmd, cmd, "cmd_out should remain unchanged");
}

/* ----------------- CH_04: Reject non-numeric command --------------- */
/* CH_04: Reject non-numeric command (rx='x', HAL_OK). Expect return=0, *cmd_out unchanged. */
void test_CH_04_Reject_non_numeric_command(void)
{
    uint8_t original_cmd = 0xAB;
    uint8_t cmd = original_cmd;
    uint8_t rx_val = 'x';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx_val, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, result, "Should reject non-numeric");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(original_cmd, cmd, "cmd_out should not be modified");
}

/* ----------------- CH_05: Reject UART receive failure ------------------ */
/* CH_05: UART failure (HAL_ERROR), expect return=0, *cmd_out unchanged */
void test_CH_05_Reject_uart_receive_failure(void)
{
    uint8_t original_cmd = 0x99;
    uint8_t cmd = original_cmd;
    uint8_t rx_val = 0x00; /* Value doesn't matter since receive fails */

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_ERROR);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, result, "Should reject when HAL_UART_Receive fails");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(original_cmd, cmd, "cmd_out should not be modified on HAL_ERROR");
}

/* ----------------- CH_06: Handle NULL pointer safely ------------------ */
/* CH_06: cmd_out=NULL; ensure no crash, no UART receive, return=0 */
void test_CH_06_Handle_null_pointer_safely(void)
{
    /* Should immediately return 0 (no call to HAL_UART_Receive allowed!) */
    uint8_t result = CommandHandler_PollCommand(NULL);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, result, "Should return 0 immediately on NULL pointer");
    /* There should be NO calls to HAL_UART_Receive */
    TEST_ASSERT_EQUAL_UINT8(0, HAL_UART_Receive_GetCallCount());
}

/* ----------------- CH_07: Do not modify output on invalid data ('9') ------------- */
/* CH_07: rx = '9', expect *cmd_out unchanged, function returns 0 */
void test_CH_07_Do_not_modify_output_on_invalid_data_9(void)
{
    uint8_t original_cmd = 0x23;
    uint8_t cmd = original_cmd;
    uint8_t rx_val = '9';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx_val, 1, 10, HAL_OK);
    HAL_UART_Receive_ReturnArrayThruPtr_pData(&rx_val, 1);

    uint8_t result = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, result, "Should return 0 if command out-of-bounds");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(original_cmd, cmd, "cmd_out should stay unchanged for rx='9'");
}
