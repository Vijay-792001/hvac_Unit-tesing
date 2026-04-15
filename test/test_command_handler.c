#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"
#include <string.h>
UART_HandleTypeDef huart2;
void setUp(void){ mock_stm32f4xx_hal_Init(); memset(&huart2, 0, sizeof(huart2));}
void tearDown(void){ mock_stm32f4xx_hal_Verify(); mock_stm32f4xx_hal_Destroy();}
void test_CH_01_Accept_valid_command_0(void) {uint8_t cmd=55,rx='0';HAL_UART_Receive_ExpectAndReturn(&huart2,&rx,1,10,HAL_OK);int ret=CommandHandler_PollCommand(&cmd);TEST_ASSERT_EQUAL_INT(1,ret);TEST_ASSERT_EQUAL_UINT8(0,cmd);}
void test_CH_02_Accept_valid_command_5(void) {uint8_t cmd=255,rx='5';HAL_UART_Receive_ExpectAndReturn(&huart2,&rx,1,10,HAL_OK);int ret=CommandHandler_PollCommand(&cmd);TEST_ASSERT_EQUAL_INT(1,ret);TEST_ASSERT_EQUAL_UINT8(5,cmd);}
void test_CH_03_Reject_numeric_out_of_range_command(void) {uint8_t initial_cmd=9,cmd=initial_cmd,rx='8';HAL_UART_Receive_ExpectAndReturn(&huart2,&rx,1,10,HAL_OK);int ret=CommandHandler_PollCommand(&cmd);TEST_ASSERT_EQUAL_INT(0,ret);TEST_ASSERT_EQUAL_UINT8(initial_cmd,cmd);}
void test_CH_04_Reject_non_numeric_command(void){uint8_t initial_cmd=99,cmd=initial_cmd,rx='x';HAL_UART_Receive_ExpectAndReturn(&huart2,&rx,1,10,HAL_OK);int ret=CommandHandler_PollCommand(&cmd);TEST_ASSERT_EQUAL_INT(0,ret);TEST_ASSERT_EQUAL_UINT8(initial_cmd,cmd);}
void test_CH_05_Reject_UART_receive_failure(void){uint8_t initial_cmd=7,cmd=initial_cmd,rx=0x7A;HAL_UART_Receive_ExpectAndReturn(&huart2,&rx,1,10,HAL_ERROR);int ret=CommandHandler_PollCommand(&cmd);TEST_ASSERT_EQUAL_INT(0,ret);TEST_ASSERT_EQUAL_UINT8(initial_cmd,cmd);}
void test_CH_06_Handle_NULL_pointer_safely(void){int ret=CommandHandler_PollCommand(NULL);TEST_ASSERT_EQUAL_INT(0,ret);}
void test_CH_07_Do_not_modify_output_on_invalid_data(void){uint8_t initial_cmd=4,cmd=initial_cmd,rx='9';HAL_UART_Receive_ExpectAndReturn(&huart2,&rx,1,10,HAL_OK);int ret=CommandHandler_PollCommand(&cmd);TEST_ASSERT_EQUAL_INT(0,ret);TEST_ASSERT_EQUAL_UINT8(initial_cmd,cmd);}
