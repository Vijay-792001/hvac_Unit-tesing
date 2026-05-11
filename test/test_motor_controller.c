#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

void setUp(void) {}
void tearDown(void) {}

/* MC_01: Init sets safe state and starts PWM */
void test_motor_controller_MC_01(void) {
    HAL_GPIO_Init_Expect(GPIOB, NULL);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Init();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    uint8_t tgt_val = 123;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&tgt_val));
    TEST_ASSERT_EQUAL_UINT8(0, tgt_val);
}

/* Additional MC_xx tests here... */