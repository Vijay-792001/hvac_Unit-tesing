#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_MotorController_Start_SetsPwmAndDirection(void)
{
    MotorDirection_t dir = MOTOR_DIRECTION_FORWARD;
    uint16_t speed = 100;

    HAL_TIM_PWM_Start_Expect(&htim2, TIM_CHANNEL_1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

    int result = MotorController_Start(dir, speed);
    TEST_ASSERT_EQUAL(MOTOR_SUCCESS, result);
}

void test_MotorController_Start_ReverseDirection(void)
{
    MotorDirection_t dir = MOTOR_DIRECTION_REVERSE;
    uint16_t speed = 50;

    HAL_TIM_PWM_Start_Expect(&htim2, TIM_CHANNEL_1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

    int result = MotorController_Start(dir, speed);
    TEST_ASSERT_EQUAL(MOTOR_SUCCESS, result);
}

void test_MotorController_Start_InvalidDirection(void)
{
    MotorDirection_t dir =  (MotorDirection_t)99;
    uint16_t speed = 50;
    int result = MotorController_Start(dir, speed);
    TEST_ASSERT_EQUAL(MOTOR_INVALID_DIRECTION, result);
}

void test_MotorController_Start_SpeedGreaterThanMax(void)
{
    MotorDirection_t dir = MOTOR_DIRECTION_FORWARD;
    uint16_t speed = 2000;

    int result = MotorController_Start(dir, speed);
    TEST_ASSERT_EQUAL(MOTOR_INVALID_SPEED, result);
}

void test_MotorController_Start_NullDirectionPointer(void)
{
    int result = MotorController_Start((MotorDirection_t)0, 100);
    TEST_ASSERT_NOT_EQUAL(MOTOR_SUCCESS, result);
}

void test_MotorController_Stop_SetsPwmLow(void)
{
    HAL_TIM_PWM_Stop_Expect(&htim2, TIM_CHANNEL_1);

    int result = MotorController_Stop();
    TEST_ASSERT_EQUAL(MOTOR_SUCCESS, result);
}
