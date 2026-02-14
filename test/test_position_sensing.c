#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_PositionSensing_Get_ReturnsValidWhenHALIsOK(void)
{
    int16_t pos = 123;
    HAL_StatusTypeDef hal_status = HAL_OK;
    HAL_ReadPosition_ExpectAnyArgsAndReturn(hal_status);
    HAL_ReadPosition_ReturnArrayThruPtr_position(&pos, 1);
    int16_t ret_pos;
    PositionStatus_t stat = PositionSensing_Get(&ret_pos);
    TEST_ASSERT_EQUAL(POSITION_STATUS_OK, stat);
    TEST_ASSERT_EQUAL(pos, ret_pos);
}

void test_PositionSensing_Get_ReturnsHAL_ERROR_OnFailure(void)
{
    int16_t dummy = 0;
    HAL_StatusTypeDef hal_status = HAL_ERROR;
    HAL_ReadPosition_ExpectAnyArgsAndReturn(hal_status);
    int16_t ret_pos = 0;
    PositionStatus_t stat = PositionSensing_Get(&ret_pos);
    TEST_ASSERT_EQUAL(POSITION_STATUS_HAL_ERROR, stat);
}

void test_PositionSensing_Get_NullPointer_ReturnsINVALID(void)
{
    PositionStatus_t stat = PositionSensing_Get(NULL);
    TEST_ASSERT_EQUAL(POSITION_STATUS_INVALID, stat);
}

void test_PositionSensing_Get_BoundaryMin(void)
{
    int16_t min = -32768;
    HAL_ReadPosition_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_ReadPosition_ReturnArrayThruPtr_position(&min, 1);
    int16_t ret_pos = 0;
    PositionStatus_t stat = PositionSensing_Get(&ret_pos);
    TEST_ASSERT_EQUAL(POSITION_STATUS_OK, stat);
    TEST_ASSERT_EQUAL(min, ret_pos);
}

void test_PositionSensing_Get_BoundaryMax(void)
{
    int16_t max = 32767;
    HAL_ReadPosition_ExpectAnyArgsAndReturn(HAL_OK);
    HAL_ReadPosition_ReturnArrayThruPtr_position(&max, 1);
    int16_t ret_pos = 0;
    PositionStatus_t stat = PositionSensing_Get(&ret_pos);
    TEST_ASSERT_EQUAL(POSITION_STATUS_OK, stat);
    TEST_ASSERT_EQUAL(max, ret_pos);
}
