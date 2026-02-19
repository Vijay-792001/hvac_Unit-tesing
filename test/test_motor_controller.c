#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

#include <string.h>
#include <stdint.h>

TIM_HandleTypeDef htim3;
GPIO_TypeDef GPIOB_inst;
#GPIO_TypeDef *GPIOB = &GPIOB_inst;

void setUp(void)
{
    MotorController_Init();
}

void tearDown(void)
{
}

void test_MC_01_MotorController_Init_sets_safe_state_and_starts_PWM(void)
{
    HAL_GPIO_Init_Expect(GPIOB, &((GPIO_InitTypeDef){
        .Pin = 0x0003,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW
    }));
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Init();
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    uint8_t tgt = 123;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL_UINT8(0, tgt);
}

void test_MC_02_MotorController_MoveTo_moves_forward_when_current_is_less_than_target(void)
{
    uint8_t input_pos = 1;
    PositionSensing_GetPosition_ExpectAndReturn(&input_pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(4);
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
}

void test_MC_03_MotorController_MoveTo_moves_reverse_when_current_greater_than_target(void)
{
    uint8_t input_pos = 5;
    PositionSensing_GetPosition_ExpectAndReturn(&input_pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(2);
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
}

void test_MC_04_MotorController_MoveTo_aborts_when_already_at_target(void)
{
    uint8_t input_pos = 3;
    PositionSensing_GetPosition_ExpectAndReturn(&input_pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(3);
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

void test_MC_05_MotorController_MoveTo_aborts_if_position_invalid(void)
{
    uint8_t pos_dummy = 0;
    PositionSensing_GetPosition_ExpectAndReturn(&pos_dummy, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(2);
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

void test_MC_06_MotorController_Update_no_action_if_not_moving(void)
{
    MotorController_Update();
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

void test_MC_07_MotorController_Update_aborts_on_invalid_position_while_moving(void)
{
    uint8_t input_pos = 0;
    PositionSensing_GetPosition_ExpectAndReturn(&input_pos, 0);
    MotorController_MoveTo(3);
    MotorController_MoveTo(1);
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Update();
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

void test_MC_08_MotorController_Update_stops_when_IsAtTarget_is_true(void)
{
    MotorController_MoveTo(4);
    PositionSensing_Update_Expect();
    uint8_t fake_curr = 4;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Update();
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

void test_MC_09_MotorController_Update_continues_if_not_at_target(void)
{
    MotorController_MoveTo(4);
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 0);
    MotorController_Update();
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

void test_MC_10_MotorController_Abort_sets_safe_outputs_and_PWM_stop(void)
{
    uint8_t current_pos = 1;
    PositionSensing_GetPosition_ExpectAndReturn(&current_pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(3);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Abort();
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

void test_MC_11_MotorController_GetTarget_returns_target_when_stopped_and_in_range(void)
{
    uint8_t store_target = 2;
    uint8_t input_pos = 1;
    PositionSensing_GetPosition_ExpectAndReturn(&input_pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(store_target);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Abort();
    uint8_t tgt_val = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&tgt_val));
    TEST_ASSERT_EQUAL_UINT8(store_target, tgt_val);
}

void test_MC_12_MotorController_GetTarget_returns_0_when_moving(void)
{
    uint8_t input_pos = 1;
    PositionSensing_GetPosition_ExpectAndReturn(&input_pos, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(2);
    uint8_t temp;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&temp));
}

void test_MC_13_MotorController_GetTarget_returns_0_on_null_pointer(void)
{
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Abort();
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(NULL));
}

void test_MC_14_MotorController_GetTarget_returns_0_when_target_out_of_range(void)
{
    uint8_t pos_dummy = 0;
    PositionSensing_GetPosition_ExpectAndReturn(&pos_dummy, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(6);
    uint8_t tgt = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}
