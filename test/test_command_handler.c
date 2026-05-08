/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_motor_controller.h"
#include "mock_position_sensing.h"
#include "mock_status_indicator.h"

void setUp(void) {}
void tearDown(void) {}

void test_CommandHandler_Should_ExecuteMotorCommands(void) {
    MotorController_Start_Expect();
    CommandHandler_ExecuteCommand(COMMAND_START);
}

void test_CommandHandler_Should_UpdatePosition(void) {
    PositionSensing_Update_Expect();
    CommandHandler_ExecuteCommand(COMMAND_UPDATE_POSITION);
}
