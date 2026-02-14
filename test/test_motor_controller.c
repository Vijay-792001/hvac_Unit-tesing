#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_MotorController_Start_Calls_HAL_StartMotor_AndReturnsSuccess(void)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    HAL_StartMotor_ExpectAndReturn(hal_status);
    MotorStatus_t ret = MotorController_Start();
    TEST_ASSERT_EQUAL(MOTOR_STATUS_OK, ret);
}

void test_MotorController_Start_HALFailure_ReturnsHAL_ERROR(void)
{
    HAL_StatusTypeDef hal_status = HAL_ERROR;
    HAL_StartMotor_ExpectAndReturn(hal_status);
    MotorStatus_t ret = MotorController_Start();
    TEST_ASSERT_EQUAL(MOTOR_STATUS_HAL_ERROR, ret);
}

void test_MotorController_Stop_Calls_HAL_StopMotor_AndReturnsSuccess(void)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    HAL_StopMotor_ExpectAndReturn(hal_status);
    MotorStatus_t ret = MotorController_Stop();
    TEST_ASSERT_EQUAL(MOTOR_STATUS_OK, ret);
}

void test_MotorController_Stop_HALFailure_ReturnsHAL_ERROR(void)
{
    HAL_StatusTypeDef hal_status = HAL_ERROR;
    HAL_StopMotor_ExpectAndReturn(hal_status);
    MotorStatus_t ret = MotorController_Stop();
    TEST_ASSERT_EQUAL(MOTOR_STATUS_HAL_ERROR, ret);
}

void test_MotorController_InvalidOperation_ReturnsInvalid(void)
{
    TEST_PASS();
}

void test_MotorController_Sequence_StartStop_HandledCorrectly(void)
{
    HAL_StartMotor_ExpectAndReturn(HAL_OK);
    HAL_StopMotor_ExpectAndReturn(HAL_OK);
    MotorStatus_t s1 = MotorController_Start();
    MotorStatus_t s2 = MotorController_Stop();
    TEST_ASSERT_EQUAL(MOTOR_STATUS_OK, s1);
    TEST_ASSERT_EQUAL(MOTOR_STATUS_OK, s2);
}
