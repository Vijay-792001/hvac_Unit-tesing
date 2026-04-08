/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

extern TIM_HandleTypeDef htim3;
GPIO_TypeDef test_GPIOB;
TIM_HandleTypeDef test_htim3;

void setUp(void)
{
    htim3 = test_htim3;
}

void tearDown(void) {}

/* MC_01: Init sets safe state and starts PWM */
void test_motor_controller_MC_01(void)
{
    HAL_GPIO_Init_Expect(GPIOB, NULL); // Will be ignored, struct value not checked by CMock
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Init();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    uint8_t t = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&t));
    TEST_ASSERT_EQUAL_UINT8(0, t);
}

/* MC_02: Move forward when current < target */
void test_motor_controller_MC_02(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out((uint8_t[]){1});
    
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* MC_03: Move reverse when current > target */
void test_motor_controller_MC_03(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out((uint8_t[]){5});
    
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* MC_04: No movement when already at target (Abort) */
void test_motor_controller_MC_04(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnThruPtr_pos_out((uint8_t[]){3});

    // MotorController_Abort will be called inside
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(3);

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_05: Abort if current position is invalid at start of MoveTo */
void test_motor_controller_MC_05(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0); // returns invalid

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_06: Update does nothing when motor not moving */
void test_motor_controller_MC_06(void)
{
    MotorController_Init();
    MotorController_Update();

    /* If not moving, these should not be called; nothing to assert, just verify logic doesn't change state */
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_07: Abort when GetPosition becomes invalid during Update */
void test_motor_controller_MC_07(void)
{
    MotorController_Init();
    // Simulate motor is moving
    MotorController_MoveTo(3); // sets movement_active

    // Now during update GetPosition returns invalid
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_08: While moving, stop when target reached using IsAtTarget */
void test_motor_controller_MC_08(void)
{
    MotorController_Init();
    MotorController_MoveTo(4);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_09: While moving: continue moving if not at target */
void test_motor_controller_MC_09(void)
{
    MotorController_Init();
    MotorController_MoveTo(4);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 0);

    MotorController_Update();

    /* No abort sequence should happen, so still moving */
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* MC_10: Abort sets safe outputs and stops PWM */
void test_motor_controller_MC_10(void)
{
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Abort();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_11: GetTarget returns target when stopped, valid, pointer valid */
void test_motor_controller_MC_11(void)
{
    MotorController_Init();
    uint8_t t = 88;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&t));
    TEST_ASSERT_EQUAL_UINT8(0, t); // after init, target=0 and is valid
}

/* MC_12: GetTarget returns 0 when moving */
void test_motor_controller_MC_12(void)
{
    MotorController_Init();
    MotorController_MoveTo(2);
    uint8_t t = 77;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&t));
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

    // Move to set value >=6; then overwrite state to simulate logic
    MotorController_MoveTo(6); // this updates s_target_position
    // Internally s_movement_active=1, so forcibly abort first
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_Abort();

    uint8_t t = 10;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&t));
}
