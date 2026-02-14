#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"

static MotorStatus_t motorStatus;

void setUp(void)
{
    memset(&motorStatus, 0, sizeof(motorStatus));
}

void tearDown(void)
{
}

void test_MotorController_StartMotor_Success(void)
{
    HAL_TIM_PWM_Start_Expect(&htim2, TIM_CHANNEL_1);

    int res = MotorController_StartMotor(&motorStatus);

    TEST_ASSERT_EQUAL(0, res);
    TEST_ASSERT_EQUAL(MOTOR_RUNNING, motorStatus.status);
}

void test_MotorController_StartMotor_AlreadyRunning_ShouldReturnError(void)
{
    motorStatus.status = MOTOR_RUNNING;

    int res = MotorController_StartMotor(&motorStatus);

    TEST_ASSERT_EQUAL(-1, res);
}

void test_MotorController_StartMotor_NullStatusPointer(void)
{
    int res = MotorController_StartMotor(NULL);

    TEST_ASSERT_EQUAL(-2, res);
}

void test_MotorController_StopMotor_Success(void)
{
    HAL_TIM_PWM_Stop_Expect(&htim2, TIM_CHANNEL_1);

    int res = MotorController_StopMotor(&motorStatus);

    TEST_ASSERT_EQUAL(0, res);
    TEST_ASSERT_EQUAL(MOTOR_STOPPED, motorStatus.status);
}

void test_MotorController_StopMotor_AlreadyStopped(void)
{
    motorStatus.status = MOTOR_STOPPED;

    int res = MotorController_StopMotor(&motorStatus);

    TEST_ASSERT_EQUAL(-1, res);
}

void test_MotorController_SetSpeed_Valid(void)
{
    uint16_t speed = 1500;
    HAL_TIM_PWM_SetCompare_Expect(&htim2, TIM_CHANNEL_1, speed);

    int res = MotorController_SetSpeed(&motorStatus, speed);

    TEST_ASSERT_EQUAL(0, res);
    TEST_ASSERT_EQUAL(1500, motorStatus.speed);
}

void test_MotorController_SetSpeed_TooHigh(void)
{
    uint16_t speed = 5000;

    int res = MotorController_SetSpeed(&motorStatus, speed);

    TEST_ASSERT_EQUAL(-1, res);
}

void test_MotorController_SetSpeed_Negative(void)
{
    int res = MotorController_SetSpeed(&motorStatus, -1);

    TEST_ASSERT_EQUAL(-1, res);
}

void test_MotorController_SetSpeed_NullStatusPointer(void)
{
    int res = MotorController_SetSpeed(NULL, 100);

    TEST_ASSERT_EQUAL(-2, res);
}
