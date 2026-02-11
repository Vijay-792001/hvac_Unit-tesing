/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_motor_controller.h"
#include "mock_position_sensing.h"
#include "mock_status_indicator.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/* TC-CH-01: Test initialization sets status to IDLE and blinks status LED */
void test_CommandHandler_Init_SetsStatusToIdleAndBlinksLed(void)
{
    StatusIndicator_Blink_Expect();
    CommandHandler_Init();
    TEST_ASSERT_EQUAL(COMMAND_STATUS_IDLE, CommandHandler_GetStatus());
}

/* TC-CH-02: Test handling valid MOVE command */
void test_CommandHandler_HandleCommand_Move_SendsCorrectMotorCommand(void)
{
    Command_t cmd = { .type = COMMAND_MOVE, .target_position = 123 };
    MotorController_MoveTo_Expect(cmd.target_position);
    StatusIndicator_Blink_Expect();
    CommandHandler_HandleCommand(&cmd);
}

/* TC-CH-03: Test handling valid STOP command */
void test_CommandHandler_HandleCommand_Stop_SendsMotorStop(void)
{
    Command_t cmd = { .type = COMMAND_STOP };
    MotorController_Stop_Expect();
    StatusIndicator_Blink_Expect();
    CommandHandler_HandleCommand(&cmd);
}

/* TC-CH-04: Test handling GET_POSITION command triggers position callback */
void test_CommandHandler_HandleCommand_GetPosition_CallsPositionRead(void)
{
    Command_t cmd = { .type = COMMAND_GET_POSITION };
    PositionSensing_Read_Expect();
    StatusIndicator_Blink_Expect();
    CommandHandler_HandleCommand(&cmd);
}

/* TC-CH-05: Test handling invalid command sets error */
void test_CommandHandler_HandleCommand_InvalidType_SetsError(void)
{
    Command_t cmd = { .type = 99 };
    StatusIndicator_Blink_Expect();
    CommandHandler_HandleCommand(&cmd);
    TEST_ASSERT_EQUAL(COMMAND_STATUS_ERROR, CommandHandler_GetStatus());
}

/* TC-CH-06: Test CommandHandler_HandleCommand with NULL command pointer */
void test_CommandHandler_HandleCommand_NullPointer_SetsError(void)
{
    StatusIndicator_Blink_Expect();
    CommandHandler_HandleCommand(NULL);
    TEST_ASSERT_EQUAL(COMMAND_STATUS_ERROR, CommandHandler_GetStatus());
}

/* TC-CH-07: Test status getter reflects latest status */
void test_CommandHandler_GetStatus_ReflectsStatus(void)
{
    StatusIndicator_Blink_Expect();
    CommandHandler_HandleCommand(NULL);
    TEST_ASSERT_EQUAL(COMMAND_STATUS_ERROR, CommandHandler_GetStatus());
}

/* TC-CH-08: Test CommandHandler_Init re-initializes and clears error */
void test_CommandHandler_Init_ClearsError(void)
{
    StatusIndicator_Blink_Expect(); // first for init
    StatusIndicator_Blink_Expect(); // re-init
    CommandHandler_HandleCommand(NULL); // Generate error
    CommandHandler_Init();
    TEST_ASSERT_EQUAL(COMMAND_STATUS_IDLE, CommandHandler_GetStatus());
}
