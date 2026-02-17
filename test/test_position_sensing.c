#include "unity.h"
#include "position_sensing.h"
#include "mock_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_PositionSensing_ReadPosition_Normal_ShouldReturnExpectedValue(void)
{
    Sensor_t sensor = SENSOR_MAIN;
    int32_t raw_value = 12345;
    mock_HAL_ReadSensor_ExpectAndReturn(sensor, raw_value);
    int pos = PositionSensing_Read(sensor);
    TEST_ASSERT_EQUAL(raw_value, pos);
}

void test_PositionSensing_ReadPosition_HAL_Failure_ShouldReturnError(void)
{
    Sensor_t sensor = SENSOR_MAIN;
    mock_HAL_ReadSensor_ExpectAndReturn(sensor, -1);
    int pos = PositionSensing_Read(sensor);
    TEST_ASSERT_EQUAL(-1, pos);
}

void test_PositionSensing_ReadPosition_InvalidSensor_ShouldReturnError(void)
{
    Sensor_t sensor = (Sensor_t)77;
    int pos = PositionSensing_Read(sensor);
    TEST_ASSERT_EQUAL(-2, pos);
}

void test_PositionSensing_ReadPosition_NullPointer_ShouldReturnError(void)
{
    int ret = PositionSensing_ReadToPtr(NULL, SENSOR_MAIN);
    TEST_ASSERT_EQUAL(-3, ret);
}

void test_PositionSensing_ReadPosition_BoundarySensor_ShouldReturnExpectedValue(void)
{
    Sensor_t sensor = SENSOR_MAX_VALID;
    int32_t val = 5555;
    mock_HAL_ReadSensor_ExpectAndReturn(sensor, val);
    int pos = PositionSensing_Read(sensor);
    TEST_ASSERT_EQUAL(val, pos);
}
