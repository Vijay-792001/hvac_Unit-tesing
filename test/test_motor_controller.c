#include "unity.h"
#include "motor_controller.h"
#include "mock_hal.h"

void setUp(void)
{
    mock_hal_Init();
}

void tearDown(void)
{
    mock_hal_Verify();
    mock_hal_Destroy();
}

// Test ID: TC_MC_001
void test_MotorController_Init_ShouldCallHALInit(void)
{
    HAL_MotorInit_Expect();

    MotorController_Init();
}

// Test ID: TC_MC_002
void test_MotorController_SetSpeed_ShouldSetValidSpeed(void)
{
    int speed = 100;
    HAL_SetMotorSpeed_ExpectAndReturn(speed, HAL_OK);

    int result = MotorController_SetSpeed(speed);

    TEST_ASSERT_EQUAL(0, result);
}

// Test ID: TC_MC_003
void test_MotorController_SetSpeed_ShouldRejectNegativeSpeed(void)
{
    int result = MotorController_SetSpeed(-10);

    TEST_ASSERT_EQUAL(-1, result);
}

// Test ID: TC_MC_004
void test_MotorController_SetSpeed_ShouldRejectSpeedAboveMax(void)
{
    int result = MotorController_SetSpeed(1001);

    TEST_ASSERT_EQUAL(-2, result);
}

// Test ID: TC_MC_005
void test_MotorController_SetSpeed_ShouldReturnErrorOnHALFailure(void)
{
    int speed = 300;
    HAL_SetMotorSpeed_ExpectAndReturn(speed, HAL_ERROR);

    int result = MotorController_SetSpeed(speed);

    TEST_ASSERT_EQUAL(-3, result);
}

// Test ID: TC_MC_006
void test_MotorController_Stop_ShouldStopMotor(void)
{
    HAL_StopMotor_ExpectAndReturn(HAL_OK);

    int result = MotorController_Stop();

    TEST_ASSERT_EQUAL(0, result);
}

// Test ID: TC_MC_007
void test_MotorController_Stop_ShouldHandleHALFailure(void)
{
    HAL_StopMotor_ExpectAndReturn(HAL_ERROR);

    int result = MotorController_Stop();

    TEST_ASSERT_EQUAL(-1, result);
}
