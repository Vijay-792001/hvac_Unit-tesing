#include "unity.h"
#include "motor_controller.h"

/* Ceedling will auto-generate this mock */
#include "mock_stm32f4xx_hal.h"

#include "mock_position_sensing.h"


/* htim3 is defined in test/support/fake_tim_handle.c */
extern TIM_HandleTypeDef htim3;

/* =========================
 * Test Lifecycle Hooks
 * ========================= */
void setUp(void) {}
void tearDown(void) {}


static uint8_t g_pos_value = 0U;
static uint8_t PositionSensing_GetPosition_Callback(uint8_t *pos_out, int cmock_num_calls)
{
    (void)cmock_num_calls;
    if (pos_out != NULL)
    {
        *pos_out = g_pos_value;
    }
    return 1U; /* success */
}

static uint8_t PositionSensing_GetPosition_FailCb(uint8_t *pos_out, int cmock_num_calls)
{
    (void)pos_out;
    (void)cmock_num_calls;
    return 0U; /* simulate failure */
}

/* MC_07 helper: first GetPosition call succeeds (for MoveTo),
   second GetPosition call fails (for Update) */
static uint8_t PositionSensing_GetPosition_SucceedThenFailCb(uint8_t *pos_out, int cmock_num_calls)
{
    /* cmock_num_calls starts at 0 for the first call */
    if (cmock_num_calls == 0)
    {
        /* First call happens inside MotorController_MoveTo() */
        if (pos_out != NULL)
        {
            *pos_out = 1U; /* current position = 1 */
        }
        return 1U; /* success */
    }

    /* Second call happens inside MotorController_Update() */
    (void)pos_out;
    return 0U; /* simulate failure */
}

/*MC_08*/
static uint8_t PositionSensing_GetPosition_AlwaysOkCb(uint8_t *pos_out, int cmock_num_calls)
{
    (void)cmock_num_calls;
    if (pos_out != NULL)
    {
        *pos_out = 2U; /* any valid current position */
    }
    return 1U; /* success */
}



/* -------------------------
 * HAL_GPIO_Init is void → stub it to keep test simple
 * ------------------------- */
static void HAL_GPIO_Init_Callback(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init, int cmock_num_calls)
{
    (void)GPIOx;
    (void)GPIO_Init;
    (void)cmock_num_calls;
}

/* =========================
 * MC_01
 * Init sets safe state and starts PWM
 * ========================= */
void test_MotorController_Init_SetsSafeState_AndStartsPWM(void)
{
    uint8_t target = 0xFFU;

    /* Stub GPIO init */
    HAL_GPIO_Init_StubWithCallback(HAL_GPIO_Init_Callback);

    /* Expect safe direction pins */
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

    /* Expect PWM start */
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    /* Act */
    MotorController_Init();

    /* Assert internal state */
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());

    TEST_ASSERT_EQUAL_UINT8(1U, MotorController_GetTarget(&target));
    TEST_ASSERT_EQUAL_UINT8(0U, target);
}

/* =========================
 * MC_02
 * MoveTo: current < target => PIN0 RESET, PIN1 SET, state=MOVING_FWD, moving=1
 * ========================= */
void test_MotorController_MoveTo_TargetGreaterThanCurrent_MovesFwd(void)
{
    /* Arrange */
    g_pos_value = 1U; /* current position */

    PositionSensing_GetPosition_StubWithCallback(PositionSensing_GetPosition_Callback);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

    HAL_TIM_PWM_Start_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    /* Act */
    MotorController_MoveTo(4U);

    /* Assert */
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(1U, MotorController_IsMoving());
}

/* =========================
 * MC_03
 * MoveTo: current > target => PIN0 SET, PIN1 RESET, state=MOVING_REV, moving=1
 * ========================= */
void test_MotorController_MoveTo_TargetLessThanCurrent_MovesRev(void)
{
    /* Arrange */
    g_pos_value = 5U; /* current position */

    PositionSensing_GetPosition_StubWithCallback(PositionSensing_GetPosition_Callback);

    /* Expect direction pins for "reverse" per your code (else branch) */
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

    /* Expect PWM start */
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    /* Act */
    MotorController_MoveTo(2U);

    /* Assert */
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(1U, MotorController_IsMoving());
}

/* =========================
 * MC_04
 * MoveTo: current == target => Abort (STOPPED, moving=0, pins RESET, PWM stop)
 * ========================= */
