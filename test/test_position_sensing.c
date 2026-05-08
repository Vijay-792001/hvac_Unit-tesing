/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_motor_controller.h"

void setUp(void) {}
void tearDown(void) {}

void test_PositionSensing_Should_UpdatePosition(void) {
    MotorController_GetPosition_ExpectAndReturn(10);
    int pos = PositionSensing_Update();
    TEST_ASSERT_EQUAL(10, pos);
}
