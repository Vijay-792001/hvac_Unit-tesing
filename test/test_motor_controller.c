/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/* TC-MC-01: Test MotorController_Init calls correct HAL */
void test_MotorController_Init_CallsHalGpioInit(void)
{
    HAL_GPIO_Init_ExpectAnyArgs();
    MotorController_Init();
}

/* TC-MC-02: Test MoveTo with valid position issues correct PWM */
void test_MotorController_MoveTo_ValidPosition_CallsPwmStart(void)
{
    uint16_t target = 1500;
    HAL_TIM_PWM_Start_ExpectAnyArgs();
    HAL_TIM_PWM_ConfigChannel_ExpectAnyArgsAndReturn(HAL_OK);
    TEST_ASSERT_EQUAL(MOTOR_OK, MotorController_MoveTo(target));
}

/* TC-MC-03: Test MoveTo with boundary position (0) */
void test_MotorController_MoveTo_ZeroPosition_CallsPwmStart(void)
{
    uint16_t target = 0;
    HAL_TIM_PWM_Start_ExpectAnyArgs();
    HAL_TIM_PWM_ConfigChannel_ExpectAnyArgsAndReturn(HAL_OK);
    TEST_ASSERT_EQUAL(MOTOR_OK, MotorController_MoveTo(target));
}

/* TC-MC-04: Test MoveTo with maximum position (4095) */
void test_MotorController_MoveTo_MaxPosition_CallsPwmStart(void)
{
    uint16_t target = 4095;
    HAL_TIM_PWM_Start_ExpectAnyArgs();
    HAL_TIM_PWM_ConfigChannel_ExpectAnyArgsAndReturn(HAL_OK);
    TEST_ASSERT_EQUAL(MOTOR_OK, MotorController_MoveTo(target));
}

/* TC-MC-05: Test MoveTo HAL error handling */
void test_MotorController_MoveTo_HalConfigFail_ReportsError(void)
{
    uint16_t target = 2000;
    HAL_TIM_PWM_ConfigChannel_ExpectAnyArgsAndReturn(HAL_ERROR);
    TEST_ASSERT_EQUAL(MOTOR_ERROR, MotorController_MoveTo(target));
}

/* TC-MC-06: Test Stop issues HAL TIM Stop */
void test_MotorController_Stop_CallsHalTimStop(void)
{
    HAL_TIM_PWM_Stop_ExpectAnyArgs();
    MotorController_Stop();
}

/* TC-MC-07: Test MoveTo with position out of range is rejected */
void test_MotorController_MoveTo_PositionOutOfRange_ReturnsError(void)
{
    uint16_t target = 5000; // out of valid range
    TEST_ASSERT_EQUAL(MOTOR_ERROR, MotorController_MoveTo(target));
}

/* TC-MC-08: Test MoveTo NULL protection - does not call HAL */
void test_MotorController_MoveTo_NullProtection(void)
{
    // Do not expect HAL calls
    TEST_ASSERT_EQUAL(MOTOR_ERROR, MotorController_MoveTo(NULL));
}