void test_MotorController_MoveTo_AlreadyAtTarget_Aborts(void)
{
    /* Arrange: current position equals requested target */
    g_pos_value = 3U;
    PositionSensing_GetPosition_StubWithCallback(PositionSensing_GetPosition_Callback);

    /* Expect Abort() behavior: both pins RESET + PWM stop */
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    /* Act */
    MotorController_MoveTo(3U);

    /* Assert */
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());
}

/* =========================
 * MC_05
 * MoveTo: if PositionSensing_GetPosition fails => Abort
 * ========================= */
void test_MotorController_MoveTo_PositionInvalid_Aborts(void)
{
    /* Arrange: simulate GetPosition failure */
    PositionSensing_GetPosition_StubWithCallback(PositionSensing_GetPosition_FailCb);

    /* Expect Abort() behavior */
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    /* Act */
    MotorController_MoveTo(4U);

    /* Assert */
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());
}

/* =========================
 * MC_06
 * Update: when motor is NOT moving => do nothing
 * (No PositionSensing/HAL calls expected)
 * ========================= */
void test_MotorController_Update_WhenNotMoving_DoesNothing(void)
{
    /* Arrange: initialize controller (sets s_movement_active = 0) */
    HAL_GPIO_Init_StubWithCallback(HAL_GPIO_Init_Callback);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

    HAL_TIM_PWM_Start_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_Init();

    /* Act:
       Because motor is NOT moving, Update() should NOT call:
       - PositionSensing_Update
       - PositionSensing_GetPosition
       - PositionSensing_IsAtTarget
       - HAL_TIM_PWM_Stop / HAL_GPIO_WritePin, etc.
       We set NO expectations for those calls. If any happens, test fails. */
    MotorController_Update();

    /* Assert: state unchanged */
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());
}

/* =========================
 * MC_07
 * While moving: if GetPosition becomes invalid -> Abort
 * ========================= */
void test_MotorController_Update_WhenGetPositionFails_Aborts(void)
{
    /* --------- Phase 0: Init the motor controller --------- */
    HAL_GPIO_Init_StubWithCallback(HAL_GPIO_Init_Callback);

    /* Init() safe outputs + PWM start */
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_Init();

    /* --------- Phase 1: Start moving (movement_active = 1) --------- */
    /* MoveTo() calls GetPosition once (we return success and pos=1) */
    PositionSensing_GetPosition_StubWithCallback(PositionSensing_GetPosition_SucceedThenFailCb);

    /* Since current(1) < target(4), your code sets:
       PIN0 RESET, PIN1 SET and state = MOVING_FWD */
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_MoveTo(4U);

    TEST_ASSERT_EQUAL_UINT8(1U, MotorController_IsMoving());

    /* --------- Phase 2: Update() and force GetPosition failure --------- */
    /* Update() always calls PositionSensing_Update() first */
    PositionSensing_Update_Expect();

    /* Now GetPosition is called again inside Update().
       Our callback returns 0 on the 2nd call => Abort should happen. */

    /* Abort() behavior: reset both pins + stop PWM */
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_Update();

}

/* =========================
 * MC_08
 * While moving: stop when target reached using IsAtTarget(target)=1
 * ========================= */
void test_MotorController_Update_WhenAtTarget_Aborts(void)
{
    /* --------- Phase 0: Init --------- */
    HAL_GPIO_Init_StubWithCallback(HAL_GPIO_Init_Callback);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_Init();

    /* --------- Phase 1: Start moving to target=4 --------- */
    PositionSensing_GetPosition_StubWithCallback(PositionSensing_GetPosition_AlwaysOkCb);

    /* current(2) < target(4) => MOVING_FWD (per your code) */
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_MoveTo(4U);

    TEST_ASSERT_EQUAL_UINT8(1U, MotorController_IsMoving());

    /* --------- Phase 2: Update() reaches target --------- */
    PositionSensing_Update_Expect();

    /* GetPosition() must succeed inside Update() */
    /* (we already stubbed it AlwaysOkCb, so no Expect needed) */

    /* At target -> return 1 */
    PositionSensing_IsAtTarget_ExpectAndReturn(4U, 1U);

    /* Abort() expected */
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_Update();

}

/* =========================
 * MC_09
 * While moving: continue moving if NOT at target (IsAtTarget=0)
 * ========================= */
