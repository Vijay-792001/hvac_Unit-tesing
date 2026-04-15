/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

void setUp(void) {}
void tearDown(void) {}

void test_motor_controller_MC_01(void)
{
    HAL_GPIO_Init_ExpectAnyArgs();
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Init();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    uint8_t tgt = 0xFF;
    TEST_ASSERT_EQUAL(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL_UINT8(0, tgt);
}

void test_motor_controller_MC_02(void)
{
    uint8_t fake_position = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&fake_position);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
}

void test_motor_controller_MC_03(void)
{
    uint8_t fake_position = 5;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&fake_position);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
}

void test_motor_controller_MC_04(void)
{
    uint8_t fake_position = 3;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&fake_position);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(3);

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

void test_motor_controller_MC_05(void)
{
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

void test_motor_controller_MC_06(void)
{
    /* To ensure update does nothing if not moving:
       No PositionSensing, HAL_GPIO_WritePin, or HAL_TIM_PWM_Stop/Start should be called.
       Just call it. */
    MotorController_Abort(); // Ensures movement_active=0
    MotorController_Update();

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

void test_motor_controller_MC_07(void)
{
    MotorController_MoveTo(3); // Start movement
    PositionSensing_Update_Expect();
    uint8_t pos_out = 0;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

void test_motor_controller_MC_08(void)
{
    MotorController_MoveTo(4); // Start moving to 4
    PositionSensing_Update_Expect();
    uint8_t valid_pos = 4;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&valid_pos);
    PositionSensing_IsAtTarget_Expect(4);
    PositionSensing_IsAtTarget_ReturnThruPtr_retval((uint8_t[]){1});

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

void test_motor_controller_MC_09(void)
{
    MotorController_MoveTo(4); // Start moving
    PositionSensing_Update_Expect();
    uint8_t valid_pos = 2;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_position(&valid_pos);
    PositionSensing_IsAtTarget_Expect(4);
    PositionSensing_IsAtTarget_ReturnThruPtr_retval((uint8_t[]){0});

    MotorController_Update();

    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
}

void test_motor_controller_MC_10(void)
{
    MotorController_MoveTo(2);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Abort();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

void test_motor_controller_MC_11(void)
{
    MotorController_Abort();
    MotorController_MoveTo(2);
    MotorController_Abort(); // Set stopped, set target=2, stopped state
    uint8_t tgt = 0xFF;
    TEST_ASSERT_EQUAL(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL_UINT8(2, tgt);
}

void test_motor_controller_MC_12(void)
{
    MotorController_MoveTo(3); // Sets movement_active=1
    uint8_t tgt = 0xFE;
    TEST_ASSERT_EQUAL(0, MotorController_GetTarget(&tgt));
}

void test_motor_controller_MC_13(void)
{
    MotorController_Abort();
    TEST_ASSERT_EQUAL(0, MotorController_GetTarget(NULL));
}

void test_motor_controller_MC_14(void)
{
    MotorController_Abort();
    MotorController_MoveTo(6); // sets target_position=6
    MotorController_Abort(); // Put into stopped state
    uint8_t tgt = 0xF0;
    TEST_ASSERT_EQUAL(0, MotorController_GetTarget(&tgt));
}
