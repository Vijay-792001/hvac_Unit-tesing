/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_motor_controller.h"
#include "mock_status_indicator.h"
#include "mock_position_sensing.h"
// Additional necessary standard headers
#include <string.h>
// TEST PLAN: TC-CH-001 - Process valid command (START)
void test_CommandHandler_ProcessCommand_StartCommand_ShouldStartMotor_AndShowRunningStatus(void) {
    Command_t cmd;
    cmd.type = CMD_START;
    cmd.param = 0;

    motor_controller_start_ExpectAndReturn(MOTOR_ID_MAIN, 0);
    status_indicator_set_ExpectAndReturn(STATUS_RUNNING, 0);

    int result = CommandHandler_ProcessCommand(&cmd);
    TEST_ASSERT_EQUAL(0, result);
}
// TEST PLAN: TC-CH-002 - Process valid command (STOP)
void test_CommandHandler_ProcessCommand_StopCommand_ShouldStopMotor_AndShowIdleStatus(void) {
    Command_t cmd;
    cmd.type = CMD_STOP;
    cmd.param = 0;

    motor_controller_stop_ExpectAndReturn(MOTOR_ID_MAIN, 0);
    status_indicator_set_ExpectAndReturn(STATUS_IDLE, 0);

    int result = CommandHandler_ProcessCommand(&cmd);
    TEST_ASSERT_EQUAL(0, result);
}
// TEST PLAN: TC-CH-003 - Set position command with valid argument
void test_CommandHandler_ProcessCommand_SetPosition_ShouldInvokeMotorMoveToAndShowCalibrating(void) {
    Command_t cmd;
    cmd.type = CMD_SET_POSITION;
    cmd.param = 75;

    motor_controller_move_to_ExpectAndReturn(MOTOR_ID_MAIN, 75, 0);
    status_indicator_set_ExpectAndReturn(STATUS_CALIBRATING, 0);

    int result = CommandHandler_ProcessCommand(&cmd);
    TEST_ASSERT_EQUAL(0, result);
}
// TEST PLAN: TC-CH-004 - Unknown command
void test_CommandHandler_ProcessCommand_UnknownCommand_ShouldReturnError(void) {
    Command_t cmd;
    cmd.type = 99; // invalid command code
    cmd.param = 0;

    int result = CommandHandler_ProcessCommand(&cmd);
    TEST_ASSERT_EQUAL(COMMAND_ERR_INVALID_CMD, result);
}
// TEST PLAN: TC-CH-005 - NULL Command pointer parameter
void test_CommandHandler_ProcessCommand_NullPointer_ShouldReturnError(void) {
    int result = CommandHandler_ProcessCommand(NULL);
    TEST_ASSERT_EQUAL(COMMAND_ERR_NULL_POINTER, result);
}
// TEST PLAN: TC-CH-006 - Set position with out-of-range position (too high)
void test_CommandHandler_ProcessCommand_SetPosition_TooHigh_ShouldReturnError(void) {
    Command_t cmd;
    cmd.type = CMD_SET_POSITION;
    cmd.param = 101; // Out of range

    int result = CommandHandler_ProcessCommand(&cmd);
    TEST_ASSERT_EQUAL(COMMAND_ERR_OUT_OF_RANGE, result);
}
// TEST PLAN: TC-CH-007 - Set position with out-of-range position (too low)
void test_CommandHandler_ProcessCommand_SetPosition_TooLow_ShouldReturnError(void) {
    Command_t cmd;
    cmd.type = CMD_SET_POSITION;
    cmd.param = -1; // Out of range

    int result = CommandHandler_ProcessCommand(&cmd);
    TEST_ASSERT_EQUAL(COMMAND_ERR_OUT_OF_RANGE, result);
}
