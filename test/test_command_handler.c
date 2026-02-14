#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

static Command_t cmd;
static SystemState_t state;

void setUp(void) {
    memset(&cmd, 0, sizeof(cmd));
    memset(&state, 0, sizeof(state));
}

void tearDown(void) {
}

void test_CommandHandler_HandleCommand_ValidCommand_ShouldSucceed(void)
{
    cmd.id = CMD_START;
    cmd.param = 1;

    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);

    int result = CommandHandler_HandleCommand(&cmd, &state);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(STATE_RUNNING, state.currentState);
}

void test_CommandHandler_HandleCommand_InvalidCommand_ShouldFail(void)
{
    cmd.id = CMD_INVALID;
    cmd.param = 0;

    int result = CommandHandler_HandleCommand(&cmd, &state);

    TEST_ASSERT_EQUAL(-1, result);
    TEST_ASSERT_EQUAL(STATE_IDLE, state.currentState);
}

void test_CommandHandler_HandleCommand_NullCommandPtr_ShouldFail(void)
{
    int result = CommandHandler_HandleCommand(NULL, &state);

    TEST_ASSERT_EQUAL(-2, result);
}

void test_CommandHandler_HandleCommand_NullStatePtr_ShouldFail(void)
{
    cmd.id = CMD_STOP;

    int result = CommandHandler_HandleCommand(&cmd, NULL);

    TEST_ASSERT_EQUAL(-3, result);
}

void test_CommandHandler_HandleCommand_StopCommand_SetsIdleState(void)
{
    cmd.id = CMD_STOP;
    cmd.param = 0;

    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

    int result = CommandHandler_HandleCommand(&cmd, &state);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(STATE_IDLE, state.currentState);
}

void test_CommandHandler_HandleCommand_MaxParameter_Accepted(void)
{
    cmd.id = CMD_START;
    cmd.param = 0xFFFF;

    HAL_GPIO_WritePin_Expect(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);

    int result = CommandHandler_HandleCommand(&cmd, &state);

    TEST_ASSERT_EQUAL(0, result);
}

void test_CommandHandler_HandleCommand_UnknownCommandID_Error(void)
{
    cmd.id = 255;

    int result = CommandHandler_HandleCommand(&cmd, &state);

    TEST_ASSERT_EQUAL(-1, result);
}
