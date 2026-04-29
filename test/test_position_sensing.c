/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"

void setUp(void) {}
void tearDown(void) {}

void test_position_sensing_should_return_correct_position(void) {
    int pos = position_sensing_get();
    TEST_ASSERT_EQUAL(0, pos);
}
