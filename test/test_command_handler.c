#include "unity.h"
#include "command_handler.h"
#include "mock_motor_controller.h"
#include "mock_position_sensing.h"
#include "mock_status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_CommandHandler_Processes_Valid_Move_Command(void)
{
    Command cmd = { .type = CMD_MOVE, .parameter = 10 };
    MotorController_Move_ExpectAndReturn(cmd.parameter, 0);
    StatusIndicator_Set_Expect(STATUS_BUSY);

    int result = CommandHandler_Process(&cmd);

    TEST_ASSERT_EQUAL(0, result);
}

void test_CommandHandler_Processes_Valid_Read_Position_Command(void)
{
    Command cmd = { .type = CMD_READ_POSITION, .parameter = 0 };
    PositionSensing_Read_ExpectAndReturn(123);

    int result = CommandHandler_Process(&cmd);

    TEST_ASSERT_EQUAL(123, result);
}

void test_CommandHandler_Processes_Valid_Status_Command(void)
{
    Command cmd = { .type = CMD_UPDATE_STATUS, .parameter = STATUS_OK };
    StatusIndicator_Set_Expect(cmd.parameter);

    int result = CommandHandler_Process(&cmd);

    TEST_ASSERT_EQUAL(0, result);
}

void test_CommandHandler_Handles_Invalid_Command(void)
{
    Command cmd = { .type = 0xFF, .parameter = 0 };

    int result = CommandHandler_Process(&cmd);

    TEST_ASSERT_EQUAL(-1, result);
}

void test_CommandHandler_Handles_Null_Cmd_Pointer(void)
{
    int result = CommandHandler_Process(NULL);

    TEST_ASSERT_EQUAL(-1, result);
}

void test_CommandHandler_Processes_Move_Command_Max_Parameter(void)
{
    Command cmd = { .type = CMD_MOVE, .parameter = 1000 };
    MotorController_Move_ExpectAndReturn(cmd.parameter, 0);
    StatusIndicator_Set_Expect(STATUS_BUSY);

    int result = CommandHandler_Process(&cmd);

    TEST_ASSERT_EQUAL(0, result);
}

void test_CommandHandler_Processes_Move_Command_Min_Parameter(void)
{
    Command cmd = { .type = CMD_MOVE, .parameter = 0 };
    MotorController_Move_ExpectAndReturn(cmd.parameter, 0);
    StatusIndicator_Set_Expect(STATUS_BUSY);

    int result = CommandHandler_Process(&cmd);

    TEST_ASSERT_EQUAL(0, result);
}

void test_CommandHandler_MotorController_Move_Error(void)
{
    Command cmd = { .type = CMD_MOVE, .parameter = 50 };
    MotorController_Move_ExpectAndReturn(cmd.parameter, -2);
    StatusIndicator_Set_Expect(STATUS_ERROR);

    int result = CommandHandler_Process(&cmd);

    TEST_ASSERT_EQUAL(-2, result);
}
