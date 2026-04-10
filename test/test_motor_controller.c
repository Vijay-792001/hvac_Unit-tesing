/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

void setUp(void) {}
void tearDown(void) {}

/* MC_01: Init sets safe state and starts PWM */
void test_motor_controller_MC_01(void)
{
    HAL_GPIO_Init_Expect(GPIOB, (GPIO_InitTypeDef*)UnityAnyPtr);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Init();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    uint8_t tgt = 42;
    TEST_ASSERT_EQUAL(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL(0, tgt);
}

/* ...TRUNCATED FOR LENGTH; content includes all test cases as previously generated... */
