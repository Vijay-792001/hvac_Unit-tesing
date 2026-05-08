/* ===== test_status_indicator.c ===== */
#include "unity.h"
#include "status_indicator.h"

void setUp(void) {}
void tearDown(void) {}

void test_StatusIndicator_Should_SetRunning(void) {
    StatusIndicator_SetRunning();
    TEST_ASSERT_TRUE(StatusIndicator_IsRunning());
}

void test_StatusIndicator_Should_SetStopped(void) {
    StatusIndicator_SetStopped();
    TEST_ASSERT_FALSE(StatusIndicator_IsRunning());
}
