/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

extern TIM_HandleTypeDef htim3;

void setUp(void) {}
void tearDown(void) {}

/* MC_01: Init sets safe state and starts PWM */
void test_motor_controller_MC_01(void)
{
    GPIO_InitTypeDef expected_gpio_init;
    expected_gpio_init.Pin = 0x0001 | 0x0002; // GPIO_PIN_0 | GPIO_PIN_1
    expected_gpio_init.Mode = 0x00000001U;    // GPIO_MODE_OUTPUT_PP
    expected_gpio_init.Pull = 0x00000000U;    // GPIO_NOPULL
    expected_gpio_init.Speed = 0x00000001U;   // GPIO_SPEED_FREQ_LOW

    HAL_GPIO_Init_Expect(GPIOB, &expected_gpio_init);
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0); // GPIO_PIN_0, GPIO_PIN_RESET
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 0); // GPIO_PIN_1, GPIO_PIN_RESET
    HAL_TIM_PWM_Start_Expect(&htim3, 1U);       // TIM_CHANNEL_1

    MotorController_Init();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_INT(MOTOR_STATE_STOPPED, MotorController_GetState());
    uint8_t t = 55;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&t));
    TEST_ASSERT_EQUAL_UINT8(0, t);
}

/* MC_02: Move forward when current < target */
void test_motor_controller_MC_02(void)
{
    /* Ensure clean state */
    MotorController_Init();

    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out((uint8_t[]){1});
    PositionSensing_GetPosition_IgnoreArg_pos_out();

    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0); // DIR_FWD RESET
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 1); // DIR_REV SET
    HAL_TIM_PWM_Start_Expect(&htim3, 1U);

    MotorController_MoveTo(4);

    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_INT(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
}

/* MC_03: Move reverse when current > target */
void test_motor_controller_MC_03(void)
{
    MotorController_Init();

    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out((uint8_t[]){5});
    PositionSensing_GetPosition_IgnoreArg_pos_out();

    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 1); // DIR_FWD SET
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 0); // DIR_REV RESET
    HAL_TIM_PWM_Start_Expect(&htim3, 1U);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_INT(MOTOR_STATE_MOVING_REV, MotorController_GetState());
}

/* MC_04: No movement when already at target (Abort) */
void test_motor_controller_MC_04(void)
{
    MotorController_Init();

    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out((uint8_t[]){3});
    PositionSensing_GetPosition_IgnoreArg_pos_out();

    /* Abort: both pins reset, PWM stopped */
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 0);
    HAL_TIM_PWM_Stop_Expect(&htim3, 1U);

    MotorController_MoveTo(3);

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_INT(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_05: Abort if current position is invalid at start of MoveTo */
void test_motor_controller_MC_05(void)
{
    MotorController_Init();

    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 0);
    HAL_TIM_PWM_Stop_Expect(&htim3, 1U);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_INT(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_06: Update does nothing when motor not moving */
void test_motor_controller_MC_06(void)
{
    MotorController_Init();

    MotorController_Update();

    /* Nothing to assert, just checking no calls/exceptions and still not moving/stopped */
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_INT(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_07: Abort during Update if GetPosition becomes invalid */
void test_motor_controller_MC_07(void)
{
    MotorController_Init();

    /* Start a move (simulate with s_movement_active=1): MoveTo with valid->invalid update */
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out((uint8_t[]){3});
    PositionSensing_GetPosition_IgnoreArg_pos_out();

    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 0);
    HAL_TIM_PWM_Start_Expect(&htim3, 1U);

    MotorController_MoveTo(2);

    /* Now, during update, simulate invalid position (GetPosition returns 0) */
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 0);
    HAL_TIM_PWM_Stop_Expect(&htim3, 1U);

    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_INT(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_08: Abort when target reached during Update */
void test_motor_controller_MC_08(void)
{
    MotorController_Init();

    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out((uint8_t[]){4});
    PositionSensing_GetPosition_IgnoreArg_pos_out();

    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 1);
    HAL_TIM_PWM_Start_Expect(&htim3, 1U);
    MotorController_MoveTo(4);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);

    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);

    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 0);
    HAL_TIM_PWM_Stop_Expect(&htim3, 1U);

    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_INT(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_09: While moving, Update continues if not at target */
void test_motor_controller_MC_09(void)
{
    MotorController_Init();

    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out((uint8_t[]){3});
    PositionSensing_GetPosition_IgnoreArg_pos_out();

    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 1);
    HAL_TIM_PWM_Start_Expect(&htim3, 1U);
    MotorController_MoveTo(4);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 0);

    MotorController_Update();

    /* Still moving, not stopped */
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
    TEST_ASSERT_TRUE_MESSAGE(
        (MotorController_GetState() == MOTOR_STATE_MOVING_FWD ||
         MotorController_GetState() == MOTOR_STATE_MOVING_REV),
        "State must be moving"
    );
}

/* MC_10: Abort sets safe outputs and stops PWM */
void test_motor_controller_MC_10(void)
{
    MotorController_Init();

    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 0);
    HAL_TIM_PWM_Stop_Expect(&htim3, 1U);

    MotorController_Abort();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL_INT(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_11: GetTarget returns target when stopped and ptr valid and < 6 */
void test_motor_controller_MC_11(void)
{
    MotorController_Init();

    uint8_t val = 2;
    /* MoveTo and finish to update target but stopped */
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&val);
    PositionSensing_GetPosition_IgnoreArg_pos_out();

    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 1);
    HAL_TIM_PWM_Start_Expect(&htim3, 1U);

    MotorController_MoveTo(val);

    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 0);
    HAL_TIM_PWM_Stop_Expect(&htim3, 1U);
    MotorController_Abort();

    uint8_t tgtq = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&tgtq));
    TEST_ASSERT_EQUAL_UINT8(val, tgtq);
}

/* MC_12: GetTarget returns 0 when moving */
void test_motor_controller_MC_12(void)
{
    MotorController_Init();

    uint8_t val = 2;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out(&val);
    PositionSensing_GetPosition_IgnoreArg_pos_out();

    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 1);
    HAL_TIM_PWM_Start_Expect(&htim3, 1U);

    MotorController_MoveTo(val);

    uint8_t tgt = 93;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}

/* MC_13: GetTarget returns 0 on NULL pointer */
void test_motor_controller_MC_13(void)
{
    MotorController_Init();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(NULL));
}

/* MC_14: GetTarget returns 0 when stored target is out of range (>=6) */
void test_motor_controller_MC_14(void)
{
    MotorController_Init();

    /* MoveTo(6) when PositionSensing_GetPosition is invalid causes abort and target=6 stored */
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 0);
    HAL_TIM_PWM_Stop_Expect(&htim3, 1U);

    MotorController_MoveTo(6);

    uint8_t tgt = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}
