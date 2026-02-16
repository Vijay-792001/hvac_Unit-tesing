#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

// Test Case: MC_001 - Motor_Init with successful HAL init
void test_MotorController_MotorInit_SuccessfulHALInit_ReturnsOK(void)
{
    HAL_Motor_Init_ExpectAndReturn(HAL_OK);

    MotorStatus_t status = MotorController_MotorInit();

    TEST_ASSERT_EQUAL(MOTOR_STATUS_OK, status);
}

// Test Case: MC_002 - Motor_Init with failed HAL init
void test_MotorController_MotorInit_HALFailure_ReturnsError(void)
{
    HAL_Motor_Init_ExpectAndReturn(HAL_ERROR);

    MotorStatus_t status = MotorController_MotorInit();

    TEST_ASSERT_EQUAL(MOTOR_STATUS_HW_ERROR, status);
}

// Test Case: MC_003 - Start motor successfully
void test_MotorController_StartMotor_Successful_ReturnsOK(void)
{
    HAL_Motor_Start_ExpectAndReturn(HAL_OK);

    MotorStatus_t status = MotorController_StartMotor();

    TEST_ASSERT_EQUAL(MOTOR_STATUS_OK, status);
}

// Test Case: MC_004 - Start motor with HAL failure
void test_MotorController_StartMotor_HALFails_ReturnsError(void)
{
    HAL_Motor_Start_ExpectAndReturn(HAL_ERROR);

    MotorStatus_t status = MotorController_StartMotor();

    TEST_ASSERT_EQUAL(MOTOR_STATUS_HW_ERROR, status);
}

// Test Case: MC_005 - Stop motor successfully
void test_MotorController_StopMotor_Successful_ReturnsOK(void)
{
    HAL_Motor_Stop_ExpectAndReturn(HAL_OK);

    MotorStatus_t status = MotorController_StopMotor();

    TEST_ASSERT_EQUAL(MOTOR_STATUS_OK, status);
}

// Test Case: MC_006 - Stop motor with HAL failure
void test_MotorController_StopMotor_HALFails_ReturnsError(void)
{
    HAL_Motor_Stop_ExpectAndReturn(HAL_ERROR);

    MotorStatus_t status = MotorController_StopMotor();

    TEST_ASSERT_EQUAL(MOTOR_STATUS_HW_ERROR, status);
}

// Test Case: MC_007 - Get motor status normal operation
void test_MotorController_GetMotorStatus_ReturnsStatusValue(void)
{
    // No HAL dependency based on header, direct value returned
    motor_status_internal = MOTOR_STATUS_OK;
    MotorStatus_t status = MotorController_GetMotorStatus();
    TEST_ASSERT_EQUAL(MOTOR_STATUS_OK, status);
}
