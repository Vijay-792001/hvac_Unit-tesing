#include "unity.h"
#include "command_handler.h"
#include "mock_motor_controller.h"
#include "mock_position_sensing.h"
#include "mock_status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/* TC_CMD_001: Verify valid command dispatches to motor controller */
void test_CommandHandler_Dispatch_Forward_Command_Should_Call_MotorController_MoveForward_and_Return_OK(void)
{
    Command_t cmd = CMD_FORWARD;
    MotorController_MoveForward_ExpectAndReturn(MOTOR_OK);
    Status_t result = CommandHandler_Dispatch(&cmd);
    TEST_ASSERT_EQUAL(STATUS_OK, result);
}

/* TC_CMD_002: Verify invalid command returns error and does not call any controller */
void test_CommandHandler_Dispatch_Invalid_Command_Should_Return_ERROR_And_Not_Call_Other_Modules(void)
{
    Command_t cmd = CMD_INVALID;
    Status_t result = CommandHandler_Dispatch(&cmd);
    TEST_ASSERT_EQUAL(STATUS_ERROR, result);
}

/* TC_CMD_003: Verify NULL command returns STATUS_ERROR */
void test_CommandHandler_Dispatch_Null_Command_Should_Return_ERROR(void)
{
    Status_t result = CommandHandler_Dispatch(NULL);
    TEST_ASSERT_EQUAL(STATUS_ERROR, result);
}

/* TC_CMD_004: Verify backward command dispatches properly */
void test_CommandHandler_Dispatch_Backward_Command_Should_Call_MotorController_MoveBackward_and_Return_OK(void)
{
    Command_t cmd = CMD_BACKWARD;
    MotorController_MoveBackward_ExpectAndReturn(MOTOR_OK);
    Status_t result = CommandHandler_Dispatch(&cmd);
    TEST_ASSERT_EQUAL(STATUS_OK, result);
}

/* TC_CMD_005: Verify GetPosition command returns sensor value */
void test_CommandHandler_Dispatch_GetPosition_Command_Calls_PositionSensing_Get_and_Returns_OK(void)
{
    Command_t cmd = CMD_GET_POSITION;
    int dummy_position = 42;
    PositionSensing_Get_ExpectAndReturn(0, dummy_position);
    Status_t result = CommandHandler_Dispatch(&cmd);
    TEST_ASSERT_EQUAL(STATUS_OK, result);
}

/* TC_CMD_006: Verify command sets status indicator on command error */
void test_CommandHandler_Dispatch_ErrorFromMotorController_SetsStatusIndicator(void)
{
    Command_t cmd = CMD_FORWARD;
    MotorController_MoveForward_ExpectAndReturn(MOTOR_ERROR);
    StatusIndicator_Set_ExpectAndReturn(STATUS_ERROR, 0);
    Status_t result = CommandHandler_Dispatch(&cmd);
    TEST_ASSERT_EQUAL(STATUS_ERROR, result);
}

/* TC_CMD_007: Verify repeated valid command is handled and returns STATUS_OK */
void test_CommandHandler_Dispatch_TwiceWithForward_Should_Call_MotorController_Twice_and_Return_OK(void)
{
    Command_t cmd = CMD_FORWARD;
    MotorController_MoveForward_ExpectAndReturn(MOTOR_OK);
    Status_t result = CommandHandler_Dispatch(&cmd);
    TEST_ASSERT_EQUAL(STATUS_OK, result);
    MotorController_MoveForward_ExpectAndReturn(MOTOR_OK);
    result = CommandHandler_Dispatch(&cmd);
    TEST_ASSERT_EQUAL(STATUS_OK, result);
}

/* TC_CMD_008: Verify Status request calls StatusIndicator_Get */
void test_CommandHandler_Dispatch_StatusRequest_Calls_StatusIndicator_Get(void)
{
    Command_t cmd = CMD_GET_STATUS;
    StatusIndicator_Get_ExpectAndReturn(STATUS_OK);
    Status_t result = CommandHandler_Dispatch(&cmd);
    TEST_ASSERT_EQUAL(STATUS_OK, result);
}
