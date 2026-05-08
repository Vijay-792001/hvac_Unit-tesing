#include "unity.h" #include "motor_controller.h" #include "mock_stm32f4xx_hal.h" #include "mock_position_sensing.h"

void setUp(void) {} void tearDown(void) {}

/* MC_01: Init sets safe state and starts PWM / void test_motor_controller_MC_01(void) { / Expect GPIOs initialized to safe state / HAL_GPIO_Init_Expect(GPIOB, NULL); / will match any GPIO_InitStruct ptr in this context */ HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

MotorController_Init();

TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());

uint8_t tgt_val = 123; /* pre-fill to detect assignment */
TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&tgt_val));
TEST_ASSERT_EQUAL_UINT8(0, tgt_val);
}

/* MC_02: Move forward when current < target */ void test_motor_controller_MC_02(void) { uint8_t fake_pos = 1;

/* Expect PositionSensing_GetPosition called */
PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
PositionSensing_GetPosition_ReturnMemThruPtr_current_position(&fake_pos, sizeof(fake_pos));

HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); /* FWD RESET */
HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);   /* REV SET */
HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

MotorController_MoveTo(4);

TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* MC_03: Move reverse when current > target */ void test_motor_controller_MC_03(void) { uint8_t fake_pos = 5;

PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
PositionSensing_GetPosition_ReturnMemThruPtr_current_position(&fake_pos, sizeof(fake_pos));

HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);     /* FWD SET */
HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);   /* REV RESET */
HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

MotorController_MoveTo(2);

TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
}

/* MC_04: No movement when already at target (Abort) */ void test_motor_controller_MC_04(void) { uint8_t fake_pos = 3;

PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
PositionSensing_GetPosition_ReturnMemThruPtr_current_position(&fake_pos, sizeof(fake_pos));

/* Abort will be called internally */
HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

MotorController_MoveTo(3);

TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_05: Abort if position invalid at start of MoveTo / void test_motor_controller_MC_05(void) { / PositionSensing_GetPosition returns 0 (invalid) */ PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

/* Abort internally */
HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

MotorController_MoveTo(4);

TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_06: Update does nothing when motor not moving / void test_motor_controller_MC_06(void) { / Ensure movement_active=0: after Init, it is 0. No mocks should run. */ MotorController_Init();

/* No position / GPIO / TIM actions expected */
MotorController_Update();

TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

/* MC_07: Update: motor moving, GetPosition invalid → Abort / void test_motor_controller_MC_07(void) { / Prepare movement active */ uint8_t fake_pos = 1; PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1); PositionSensing_GetPosition_ReturnMemThruPtr_current_position(&fake_pos, sizeof(fake_pos)); HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

MotorController_MoveTo(2);

PositionSensing_Update_Expect();

uint8_t dummy = 0;
PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);

/* Abort expected */
HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

MotorController_Update();

TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_08: Update: moving and IsAtTarget returns 1 → Abort / void test_motor_controller_MC_08(void) { / Prepare movement active */ uint8_t fake_pos = 3; PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1); PositionSensing_GetPosition_ReturnMemThruPtr_current_position(&fake_pos, sizeof(fake_pos)); HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1); MotorController_MoveTo(4);

/* MC_08 update step */
PositionSensing_Update_Expect();

uint8_t dummy = 4;
PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
PositionSensing_GetPosition_ReturnMemThruPtr_current_position(&dummy, sizeof(dummy));
PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);

/* Abort expected */
HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

MotorController_Update();

TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_09: Update: moving, IsAtTarget returns 0 → keep moving / void test_motor_controller_MC_09(void) { / Prepare movement active */ uint8_t fake_pos = 1; PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1); PositionSensing_GetPosition_ReturnMemThruPtr_current_position(&fake_pos, sizeof(fake_pos)); HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1); MotorController_MoveTo(4);

/* MC_09 update step */
PositionSensing_Update_Expect();
uint8_t dummy = 2;
PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
PositionSensing_GetPosition_ReturnMemThruPtr_current_position(&dummy, sizeof(dummy));
PositionSensing_IsAtTarget_ExpectAndReturn(4, 0);

/* No abort, so no GPIO_RESET or TIM_STOP expected */
MotorController_Update();

TEST_ASSERT_EQUAL_UINT8(1, MotorController_IsMoving());
/* No change in state */
}

/* MC_10: Abort sets safe outputs and stops PWM / void test_motor_controller_MC_10(void) { / Will always set STOPPED+0, and safe outputs */ HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

MotorController_Abort();

TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
TEST_ASSERT_EQUAL_UINT8(0, MotorController_IsMoving());
}

/* MC_11: GetTarget returns target when stopped and <6 and ptr valid / void test_motor_controller_MC_11(void) { / move to set a target, but finish at stopped first */

/* Prepare: Actually, short circuit to set internal state */
HAL_GPIO_Init_Expect(GPIOB, NULL);
HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);
MotorController_Init();

/* Emulate internal s_target_position < 6, stopped, movement==0 */
/* MoveTo sets target, but updates don't trigger movement here (simulate user scenario!) */
/* We'll simulate by setting target using MoveTo but immediate abort ensures stopped state */
uint8_t fake_pos = 2;
PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
PositionSensing_GetPosition_ReturnMemThruPtr_current_position(&fake_pos, sizeof(fake_pos));
HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
MotorController_MoveTo(2);

uint8_t tgt = 9;
TEST_ASSERT_EQUAL_UINT8(1, MotorController_GetTarget(&tgt));
TEST_ASSERT_EQUAL_UINT8(2, tgt);
}

/* MC_12: GetTarget returns 0 when moving / void test_motor_controller_MC_12(void) { / Move motor to set to moving state */ uint8_t fake_cur = 1; PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1); PositionSensing_GetPosition_ReturnMemThruPtr_current_position(&fake_cur, sizeof(fake_cur)); HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

MotorController_MoveTo(4); /* movement_active set */

uint8_t tgt = 7;
TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}

/* MC_13: GetTarget returns 0 on NULL pointer / void test_motor_controller_MC_13(void) { / Ensure stopped */ HAL_GPIO_Init_Expect(GPIOB, NULL); HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1); MotorController_Init();

TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(NULL));
}

/* MC_14: GetTarget returns 0 when stored target is out of range (>=6) / void test_motor_controller_MC_14(void) { / This sets s_target_position to 6, but position invalid: triggers abort, so after MoveTo target=6 */ PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0); HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

MotorController_MoveTo(6);

uint8_t tgt = 44;
TEST_ASSERT_EQUAL_UINT8(0, MotorController_GetTarget(&tgt));
}
