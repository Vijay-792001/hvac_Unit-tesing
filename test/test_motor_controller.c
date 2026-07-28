#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

TIM_HandleTypeDef htim3;
GPIO_TypeDef GPIOB_inst;

void setUp(void) {}
void tearDown(void) {}

void test_motor_controller_MC_01(void)
{
    uint8_t target = 5U;
    GPIO_InitTypeDef *init_ptr = NULL;

    HAL_GPIO_Init_Expect(GPIOB, init_ptr);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_Init();

    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());
}
