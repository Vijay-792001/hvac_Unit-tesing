#include "unity.h"
#include "command_handler.h"
#include "mock_motor_controller.h"
#include "mock_status_indicator.h"
#include "mock_position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void)
{
    // This is run before EACH TEST
}

void tearDown(void)
{
    // This is run after EACH TEST
}

void test_CommandHandler_ValidCommand_ExecutesMotorAndStatusIndicator(void)
{
    Command_t valid_cmd = CMD_MOVE_UP;
    MotorController_Move_ExpectAndReturn(valid_cmd, MOTOR_OK);
    StatusIndicator_Update_Expect(STATUS_BUSY);
    CommandHandler_Dispatch(&valid_cmd);
}

void test_CommandHandler_InvalidCommand_TriggersErrorStatus(void)
{
    Command_t invalid_cmd = CMD_INVALID;
    StatusIndicator_Update_Expect(STATUS_ERROR);
    CommandHandler_Dispatch(&invalid_cmd);
}

void test_CommandHandler_NullPointerCommand_SafeNoAction(void)
{
    CommandHandler_Dispatch(NULL);
}

void test_CommandHandler_MotorControllerFails_TriggersErrorStatus(void)
{
    Command_t cmd = CMD_MOVE_DOWN;
    MotorController_Move_ExpectAndReturn(cmd, MOTOR_ERR);
    StatusIndicator_Update_Expect(STATUS_ERROR);
    CommandHandler_Dispatch(&cmd);
}

void test_CommandHandler_ValidCommand_SetsIdleAfterCompletion(void)
{
    Command_t cmd = CMD_MOVE_HOME;
    MotorController_Move_ExpectAndReturn(cmd, MOTOR_OK);
    StatusIndicator_Update_Expect(STATUS_BUSY);
    StatusIndicator_Update_Expect(STATUS_IDLE);
    CommandHandler_Dispatch(&cmd);
}

void test_CommandHandler_PositionRequest_InvokesPositionSensing(void)
{
    Command_t cmd = CMD_GET_POSITION;
    int fake_position = 42;
    PositionSensing_Get_ExpectAndReturn(0, fake_position);
    StatusIndicator_Update_Expect(STATUS_POS_REPORTED);
    CommandHandler_Dispatch(&cmd);
}
