#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_MotorController_Init_Should_ConfigurePWMTimerAndPins(void) {
    HAL_TIM_PWM_Start_Expect(&htim1, TIM_CHANNEL_1);
    MotorController_Init();
}

void test_MotorController_SetMotorSpeed_Should_UpdatePWMDutyCycle(void) {
    uint8_t speed = 120;
    __HAL_TIM_SET_COMPARE_Expect(&htim1, TIM_CHANNEL_1, 120);
    int ret = MotorController_SetSpeed(speed);
    TEST_ASSERT_EQUAL(0, ret);
}

void test_MotorController_SetMotorSpeed_Should_ClampToMaxAt255(void) {
    uint8_t speed = 255;
    __HAL_TIM_SET_COMPARE_Expect(&htim1, TIM_CHANNEL_1, 255);
    int ret = MotorController_SetSpeed(speed);
    TEST_ASSERT_EQUAL(0, ret);
}

void test_MotorController_SetMotorSpeed_Should_ReturnError_OnOutOfRange(void) {
    uint8_t speed = 300;
    int ret = MotorController_SetSpeed(speed);
    TEST_ASSERT_EQUAL(-1, ret);
}

void test_MotorController_SetMotorSpeed_Should_StopPWM_OnSpeed0(void) {
    uint8_t speed = 0;
    __HAL_TIM_SET_COMPARE_Expect(&htim1, TIM_CHANNEL_1, 0);
    int ret = MotorController_SetSpeed(speed);
    TEST_ASSERT_EQUAL(0, ret);
}

void test_MotorController_Deinit_Should_StopTimerAndDisable(void) {
    HAL_TIM_PWM_Stop_Expect(&htim1, TIM_CHANNEL_1);
    MotorController_Deinit();
}
