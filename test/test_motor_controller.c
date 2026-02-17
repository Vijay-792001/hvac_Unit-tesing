#include "unity.h"
#include "motor_controller.h"
#include "mock_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_MotorController_StartMotor_Valid_ShouldInvokeHALAndSucceed(void)
{
    Motor_t motor = MOTOR_1;
    mock_HAL_MotorStart_ExpectAndReturn(motor, HAL_OK);
    int ret = MotorController_Start(motor);
    TEST_ASSERT_EQUAL(0, ret);
}

void test_MotorController_StartMotor_HAL_Fail_ShouldReturnError(void)
{
    Motor_t motor = MOTOR_2;
    mock_HAL_MotorStart_ExpectAndReturn(motor, HAL_ERROR);
    int ret = MotorController_Start(motor);
    TEST_ASSERT_EQUAL(-2, ret);
}

void test_MotorController_StopMotor_Valid_ShouldInvokeHALAndSucceed(void)
{
    Motor_t motor = MOTOR_1;
    mock_HAL_MotorStop_ExpectAndReturn(motor, HAL_OK);
    int ret = MotorController_Stop(motor);
    TEST_ASSERT_EQUAL(0, ret);
}

void test_MotorController_StopMotor_HAL_Fail_ShouldReturnError(void)
{
    Motor_t motor = MOTOR_2;
    mock_HAL_MotorStop_ExpectAndReturn(motor, HAL_ERROR);
    int ret = MotorController_Stop(motor);
    TEST_ASSERT_EQUAL(-2, ret);
}

void test_MotorController_InvalidMotorID_ShouldReturnError(void)
{
    Motor_t motor = (Motor_t)99;
    int ret_start = MotorController_Start(motor);
    int ret_stop = MotorController_Stop(motor);
    TEST_ASSERT_EQUAL(-1, ret_start);
    TEST_ASSERT_EQUAL(-1, ret_stop);
}
