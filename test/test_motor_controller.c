#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/*
Test Plan ID: TC_MC_01
Description: Test MotorController_Init initializes HAL and motor peripherals.
*/
void test_MotorController_Init_should_initialize_peripherals_TC_MC_01(void)
{
    HAL_Motor_Init_ExpectAndReturn(HAL_OK);

    TEST_ASSERT_EQUAL(MOTOR_CONTROLLER_OK, MotorController_Init());
}

/*
Test Plan ID: TC_MC_02
Description: Test MotorController_Init propagates HAL error.
*/
void test_MotorController_Init_should_return_error_on_HAL_failure_TC_MC_02(void)
{
    HAL_Motor_Init_ExpectAndReturn(HAL_ERROR);

    TEST_ASSERT_EQUAL(MOTOR_CONTROLLER_ERROR, MotorController_Init());
}

/*
Test Plan ID: TC_MC_03
Description: Test MotorController_SetSpeed calls HAL and returns success for valid input.
*/
void test_MotorController_SetSpeed_should_call_HAL_with_valid_speed_TC_MC_03(void)
{
    uint8_t speed = 80;
    HAL_Motor_SetSpeed_ExpectAndReturn(speed, HAL_OK);

    TEST_ASSERT_EQUAL(MOTOR_CONTROLLER_OK, MotorController_SetSpeed(speed));
}

/*
Test Plan ID: TC_MC_04
Description: Test MotorController_SetSpeed returns error for out-of-range speed.
*/
void test_MotorController_SetSpeed_should_return_error_on_invalid_speed_TC_MC_04(void)
{
    uint8_t speed = 255; // if out of range

    TEST_ASSERT_EQUAL(MOTOR_CONTROLLER_INVALID_PARAM, MotorController_SetSpeed(speed));
}

/*
Test Plan ID: TC_MC_05
Description: Test MotorController_SetSpeed handles HAL failure.
*/
void test_MotorController_SetSpeed_should_return_error_if_HAL_fails_TC_MC_05(void)
{
    uint8_t speed = 50;
    HAL_Motor_SetSpeed_ExpectAndReturn(speed, HAL_ERROR);

    TEST_ASSERT_EQUAL(MOTOR_CONTROLLER_ERROR, MotorController_SetSpeed(speed));
}

/*
Test Plan ID: TC_MC_06
Description: Test MotorController_Stop calls HAL.
*/
void test_MotorController_Stop_should_call_HAL_Stop_TC_MC_06(void)
{
    HAL_Motor_Stop_ExpectAndReturn(HAL_OK);

    TEST_ASSERT_EQUAL(MOTOR_CONTROLLER_OK, MotorController_Stop());
}

/*
Test Plan ID: TC_MC_07
Description: Test MotorController_Stop handles HAL failure.
*/
void test_MotorController_Stop_should_return_error_on_HAL_failure_TC_MC_07(void)
{
    HAL_Motor_Stop_ExpectAndReturn(HAL_ERROR);

    TEST_ASSERT_EQUAL(MOTOR_CONTROLLER_ERROR, MotorController_Stop());
}

/*
Test Plan ID: TC_MC_08
Description: Test NULL pointer handling on SetSpeed if speed is via pointer (else skip).
*/
void test_MotorController_SetSpeed_should_return_error_on_NULL_TC_MC_08(void)
{
    // If API is pointer based, test NULL. Otherwise, skip.
}
