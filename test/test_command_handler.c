#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
    // Set up before each test
}

void tearDown(void)
{
    // Clean up after each test
}

void test_Command_Handler_Init_should_Initialize_State(void)
{
    Command_Handler_Init();
}

void test_Command_Handler_ProcessCommand_should_Handle_Valid_Command(void)
{
    Command_t command = { .type = COMMAND_TYPE_START, .param = 5 };
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
    Command_Handler_ProcessCommand(&command);
}

void test_Command_Handler_ProcessCommand_should_Not_Process_Null_Command(void)
{
    Command_Handler_ProcessCommand(NULL);
}

void test_Command_Handler_ProcessCommand_should_Handle_Boundary_Param(void)
{
    Command_t command = { .type = COMMAND_TYPE_SET_PARAM, .param = 0xFFFF };
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
    Command_Handler_ProcessCommand(&command);
}

void test_Command_Handler_ProcessCommand_should_Ignore_Unknown_Command(void)
{
    Command_t command = { .type = 999, .param = 2 };
    Command_Handler_ProcessCommand(&command);
}

void test_Command_Handler_ProcessCommand_should_Handle_HAL_Error_On_Start(void)
{
    Command_t command = { .type = COMMAND_TYPE_START, .param = 1 };
    HAL_GPIO_WritePin_ExpectAndReturn(GPIOA, GPIO_PIN_1, GPIO_PIN_SET, HAL_ERROR);
    Command_Handler_ProcessCommand(&command);
}
