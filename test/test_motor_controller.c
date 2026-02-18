#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_MotorController_Start_Success(void) {
    HAL_GPIO_WritePin_ExpectAndReturn(GPIO_MOTOR_PORT, GPIO_PIN_MOTOR_ENABLE, GPIO_PIN_SET, HAL_OK);
    int result = motor_controller_start(MOTOR_1);
    TEST_ASSERT_EQUAL(0, result);
}

void test_MotorController_Start_HALFailure(void) {
    HAL_GPIO_WritePin_ExpectAndReturn(GPIO_MOTOR_PORT, GPIO_PIN_MOTOR_ENABLE, GPIO_PIN_SET, HAL_ERROR);
    int result = motor_controller_start(MOTOR_1);
    TEST_ASSERT_EQUAL(-1, result);
}

void test_MotorController_Stop_Success(void) {
    HAL_GPIO_WritePin_ExpectAndReturn(GPIO_MOTOR_PORT, GPIO_PIN_MOTOR_ENABLE, GPIO_PIN_RESET, HAL_OK);
    int result = motor_controller_stop(MOTOR_1);
    TEST_ASSERT_EQUAL(0, result);
}

void test_MotorController_Stop_HALFailure(void) {
    HAL_GPIO_WritePin_ExpectAndReturn(GPIO_MOTOR_PORT, GPIO_PIN_MOTOR_ENABLE, GPIO_PIN_RESET, HAL_ERROR);
    int result = motor_controller_stop(MOTOR_1);
    TEST_ASSERT_EQUAL(-1, result);
}

void test_MotorController_Start_InvalidMotor(void) {
    int result = motor_controller_start((motor_id_t)99);
    TEST_ASSERT_EQUAL(-2, result);
}

void test_MotorController_Stop_InvalidMotor(void) {
    int result = motor_controller_stop((motor_id_t)99);
    TEST_ASSERT_EQUAL(-2, result);
}
