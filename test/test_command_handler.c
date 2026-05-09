/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* Test Plan ID: TC01 */
void test_command_handler_TC01(void)
{
    uint8_t rx_buffer[10] = {0};
    HAL_UART_Receive_ExpectAndReturn(&huart1, rx_buffer, sizeof(rx_buffer), HAL_OK);
    int result = command_handler_receive_command(rx_buffer, sizeof(rx_buffer));
    TEST_ASSERT_EQUAL(0, result);
}

/* Test Plan ID: TC02 */
void test_command_handler_TC02(void)
{
    uint8_t rx_buffer[10] = {0};
    HAL_UART_Receive_ExpectAndReturn(&huart1, rx_buffer, sizeof(rx_buffer), HAL_ERROR);
    int result = command_handler_receive_command(rx_buffer, sizeof(rx_buffer));
    TEST_ASSERT_EQUAL(-1, result);
}

/* Test Plan ID: TC03 */
void test_command_handler_TC03(void)
{
    uint8_t rx_buffer[10] = {0x01, 0x02};
    HAL_UART_Receive_ExpectAndReturn(&huart1, rx_buffer, sizeof(rx_buffer), HAL_OK);
    int result = command_handler_process_command(rx_buffer, sizeof(rx_buffer));
    TEST_ASSERT_EQUAL(1, result);
}

/* Test Plan ID: TC04 */
void test_command_handler_TC04(void)
{
    uint8_t rx_buffer[10] = {0xFF};
    HAL_UART_Receive_ExpectAndReturn(&huart1, rx_buffer, sizeof(rx_buffer), HAL_OK);
    int result = command_handler_process_command(rx_buffer, sizeof(rx_buffer));
    TEST_ASSERT_EQUAL(-1, result);
}

/* Test Plan ID: TC05 */
void test_command_handler_TC05(void)
{
    uint8_t rx_buffer[10] = {0};
    HAL_UART_Receive_ExpectAndReturn(&huart1, rx_buffer, sizeof(rx_buffer), HAL_OK);
    int result = command_handler_execute(rx_buffer, sizeof(rx_buffer));
    TEST_ASSERT_EQUAL(0, result);
}