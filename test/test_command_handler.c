/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_motor_controller.h"
#include "mock_position_sensing.h"
#include "mock_status_indicator.h"

void setUp(void) {}
void tearDown(void) {}

void test_command_handler_should_execute_motor_commands(void) {
    motor_controller_start_Expect();
    command_handler_execute();
}
