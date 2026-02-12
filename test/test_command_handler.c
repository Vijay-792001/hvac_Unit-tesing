#include "unity.h"
#include "command_handler.h"
#include "mock_hal.h"  // Replace 'hal' with the actual HAL dependency name if  it is different
#include <string.h>

void setUp(void)
{
    mock_hal_Init();
}

void tearDown(void)
{
    mock_hal_Verify();
    mock_hal_Destroy();
}

// Test ID: TC_CH_001
void test_CommandHandler_Init_ShouldInitializeAllInternalState(void)
{
    // Arrange
    HAL_Init_Expect();

    // Act
    CommandHandler_Init();

    // Assert
    // No explicit outputs; all verification is through mock expectations.
}

// Test ID: TC_CH_002
void test_CommandHandler_ProcessCommand_ShouldHandleValidCommand(void)
{
    // Arrange
    const char* valid_cmd = "SET_TEMP 22";
    HAL_SendCommand_ExpectAndReturn(valid_cmd, HAL_OK);

    // Act
    int result = CommandHandler_ProcessCommand(valid_cmd);

    // Assert
    TEST_ASSERT_EQUAL(0, result);
}

// Test ID: TC_CH_003
void test_CommandHandler_ProcessCommand_ShouldRejectNullCommand(void)
{
    // Act
    int result = CommandHandler_ProcessCommand(NULL);

    // Assert
    TEST_ASSERT_EQUAL(-1, result);
}

// Test ID: TC_CH_004
void test_CommandHandler_ProcessCommand_ShouldRejectUnsupportedCommand(void)
{
    // Arrange
    const char* invalid_cmd = "INVALID_CMD";

    // Act
    int result = CommandHandler_ProcessCommand(invalid_cmd);

    // Assert
    TEST_ASSERT_EQUAL(-2, result);
}

// Test ID: TC_CH_005
void test_CommandHandler_ProcessCommand_ShouldHandleHALFailure(void)
{
    // Arrange
    const char* valid_cmd = "SET_TEMP 22";
    HAL_SendCommand_ExpectAndReturn(valid_cmd, HAL_ERROR);

    // Act
    int result = CommandHandler_ProcessCommand(valid_cmd);

    // Assert
    TEST_ASSERT_EQUAL(-3, result);
}
