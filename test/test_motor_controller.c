/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
// TEST PLAN: TC-MC-001 - Start main motor, normal
void test_MotorController_StartMainMotor_Normal_ShouldCallHALAndReturn0(void) {
    HAL_Motor_Start_ExpectAndReturn(HAL_MOTOR1, 0);
    int result = motor_controller_start(MOTOR_ID_MAIN);
    TEST_ASSERT_EQUAL(0, result);
}
// TEST PLAN: TC-MC-002 - Stop main motor, normal
void test_MotorController_StopMainMotor_Normal_ShouldCallHALAndReturn0(void) {
    HAL_Motor_Stop_ExpectAndReturn(HAL_MOTOR1, 0);
    int result = motor_controller_stop(MOTOR_ID_MAIN);
    TEST_ASSERT_EQUAL(0, result);
}
// TEST PLAN: TC-MC-003 - Move motor to in-range position
void test_MotorController_MoveTo_ValidPosition_CallsHALMove(void) {
    int target_pos = 45;
    HAL_Motor_MoveTo_ExpectAndReturn(HAL_MOTOR1, target_pos, 0);
    int result = motor_controller_move_to(MOTOR_ID_MAIN, target_pos);
    TEST_ASSERT_EQUAL(0, result);
}
// TEST PLAN: TC-MC-004 - Move motor with invalid id
void test_MotorController_MoveTo_InvalidId_ShouldReturnError(void) {
    int result = motor_controller_move_to(255, 25); // Invalid id
    TEST_ASSERT_EQUAL(MC_ERR_INVALID_ID, result);
}
// TEST PLAN: TC-MC-005 - Move motor to out-of-range position
void test_MotorController_MoveTo_OutOfRange_ShouldReturnError(void) {
    int result = motor_controller_move_to(MOTOR_ID_MAIN, 150); // Out of range
    TEST_ASSERT_EQUAL(MC_ERR_OUT_OF_RANGE, result);
}
// TEST PLAN: TC-MC-006 - Start with HAL error
void test_MotorController_Start_HALFailure_ShouldReturnError(void) {
    HAL_Motor_Start_ExpectAndReturn(HAL_MOTOR1, -1);
    int result = motor_controller_start(MOTOR_ID_MAIN);
    TEST_ASSERT_EQUAL(MC_ERR_HAL, result);
}
