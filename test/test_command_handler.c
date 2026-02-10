/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

extern UART_HandleTypeDef huart2;

void setUp(void) {}
void tearDown(void) {}

/*
 * TestPlan: Command Handler
 * CH_TC01: Accept valid command '0'
 * CH_TC02: Accept valid command '5'
 * CH_TC03: Reject numeric out-of-range command ('8')
 * CH_TC04: Reject non-numeric command ('x')
 * CH_TC05: Reject UART receive failure
 * CH_TC06: Handle NULL pointer safely
 * CH_TC07: Do not modify output on invalid data
 */

void test_CH_TC01_AcceptValidCommand_Zero(void)
{
    uint8_t result = 0xAA;
    uint8_t rx = '0';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    uint8_t ret = CommandHandler_PollCommand(&result);

    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(0, result);
}

void test_CH_TC02_AcceptValidCommand_Five(void)
{
    uint8_t result = 0xAA;
    uint8_t rx = '5';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    uint8_t ret = CommandHandler_PollCommand(&result);

    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(5, result);
}

void test_CH_TC03_RejectNumericOutOfRange(void)
{
    uint8_t result = 0xAB;
    uint8_t rx = '8';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    uint8_t ret = CommandHandler_PollCommand(&result);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0xAB, result);
}

void test_CH_TC04_RejectNonNumericCommand(void)
{
    uint8_t result = 0xAD;
    uint8_t rx = 'x';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    uint8_t ret = CommandHandler_PollCommand(&result);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0xAD, result);
}

void test_CH_TC05_RejectUARTRxFailure(void)
{
    uint8_t result = 0x12;
    uint8_t rx = 0x00;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_ERROR);

    uint8_t ret = CommandHandler_PollCommand(&result);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0x12, result);
}

void test_CH_TC06_HandleNullPointer(void)
{
    uint8_t ret = CommandHandler_PollCommand(NULL);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
}

void test_CH_TC07_OutputUnchangedOnInvalidData(void)
{
    uint8_t result = 0x55;
    uint8_t rx = '9';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    uint8_t ret = CommandHandler_PollCommand(&result);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0x55, result);
}

