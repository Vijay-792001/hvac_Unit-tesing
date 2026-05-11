/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* MC_01: Init sets safe state and starts PWM */
void test_motor_controller_MC_01(void)
{
    HAL_GPIO_Init_Expect(GPIOB, NULL);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Init();
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    uint8_t target;
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_GetTarget(&target));
}

/* MC_02: Move forward when current < target */
void test_motor_controller_MC_02(void)
{
    uint8_t pos = 1;
    PositionSensing_GetPosition_ExpectAndReturn(&pos, 1);
    PositionSensing_GetPosition_IgnoreArg_current_position();
    PositionSensing_GetPosition_ReturnMemThruPtr_current_position(&pos, 1);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(1U, MotorController_IsMoving());
}

/* MC_03: Move reverse when current > target */
void test_motor_controller_MC_03(void)
{
    uint8_t pos = 5;
    PositionSensing_GetPosition_ExpectAndReturn(&pos, 1);
    PositionSensing_GetPosition_IgnoreArg_current_position();
    PositionSensing_GetPosition_ReturnMemThruPtr_current_position(&pos, 1);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(1U, MotorController_IsMoving());
}

/* MC_04: No movement when already at target (Abort) */
void test_motor_controller_MC_04(void)
{
    uint8_t pos = 3;
    PositionSensing_GetPosition_ExpectAndReturn(&pos, 1);
    PositionSensing_GetPosition_IgnoreArg_current_position();
    PositionSensing_GetPosition_ReturnMemThruPtr_current_position(&pos, 1);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(3);
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());
}

/* MC_05: Abort if current position is invalid at start of MoveTo */
void test_motor_controller_MC_05(void)
{
    PositionSensing_GetPosition_ExpectAndReturn(NULL, 0);
    PositionSensing_GetPosition_IgnoreArg_current_position();

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());
}

/* MC_06: Update does nothing when motor not moving */
void test_motor_controller_MC_06(void)
{
    MotorController_Update();
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_07: While motor moving and GetPosition invalid -> Abort */
void test_motor_controller_MC_07(void)
{
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAndReturn(NULL, 0);
    PositionSensing_GetPosition_IgnoreArg_current_position();

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();
}

/* MC_08: Stop when target reached */
void test_motor_controller_MC_08(void)
{
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAndReturn(NULL, 1);
    PositionSensing_GetPosition_IgnoreArg_current_position();
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();
}

/* MC_09: Continue moving if not at target */
void test_motor_controller_MC_09(void)
{
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAndReturn(NULL, 1);
    PositionSensing_GetPosition_IgnoreArg_current_position();
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 0);

    MotorController_Update();
}

/* MC_10: Abort sets safe outputs and stops PWM */
void test_motor_controller_MC_10(void)
{
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Abort();
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());
}

/* MC_11: GetTarget returns target when stopped */
void test_motor_controller_MC_11(void)
{
    uint8_t target;
    TEST_ASSERT_EQUAL_UINT8(1U, MotorController_GetTarget(&target));
}

/* MC_12: GetTarget returns 0 when moving */
void test_motor_controller_MC_12(void)
{
    uint8_t target;
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_GetTarget(&target));
}

/* MC_13: GetTarget returns 0 on NULL pointer */
void test_motor_controller_MC_13(void)
{
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_GetTarget(NULL));
}

/* MC_14: GetTarget returns 0 when stored target >=6 */
void test_motor_controller_MC_14(void)
{
    uint8_t target;
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_GetTarget(&target));
}