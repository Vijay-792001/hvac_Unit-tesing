#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_CommandHandler_Init_Should_InitializeStateAndDependencies(void) {
    CommandHandler_Init();
}

void test_CommandHandler_ProcessCommand_Should_TriggerAction_OnValidCommand(void) {
    Command_t valid_cmd;
    valid_cmd.id = CMD_MOTOR_ON;
    valid_cmd.param = 42;
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
    int ret = CommandHandler_ProcessCommand(&valid_cmd);
    TEST_ASSERT_EQUAL(0, ret);
}

void test_CommandHandler_ProcessCommand_Should_ReturnError_OnInvalidCommand(void) {
    Command_t invalid_cmd;
    invalid_cmd.id = 0xFF;
    invalid_cmd.param = 0;
    int ret = CommandHandler_ProcessCommand(&invalid_cmd);
    TEST_ASSERT_EQUAL(-1, ret);
}

void test_CommandHandler_ProcessCommand_Should_ReturnError_OnNullPointer(void) {
    int ret = CommandHandler_ProcessCommand(NULL);
    TEST_ASSERT_EQUAL(-2, ret);
}

void test_CommandHandler_Deinit_Should_PerformCleanup(void) {
    CommandHandler_Deinit();
}

void test_CommandHandler_ProcessCommand_Should_Handle_BoundaryParam(void) {
    Command_t bndry_cmd;
    bndry_cmd.id = CMD_MOTOR_ON;
    bndry_cmd.param = 0x7FFFFFFF;
    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
    int ret = CommandHandler_ProcessCommand(&bndry_cmd);
    TEST_ASSERT_EQUAL(0, ret);
}
