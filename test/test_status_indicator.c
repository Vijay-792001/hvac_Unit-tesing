/* ===== test_status_indicator.c ===== */
#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"
// TEST PLAN: TC-SI-001 - Set status to running
void test_StatusIndicator_Set_Running_ShouldSetLEDsSuccess(void) {
    HAL_LED_Set_ExpectAndReturn(LED_RUNNING, 1, 0);
    HAL_LED_Set_ExpectAndReturn(LED_IDLE, 0, 0);
    HAL_LED_Set_ExpectAndReturn(LED_ERROR, 0, 0);
    int result = status_indicator_set(STATUS_RUNNING);
    TEST_ASSERT_EQUAL(0, result);
}
// TEST PLAN: TC-SI-002 - Set status to idle
void test_StatusIndicator_Set_Idle_ShouldSetLEDsSuccess(void) {
    HAL_LED_Set_ExpectAndReturn(LED_RUNNING, 0, 0);
    HAL_LED_Set_ExpectAndReturn(LED_IDLE, 1, 0);
    HAL_LED_Set_ExpectAndReturn(LED_ERROR, 0, 0);
    int result = status_indicator_set(STATUS_IDLE);
    TEST_ASSERT_EQUAL(0, result);
}
// TEST PLAN: TC-SI-003 - Set status to error
void test_StatusIndicator_Set_Error_ShouldSetLEDsSuccess(void) {
    HAL_LED_Set_ExpectAndReturn(LED_RUNNING, 0, 0);
    HAL_LED_Set_ExpectAndReturn(LED_IDLE, 0, 0);
    HAL_LED_Set_ExpectAndReturn(LED_ERROR, 1, 0);
    int result = status_indicator_set(STATUS_ERROR);
    TEST_ASSERT_EQUAL(0, result);
}
// TEST PLAN: TC-SI-004 - Set invalid status (should return error)
void test_StatusIndicator_Set_InvalidStatus_ShouldReturnError(void) {
    int result = status_indicator_set(99);
    TEST_ASSERT_EQUAL(SI_ERR_INVALID_STATUS, result);
}
// TEST PLAN: TC-SI-005 - HAL LED set fails
void test_StatusIndicator_Set_LEDSetFail_ShouldReturnError(void) {
    HAL_LED_Set_ExpectAndReturn(LED_RUNNING, 0, -1); // First call fails
    int result = status_indicator_set(STATUS_ERROR);
    TEST_ASSERT_EQUAL(SI_ERR_HAL, result);
}
