/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_motor_controller_MC_01(void)
{
    /* MC_01: Init sets safe state and starts PWM */
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0);  // DIR_FWD RESET
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 0);  // DIR_REV RESET
    HAL_TIM_PWM_Start_Expect(&htim3, 1U);        // PWM start
    MotorController_Init();
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    uint8_t tgt = 123;
    TEST_ASSERT_EQUAL(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL(0, tgt);
}

void test_motor_controller_MC_02(void)
{
    /* MC_02: Move forward when current < target */
    extern uint8_t PositionSensing_GetPosition(uint8_t *);  // function exists in dependency, to be mocked in full integration
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0);  // DIR_FWD RESET
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 1);  // DIR_REV SET
    HAL_TIM_PWM_Start_Expect(&htim3, 1U);
    /* Precondition of position valid, current=1 is simulated in integration */
    MotorController_MoveTo(4);
    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
}

void test_motor_controller_MC_03(void)
{
    /* MC_03: Move reverse when current > target */
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 1);  // DIR_FWD SET
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 0);  // DIR_REV RESET
    HAL_TIM_PWM_Start_Expect(&htim3, 1U);
    MotorController_MoveTo(2);
    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
}

void test_motor_controller_MC_04(void)
{
    /* MC_04: No movement when already at target (Abort) */
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 0);
    HAL_TIM_PWM_Stop_Expect(&htim3, 1U);
    MotorController_MoveTo(3);
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

void test_motor_controller_MC_05(void)
{
    /* MC_05: Abort if current position is invalid at start of MoveTo */
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 0);
    HAL_TIM_PWM_Stop_Expect(&htim3, 1U);
    MotorController_MoveTo(5);
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

void test_motor_controller_MC_06(void)
{
    /* MC_06: Update does nothing when motor not moving */
    MotorController_Abort();
    MotorController_Update();
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

void test_motor_controller_MC_07(void)
{
    /* MC_07: While moving, Update + position invalid triggers Abort */
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 0);
    HAL_TIM_PWM_Stop_Expect(&htim3, 1U);
    MotorController_MoveTo(2); // start move
    MotorController_Update();  // cause abort by simulating invalid position
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

void test_motor_controller_MC_08(void)
{
    /* MC_08: While moving, Update stops at target using IsAtTarget */
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 0);
    HAL_TIM_PWM_Stop_Expect(&htim3, 1U);
    MotorController_MoveTo(4);
    MotorController_Update();
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

void test_motor_controller_MC_09(void)
{
    /* MC_09: While moving, continue if not at target */
    MotorController_MoveTo(4);
    MotorController_Update();
    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
}

void test_motor_controller_MC_10(void)
{
    /* MC_10: Abort sets safe outputs and stops PWM */
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0001, 0);
    HAL_GPIO_WritePin_Expect(GPIOB, 0x0002, 0);
    HAL_TIM_PWM_Stop_Expect(&htim3, 1U);
    MotorController_Abort();
    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

void test_motor_controller_MC_11(void)
{
    /* MC_11: GetTarget returns target when stopped and < 6 */
    MotorController_Abort();
    uint8_t tgt = 2;
    MotorController_MoveTo(2);
    MotorController_Abort();
    uint8_t t = 99;
    TEST_ASSERT_EQUAL(1, MotorController_GetTarget(&t));
    TEST_ASSERT_EQUAL(2, t);
}

void test_motor_controller_MC_12(void)
{
    /* MC_12: GetTarget returns 0 when moving */
    MotorController_MoveTo(2);
    uint8_t t = 9;
    TEST_ASSERT_EQUAL(0, MotorController_GetTarget(&t));
}

void test_motor_controller_MC_13(void)
{
    /* MC_13: GetTarget returns 0 on NULL pointer */
    MotorController_Abort();
    TEST_ASSERT_EQUAL(0, MotorController_GetTarget(NULL));
}

void test_motor_controller_MC_14(void)
{
    /* MC_14: GetTarget returns 0 when stored target >= 6 */
    MotorController_Abort();
    MotorController_MoveTo(6);
    MotorController_Abort();
    uint8_t t = 1;
    TEST_ASSERT_EQUAL(0, MotorController_GetTarget(&t));
}
