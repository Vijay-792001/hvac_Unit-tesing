#include "unity.h"
#include "command_handler.h"
#include "mock_motor_controller.h"
#include "mock_status_indicator.h"
#include "mock_position_sensing.h"

void setUp(void) {}
void tearDown(void) {}

void test_CommandHandler_ProcessCommand_MotorStart_Success(void) {
    Command_t cmd = CMD_MOTOR_START;
    motor_controller_start_ExpectAndReturn(MOTOR_1, 0);
    status_indicator_set_ExpectAndReturn(STATUS_RUNNING, 0);
    int result = CommandHandler_ProcessCommand(cmd);
    TEST_ASSERT_EQUAL(0, result);
}

void test_CommandHandler_ProcessCommand_MotorStop_Success(void) {
    Command_t cmd = CMD_MOTOR_STOP;
    motor_controller_stop_ExpectAndReturn(MOTOR_1, 0);
    status_indicator_set_ExpectAndReturn(STATUS_STOPPED, 0);
    int result = CommandHandler_ProcessCommand(cmd);
    TEST_ASSERT_EQUAL(0, result);
}

void test_CommandHandler_ProcessCommand_InvalidCommand_Failure(void) {
    Command_t cmd = (Command_t)99;
    int result = CommandHandler_ProcessCommand(cmd);
    TEST_ASSERT_EQUAL(-1, result);
}

void test_CommandHandler_ProcessCommand_GetPosition_Success(void) {
    Command_t cmd = CMD_GET_POSITION;
    position_t position = {0};
    position_sensing_get_ExpectAndReturn(&position, 0);
    status_indicator_set_ExpectAndReturn(STATUS_POSITION_READ, 0);
    int result = CommandHandler_ProcessCommand(cmd);
    TEST_ASSERT_EQUAL(0, result);
}

void test_CommandHandler_ProcessCommand_StatusIndicatorSetFailure(void) {
    Command_t cmd = CMD_MOTOR_START;
    motor_controller_start_ExpectAndReturn(MOTOR_1, 0);
    status_indicator_set_ExpectAndReturn(STATUS_RUNNING, -1);
    int result = CommandHandler_ProcessCommand(cmd);
    TEST_ASSERT_EQUAL(-2, result);
}

void test_CommandHandler_ProcessCommand_MotorControllerFailure(void) {
    Command_t cmd = CMD_MOTOR_START;
    motor_controller_start_ExpectAndReturn(MOTOR_1, -1);
    int result = CommandHandler_ProcessCommand(cmd);
    TEST_ASSERT_EQUAL(-3, result);
}

void test_CommandHandler_ProcessCommand_PositionSensingFailure(void) {
    Command_t cmd = CMD_GET_POSITION;
    position_t position = {0};
    position_sensing_get_ExpectAndReturn(&position, -1);
    int result = CommandHandler_ProcessCommand(cmd);
    TEST_ASSERT_EQUAL(-4, result);
}
