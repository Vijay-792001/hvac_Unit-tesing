/* ===== test_status_indicator.c ===== */
#include "unity.h"
#include "status_indicator.h"

void setUp(void) {}
void tearDown(void) {}

void test_status_indicator_should_turn_on(void) {
    status_indicator_on();
    TEST_ASSERT_TRUE(status_indicator_is_on());
}
