/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

#include <string.h>

/* Hardware "externs" needed by implementation */
TIM_HandleTypeDef htim3;
GPIO_TypeDef GPIOB_inst;
GPIO_TypeDef *GPIOB = &GPIOB_inst;

/* Used test constants (matches .c/.h) */
#define MOTOR_PWM_HANDLE      htim3
#define MOTOR_PWM_CHANNEL     TIM_CHANNEL_1
#define MOTOR_GPIO_PORT_DIR   GPIOB
#define MOTOR_PIN_DIR_FWD     GPIO_PIN_0
#define MOTOR_PIN_DIR_REV     GPIO_PIN_1

/* test support */
void setUp(void)
{
    mock_stm32f4xx_hal_Init();
    mock_position_sensing_Init();
    memset(&htim3, 0, sizeof(htim3));
}
void tearDown(void)
{
}

/* ========== MC_01: Init sets safe state and starts PWM ========== */
void test_MC_01_Init_sets_safe_state_and_starts_PWM(void)
{
    /* HAL_GPIO_Init is called, then both DIR pins RESET, then PWM started */
    HAL_GPIO_Init_Expect(MOTOR_GPIO_PORT_DIR, (GPIO_InitTypeDef*)UnityAnyArgsPointer());
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Init();

    /* Internal API checks */
    TEST_ASSERT_EQUAL_UINT8(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    uint8_t tgt = 42;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL_UINT8(0, tgt);
}

/* ========== MC_02: Move forward when current < target ========== */
void test_MC_02_Move_forward_when_current_less_than_target(void)
{
    MotorController_Init();

    /* Prepare for MoveTo: PositionSensing_GetPosition returns 1 */
    uint8_t pos = 1;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&pos, 1);

    /* Expectation: direction FWD RESET, REV SET, state MOVING_FWD, PWM started */
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_REV, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
}

/* ========== MC_03: Move reverse when current > target ========== */
void test_MC_03_Move_reverse_when_current_greater_than_target(void)
{
    MotorController_Init();

    uint8_t pos = 5;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&pos, 1);

    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_FWD, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
}

/* ========== MC_04: No movement if already at target (Abort) ========== */
void test_MC_04_No_movement_when_already_at_target_calls_Abort(void)
{
    MotorController_Init();

    /* Prepare for MoveTo, PositionSensing_GetPosition returns target==current */
    uint8_t pos = 3;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&pos, 1);

    /* Should call Abort: both dir pins RESET, PWM stopped */
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(3);

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* ========== MC_05: Abort if current position is invalid at start of MoveTo ========== */
void test_MC_05_Abort_if_PositionSensing_invalid_on_MoveTo(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    /* Should call Abort: both dir pins RESET, PWM stopped */
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, MOTOR_PWM_CHANNEL);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* ========== MC_06: Update does nothing when not moving ========== */
void test_MC_06_Update_does_nothing_when_not_moving(void)
{
    MotorController_Init();
    /* No calls to PositionSensing_* or HAL_* */
    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* ========== MC_07: Abort during MotorController_Update if GetPosition becomes invalid ========== */
void test_MC_07_Abort_on_invalid_position_during_Update(void)
{
    MotorController_Init();
    /* Simulate movement is active */
    /* Force state - triggers movement */
    MotorController_MoveTo(2);
    /* Make IsMoving=1 manually (would be set by MoveTo for valid move) */
    // For explicit setting, might need to call MoveTo on real MCU, but for test we'll skip.

    /* Update() should call PositionSensing_Update, then GetPosition (fail -> abort) */
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

    /* Abort sequence: Dir pins RESET, PWM stopped */
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, MOTOR_PWM_CHANNEL);

    /* Simulate update while moving and position becomes invalid */
    MotorController_Update();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* ========== MC_08: Stop at target using IsAtTarget during Update ========== */
void test_MC_08_Stop_at_target_when_IsAtTarget_1_during_Update(void)
{
    MotorController_Init();
    /* Simulate moving state */
    MotorController_MoveTo(4);

    /* On update, IsMoving==1, so will call Update, GetPosition(valid), then IsAtTarget */
    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);
    /* Abort sequence: dir pins RESET, PWM stopped */
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, MOTOR_PWM_CHANNEL);

    MotorController_Update();

    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* ========== MC_09: Continue moving if not at target ========== */
void test_MC_09_Continue_moving_if_not_at_target(void)
{
    MotorController_Init();
    MotorController_MoveTo(3);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_IsAtTarget_ExpectAndReturn(3, 0);
    /* No abort -> No HAL_TIM_PWM_Stop, no reset pins calls */
    MotorController_Update();

    TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* ========== MC_10: Abort sets safe outputs and stops PWM ========== */
void test_MC_10_Abort_sets_safe_outputs_and_stops_PWM(void)
{
    MotorController_Init();
    /* Should always set both pins to RESET and stop PWM */
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, MOTOR_PWM_CHANNEL);

    MotorController_Abort();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* ========== MC_11: GetTarget returns target when stopped and <6 and ptr valid ========== */
void test_MC_11_GetTarget_returns_target_when_stopped_and_ptr_valid(void)
{
    MotorController_Init();
    /* MoveTo valid target and force stopped state */
    MotorController_MoveTo(2);
    HAL_GPIO_WritePin_Ignore(); /* Ignore abort sequence for fake abort if any */
    HAL_TIM_PWM_Stop_Ignore();

    /* Simulate the target is stored = 2 and we're stopped */
    MotorController_Abort();

    uint8_t t_read = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&t_read));
    TEST_ASSERT_EQUAL_UINT8(2, t_read);
}

/* ========== MC_12: GetTarget returns 0 when moving ========== */
void test_MC_12_GetTarget_returns_0_when_moving(void)
{
    MotorController_Init();
    /* Trigger move (makes s_movement_active = 1 if GetPosition valid) */
    uint8_t pos = 0;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&pos, 1);

    HAL_GPIO_WritePin_ExpectAnyArgs(); /* Dir pin expects, not checked here */
    HAL_GPIO_WritePin_ExpectAnyArgs();
    HAL_TIM_PWM_Start_ExpectAnyArgs();

    MotorController_MoveTo(3);

    uint8_t tgt = 55;
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}

/* ========== MC_13: GetTarget returns 0 on NULL pointer ========== */
void test_MC_13_GetTarget_returns_0_on_NULL_pointer(void)
{
    MotorController_Init();

    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(NULL));
}

/* ========== MC_14: GetTarget returns 0 when stored target out of range ========== */
void test_MC_14_GetTarget_returns_0_for_out_of_range_target(void)
{
    MotorController_Init();
    /* Set a target >= 6 (out of range) */
    /* Direct field access forbidden; use MoveTo, but since MoveTo sets it only as a param,
       we can simulate by moving to 6 and mocking GetPosition to return valid to allow assignment */
    uint8_t pos = 3;
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_GetPosition_ReturnArrayThruPtr_pos_out(&pos, 1);

    HAL_GPIO_WritePin_ExpectAnyArgs(); /* Dir pin expects, not assert-checked here */
    HAL_GPIO_WritePin_ExpectAnyArgs();
    HAL_TIM_PWM_Start_ExpectAnyArgs();

    MotorController_MoveTo(6);

    uint8_t tgt = 21;
    /* GetTarget should fail because target position is >=6 */
    TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}
