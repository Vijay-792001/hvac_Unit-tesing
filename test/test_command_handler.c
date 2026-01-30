#include "unity.h"
#include "command_handler.h"

/* Ceedling will auto-generate this mock */
#include "mock_stm32f4xx_hal.h"

/* Fake UART handle required by SUT */
UART_HandleTypeDef huart2;

/* =========================
 * Test Lifecycle Hooks
 * ========================= */
void setUp(void)
{
}

void tearDown(void)
{
}

/* =========================
 * CH-01
 * Accept valid command '0'
 * ========================= */
void test_CommandHandler_Accepts_Zero(void)
{
    uint8_t cmd = 0xFF;
    //uint8_t rx = '0';

   /*HAL_UART_Receive_ExpectAndReturn(
        &huart2,     // expected UART handle
        &rx,         // pointer passed to HAL
        1,           // Size
        10,          // Timeout
        HAL_OK       // return value
    );
    HAL_UART_Receive_ReturnThruPtr_pData(&rx);*/
    
    HAL_StatusTypeDef HAL_UART_Receive_Callback(
    UART_HandleTypeDef *huart,
    uint8_t *pData,
    uint16_t Size,
    uint32_t Timeout,
    int cmock_num_calls
)
{
    *pData = '0';   // simulate UART receiving '0'
    return HAL_OK;
}

HAL_UART_Receive_StubWithCallback(HAL_UART_Receive_Callback);

   




    uint8_t ret = CommandHandler_PollCommand(&cmd);

    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(0, cmd);
}

