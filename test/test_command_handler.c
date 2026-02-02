#include "unity.h"
#include "command_handler.h"

/* Ceedling will auto-generate this mock */
#include "mock_stm32f4xx_hal.h"

/* Fake UART handle required by SUT */
UART_HandleTypeDef huart2;

static uint8_t g_rx = '0';
static HAL_StatusTypeDef g_status = HAL_OK;

/* =========================
 * Test Lifecycle Hooks
 * ========================= */
void setUp(void)
{
}

void tearDown(void)
{
}


static HAL_StatusTypeDef HAL_UART_Receive_Callback(
    UART_HandleTypeDef *huart,
    uint8_t *pData,
    uint16_t Size,
    uint32_t Timeout,
    int cmock_num_calls
)
{
    if (g_status == HAL_OK)
    {
        *pData = g_rx;   /* simulate UART receiving a byte */
    }
    return g_status;
}

/* =========================
 * CH-01
 * Accept valid command '0'
 * ========================= */
void test_CommandHandler_Accepts_Zero(void)
{
    uint8_t cmd = 0xFF;
    
    g_rx = '0';
    g_status = HAL_OK;
    HAL_UART_Receive_StubWithCallback(HAL_UART_Receive_Callback);

   
    uint8_t ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(0, cmd);
}

/* =========================
 * CH_02
 * Accept valid command '5' (upper bound)
 * ========================= */
void test_CommandHandler_Accepts_Five(void)
{
    uint8_t cmd = 0xFF;

    g_rx = '5';
    g_status = HAL_OK;
    HAL_UART_Receive_StubWithCallback(HAL_UART_Receive_Callback);

    uint8_t ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(5, cmd);
}

/* =========================
 * CH_03
 * Reject numeric out-of-range command
 * ========================= */
void test_CommandHandler_Rejects_OutOfRange_Command(void)
{
    uint8_t cmd = 0xFF;

    g_rx = '8';
    g_status = HAL_OK;
    HAL_UART_Receive_StubWithCallback(HAL_UART_Receive_Callback);

    uint8_t ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0xFF, cmd); /* unchanged */
}

/* =========================
 * CH_04
 * Reject non-numeric command
 * ========================= */
void test_CommandHandler_Rejects_NonNumeric_Command(void)
{
    uint8_t cmd = 0xFF;

    g_rx = 'x';
    g_status = HAL_OK;
    HAL_UART_Receive_StubWithCallback(HAL_UART_Receive_Callback);

    uint8_t ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0xFF, cmd); /* unchanged */
}

/* =========================
 * CH_05
 * Reject UART receive failure
 * ========================= */
void test_CommandHandler_Rejects_Uart_Receive_Failure(void)
{
    uint8_t cmd = 0xFF;

    g_status = HAL_ERROR;
    HAL_UART_Receive_StubWithCallback(HAL_UART_Receive_Callback);

    uint8_t ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0xFF, cmd); /* unchanged */
}

/* =========================
 * CH_06
 * Handle NULL pointer safely
 * ========================= */
void test_CommandHandler_Null_Pointer_Protected(void)
{
    /* No stub needed: function returns before calling HAL_UART_Receive */
    uint8_t ret = CommandHandler_PollCommand(NULL);
    TEST_ASSERT_EQUAL_UINT8(0, ret);
}

/* =========================
 * CH_07
 * Do not modify output on invalid data
 * ========================= */
void test_CommandHandler_DoesNot_Modify_Output_On_Invalid_Data(void)
{
    uint8_t cmd = 0xAA;

    g_rx = '9';
    g_status = HAL_OK;
    HAL_UART_Receive_StubWithCallback(HAL_UART_Receive_Callback);

    uint8_t ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0xAA, cmd); /* unchanged */
}

