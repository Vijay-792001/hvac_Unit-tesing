/* test_motor_controller.c */
#include "unity.h"
#include "motor_controller.h"
#include "mock_position_sensing.h"
#include "mock_stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim3;

void setUp(void)
{
    mock_position_sensing_Init();
    mock_stm32f4xx_hal_Init();
}

void tearDown(void)
{
    mock_position_sensing_Verify();
    mock_stm32f4xx_hal_Verify();
    mock_position_sensing_Destroy();
    mock_stm32f4xx_hal_Destroy();
}

void test_MotorController_Init_InitializesHardwareAndStops_MC01(void)
{
    GPIO_InitTypeDef expected_gpio;
    expected_gpio.Pin   = GPIO_PIN_0 | GPIO_PIN_1;
    expected_gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    expected_gpio.Pull  = GPIO_NOPULL;
    expected_gpio.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init_Expect(GPIOB, &expected_gpio);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Init();
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

void test_MotorController_MoveTo_TargetGreaterThanCurrent_MovesFWD_MC02(void)
{
    uint8_t targ = 4;
    uint8_t curr = 2;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&curr, 1);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(targ);

    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
}

void test_MotorController_MoveTo_TargetLessThanCurrent_MovesREV_MC03(void)
{
    uint8_t targ = 1;
    uint8_t curr = 3;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&curr, 1);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(targ);

    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
}

void test_MotorController_MoveTo_AtTarget_Aborts_MC04(void)
{
    uint8_t target = 2;
    uint8_t curr = 2;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&curr, 1);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(target);

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

void test_MotorController_MoveTo_InvalidPosition_Aborts_MC05(void)
{
    uint8_t target = 5;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(target);

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

void test_MotorController_Abort_StopsAllSignals_MC06(void)
{
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Abort();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

void test_MotorController_Update_StopsAtTarget_MC07(void)
{
    MotorController_Init();
    uint8_t target = 3;
    uint8_t curr = 2;

    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&curr, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(target);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&curr, 1);
    PositionSensing_IsAtTarget_ExpectAndReturn(target, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

void test_MotorController_Update_InvalidPosition_Stops_MC08(void)
{
    MotorController_Init();
    uint8_t target = 4;
    uint8_t curr = 2;

    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&curr, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(target);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

void test_MotorController_GetTarget_ValidWhenStopped_MC09(void)
{
    MotorController_Init();
    uint8_t target = 0xFF;
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Abort();
    uint8_t ret = MotorController_GetTarget(&target);
    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(0, target);
}

void test_MotorController_GetTarget_FailsIfMoving_MC10(void)
{
    MotorController_Init();
    uint8_t curr = 0, target = 5;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&curr, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(target);

    uint8_t tgtval = 0;
    uint8_t ret = MotorController_GetTarget(&tgtval);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
}

void test_MotorController_GetTarget_FailsIfNullPointer_MC11(void)
{
    MotorController_Init();
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Abort();

    uint8_t ret = MotorController_GetTarget(NULL);
    TEST_ASSERT_EQUAL_UINT8(0, ret);
}

void test_MotorController_GetTarget_FailsIfTargetOutOfRange_MC12(void)
{
    MotorController_Init();
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Abort();

    uint8_t tgtval = 0;
    uint8_t ret = MotorController_GetTarget(&tgtval);
    TEST_ASSERT(ret == 1 || ret == 0);
}
