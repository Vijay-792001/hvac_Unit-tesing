/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

/* Externals required by production file */
TIM_HandleTypeDef htim3;

/* GPIOB is needed for HAL_GPIO calls */
GPIO_TypeDef GPIOB_inst;
GPIO_TypeDef* GPIOB = &GPIOB_inst;

void setUp(void)
{
    /* CMock resets handled automatically */
}

void tearDown(void)
{
}

/* ------------ MC_01: Init sets safe state and starts PWM ------------ */
void test_MC_01_Init_sets_safe_state_and_starts_PWM(void)
{
    /* ...TEST CASE BODY OMITTED FOR BREVITY... */
}
/* ...all other MC_xx test cases omitted for brevity, same as above... */
