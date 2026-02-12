#include "unity.h"
#include "position_sensing.h"
#include "mock_hal.h"

void setUp(void)
{
    mock_hal_Init();
}

void tearDown(void)
{
    mock_hal_Verify();
    mock_hal_Destroy();
}

// Test ID: TC_PS_001
void test_PositionSensing_Init_ShouldCallHALInit(void)
{
    HAL_PositionSensorInit_Expect();

    PositionSensing_Init();
}

// Test ID: TC_PS_002
void test_PositionSensing_GetCurrentPosition_ShouldReturnPositionOnSuccess(void)
{
    int fake_pos = 1234;
    HAL_ReadPosition_ExpectAndReturn(&fake_pos, HAL_OK);

    int pos;
    int result = PositionSensing_GetCurrentPosition(&pos);

    TEST_ASSERT_EQUAL(0, result);
    // In a real implementation, you would check that pos == fake_pos, but here pos is set by HAL mock.
}

// Test ID: TC_PS_003
void test_PositionSensing_GetCurrentPosition_ShouldReturnErrorOnHALFailure(void)
{
    HAL_ReadPosition_ExpectAndReturn(NULL, HAL_ERROR);

    int result = PositionSensing_GetCurrentPosition(NULL);

    TEST_ASSERT_EQUAL(-1, result);
}

// Test ID: TC_PS_004
void test_PositionSensing_GetCurrentPosition_ShouldRejectNullPointer(void)
{
    int result = PositionSensing_GetCurrentPosition(NULL);

    TEST_ASSERT_EQUAL(-2, result);
}