void test_MotorController_Update_WhenNotAtTarget_ContinuesMoving(void)
{
    /* --------- Phase 0: Init --------- */
    HAL_GPIO_Init_StubWithCallback(HAL_GPIO_Init_Callback);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_Init();

    /* --------- Phase 1: Start moving to target=4 --------- */
    PositionSensing_GetPosition_StubWithCallback(PositionSensing_GetPosition_AlwaysOkCb);

    /* current(2) < target(4) => MOVING_FWD */
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_MoveTo(4U);

    TEST_ASSERT_EQUAL_UINT8(1U, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());

    /* --------- Phase 2: Update() but NOT at target --------- */
    PositionSensing_Update_Expect();

    /* GetPosition() succeeds via stub callback */

    PositionSensing_IsAtTarget_ExpectAndReturn(4U, 0U);

    /* IMPORTANT:
       No Abort expected => no GPIO reset calls, no PWM stop call.
       With fail_on_unexpected_calls=true, if Abort happens, test fails. */

    MotorController_Update();

}

/* =========================
 * MC_10
 * Abort sets safe outputs and stops PWM
 * ========================= */
void test_MotorController_Abort_SetsSafeOutputs_AndStopsPWM(void)
{
    /* --------- Phase 0: Init --------- */
    HAL_GPIO_Init_StubWithCallback(HAL_GPIO_Init_Callback);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_Init();

    /* --------- Phase 1: Start moving (so abort is meaningful) --------- */
    g_pos_value = 1U;
    PositionSensing_GetPosition_StubWithCallback(PositionSensing_GetPosition_Callback);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_MoveTo(4U);

    TEST_ASSERT_EQUAL_UINT8(1U, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());

    /* --------- Phase 2: Abort --------- */
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_Abort();

    /* --------- Assert stopped --------- */
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());
    
}

/* =========================
 * MC_11
 * GetTarget: when stopped and target < 6 and ptr != NULL => returns 1 and copies target
 * ========================= */
void test_MotorController_GetTarget_WhenStopped_ReturnsStoredTarget(void)
{
    uint8_t target_out = 0xFFU;

    /* --------- Phase 0: Init --------- */
    HAL_GPIO_Init_StubWithCallback(HAL_GPIO_Init_Callback);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_Init();

    /* --------- Phase 1: Set target but force Abort (Position invalid) --------- */
    PositionSensing_GetPosition_StubWithCallback(PositionSensing_GetPosition_FailCb);

    /* Abort() effects inside MoveTo() */
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_MoveTo(4U);

    /* Confirm motor is stopped */
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());

    /* --------- Phase 2: GetTarget --------- */
    TEST_ASSERT_EQUAL_UINT8(1U, MotorController_GetTarget(&target_out));
    TEST_ASSERT_EQUAL_UINT8(4U, target_out);
}

/* =========================
 * MC_12
 * GetTarget returns 0 when motor is moving
 * ========================= */
void test_MotorController_GetTarget_WhenMotorMoving_Returns0(void)
{
    uint8_t out = 0xAAU;

    /* Init controller */
    HAL_GPIO_Init_StubWithCallback(HAL_GPIO_Init_Callback);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_Init();

    /* Simulate current position */
    g_pos_value = 1U;
    PositionSensing_GetPosition_StubWithCallback(PositionSensing_GetPosition_Callback);

    /* Start movement */
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_MoveTo(4U);

    /* Sanity check */
    TEST_ASSERT_EQUAL_UINT8(1U, MotorController_IsMoving());

    /* GetTarget must fail while moving */
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_GetTarget(&out));
}

/* =========================
 * MC_13
 * GetTarget(NULL) returns 0 (null pointer protection)
 * ========================= */
void test_MotorController_GetTarget_NullPointer_Returns0(void)
{
    /* Init controller */
    HAL_GPIO_Init_StubWithCallback(HAL_GPIO_Init_Callback);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_Init();

    /* Act + Assert */
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_GetTarget(NULL));
}

/* =========================
 * MC_14
 * GetTarget returns 0 when stored target is out of range (>=6)
 * ========================= */
void test_MotorController_GetTarget_TargetOutOfRange_Returns0(void)
{
    uint8_t target = 0xAA;

    /* ---- Init (motor stopped) ---- */
    HAL_GPIO_Init_StubWithCallback(HAL_GPIO_Init_Callback);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_Init();

    /* ---- Force stored target >= 6 ---- */
    /* Position is irrelevant here, but MoveTo must not crash */
    PositionSensing_GetPosition_StubWithCallback(PositionSensing_GetPosition_FailCb);

    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_ExpectAndReturn(&htim3, TIM_CHANNEL_1, HAL_OK);

    MotorController_MoveTo(6U);  /* invalid target stored */

    /* Sanity: motor stopped */
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_IsMoving());

    /* ---- Act + Assert ---- */
    TEST_ASSERT_EQUAL_UINT8(0U, MotorController_GetTarget(&target));

}















