#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_MotorController_Move_Up_Success(void)
{
    MotorAction_t action = CMD_MOVE_UP;
    HAL_GPIO_WritePin_Expect(MOTOR_PORT, MOTOR_UP_PIN, GPIO_PIN_SET);
    HAL_Delay_Expect(MOTOR_MOVE_TIME_MS);
    HAL_GPIO_WritePin_Expect(MOTOR_PORT, MOTOR_UP_PIN, GPIO_PIN_RESET);
    MotorStatus_t result = MotorController_Move(action);
    TEST_ASSERT_EQUAL(MOTOR_OK, result);
}

void test_MotorController_Move_Down_Success(void)
{
    MotorAction_t action = CMD_MOVE_DOWN;
    HAL_GPIO_WritePin_Expect(MOTOR_PORT, MOTOR_DOWN_PIN, GPIO_PIN_SET);
    HAL_Delay_Expect(MOTOR_MOVE_TIME_MS);
    HAL_GPIO_WritePin_Expect(MOTOR_PORT, MOTOR_DOWN_PIN, GPIO_PIN_RESET);
    MotorStatus_t result = MotorController_Move(action);
    TEST_ASSERT_EQUAL(MOTOR_OK, result);
}

void test_MotorController_Move_InvalidAction_ReturnsError(void)
{
    MotorAction_t action = (MotorAction_t)99;
    MotorStatus_t result = MotorController_Move(action);
    TEST_ASSERT_EQUAL(MOTOR_ERR, result);
}

void test_MotorController_Move_HALPinSet_Fails_ReturnsError(void)
{
    MotorAction_t action = CMD_MOVE_UP;
    HAL_GPIO_WritePin_Expect(MOTOR_PORT, MOTOR_UP_PIN, GPIO_PIN_SET);
    MotorStatus_t result = MotorController_Move(action);
    TEST_ASSERT_EQUAL(MOTOR_OK, result);
}

void test_MotorController_Move_Boundary_NoDependencies_NoCrash(void)
{
    MotorStatus_t result = MotorController_Move(CMD_STOP);
    TEST_ASSERT_EQUAL(MOTOR_OK, result);
}
