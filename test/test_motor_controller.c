/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_position_sensing.h"
#include "mock_stm32f4xx_hal.h"

TIM_HandleTypeDef htim3;
GPIO_TypeDef GPIOB_inst;

void setUp(void) {}
void tearDown(void) {}

/* MC_01: Init sets safe state and starts PWM */
void test_MC_01_Init_sets_safe_state_and_starts_PWM(void)
{
    HAL_GPIO_Init_Expect(GPIOB, (GPIO_InitTypeDef*)ANY);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, 1U);

    MotorController_Init();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    uint8_t tgt = 0xFF;
    uint8_t ret = MotorController_GetTarget(&tgt);
    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(0, tgt);
}

/* MC_02: Move forward when current < target */
void test_MC_02_Move_forward_when_current_less_than_target(void)
{
    // Move to 4, current=1
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out((uint8_t[]){1}, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, 1U);

    MotorController_MoveTo(4);

    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* MC_03: Move reverse when current > target */
void test_MC_03_Move_reverse_when_current_greater_than_target(void)
{
    // Move to 2, current=5
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out((uint8_t[]){5}, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, 1U);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* MC_04: No movement when already at target (Abort) */
void test_MC_04_No_movement_when_already_at_target_calls_Abort(void)
{
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out((uint8_t[]){3}, 1);

    // MotorController_Abort called
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, 1U);

    MotorController_MoveTo(3);

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_05: Abort if current position is invalid at start of MoveTo */
void test_MC_05_Abort_if_current_position_invalid_at_start_of_MoveTo(void)
{
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, 1U);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_06: Update does nothing when motor not moving */
void test_MC_06_Update_does_nothing_when_motor_not_moving(void)
{
    // Expect no PositionSensing or HAL calls
    MotorController_Abort(); // Ensure not moving

    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_07: While moving, update aborts on invalid GetPosition */
void test_MC_07_Update_aborts_on_invalid_GetPosition(void)
{
    // Simulate movement
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    PositionSensing_Update_Expect();
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, 1U);

    // Force state to active
    MotorController_MoveTo(4); // Start moving (sets movement_active=1)
    // Patch: Now force state to active before update
    // In real code would set movement_active to 1
    MotorController_Update();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_08: While moving, update aborts when IsAtTarget reports 1 */
void test_MC_08_Update_aborts_when_IsAtTarget_reports_1(void)
{
    // Simulate movement
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_Update_Expect();
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, 1U);

    MotorController_MoveTo(4); // Start moving
    MotorController_Update();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_09: While moving, update continues if IsAtTarget=0 */
void test_MC_09_Update_continues_if_not_at_target(void)
{
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_Update_Expect();
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 0);

    MotorController_MoveTo(4);
    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* MC_10: Abort sets safe outputs and stops PWM */
void test_MC_10_Abort_sets_safe_outputs_and_stops_PWM(void)
{
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, 1U);

    MotorController_Abort();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_11: GetTarget returns target when stopped and target < 6 and ptr valid */
void test_MC_11_GetTarget_stopped_and_target_valid(void)
{
    MotorController_Init();
    MotorController_MoveTo(2);
    MotorController_Abort();

    uint8_t tgt = 0xFF;
    uint8_t ret = MotorController_GetTarget(&tgt);
    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(2, tgt);
}

/* MC_12: GetTarget returns 0 when moving */
void test_MC_12_GetTarget_returns_0_when_moving(void)
{
    MotorController_Init();
    MotorController_MoveTo(4);

    uint8_t tgt = 0xFF;
    uint8_t ret = MotorController_GetTarget(&tgt);
    TEST_ASSERT_EQUAL_UINT8(0, ret);
}

/* MC_13: GetTarget returns 0 on NULL pointer */
void test_MC_13_GetTarget_returns_0_on_NULL_pointer(void)
{
    MotorController_Init();
    MotorController_MoveTo(2);
    MotorController_Abort();
    uint8_t ret = MotorController_GetTarget(NULL);
    TEST_ASSERT_EQUAL_UINT8(0, ret);
}

/* MC_14: GetTarget returns 0 when target out of range (>=6) */
void test_MC_14_GetTarget_returns_0_when_target_out_of_range(void)
{
    MotorController_Init();
    MotorController_MoveTo(7); // Set target_position=7, which is >=6
    MotorController_Abort();

    uint8_t tgt = 0xFF;
    uint8_t ret = MotorController_GetTarget(&tgt);
    TEST_ASSERT_EQUAL_UINT8(0, ret);
}
