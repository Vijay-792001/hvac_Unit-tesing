/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_status_indicator.h"

void setUp(void) {}
void tearDown(void) {}

void test_MotorController_Should_StartMotor(void) {
    StatusIndicator_SetRunning_Expect();
    MotorController_Start();
}

void test_MotorController_Should_StopMotor(void) {
    StatusIndicator_SetStopped_Expect();
    MotorController_Stop();
}
