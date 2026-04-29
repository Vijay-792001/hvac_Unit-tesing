/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_position_sensing.h"

void setUp(void) {}
void tearDown(void) {}

void test_motor_controller_should_start_motor(void) {
    motor_controller_start();
    TEST_ASSERT_TRUE(motor_controller_is_running());
}
