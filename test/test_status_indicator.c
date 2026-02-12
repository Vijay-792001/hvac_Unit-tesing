#include "unity.h"
#include "status_indicator.h"
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

// Test ID: TC_SI_001
void test_StatusIndicator_Init_ShouldInitLEDandBuzzer(void)
{
    HAL_LEDInit_Expect();
    HAL_BuzzerInit_Expect();

    StatusIndicator_Init();
}

// Test ID: TC_SI_002
void test_StatusIndicator_SetLED_ShouldTurnOnLEDOnValidCall(void)
{
    HAL_SetLED_ExpectAndReturn(HAL_LED_ON, HAL_OK);

    int result = StatusIndicator_SetLED(HAL_LED_ON);

    TEST_ASSERT_EQUAL(0, result);
}

// Test ID: TC_SI_003
void test_StatusIndicator_SetLED_ShouldHandleInvalidState(void)
{
    int result = StatusIndicator_SetLED(42);

    TEST_ASSERT_EQUAL(-1, result);
}

// Test ID: TC_SI_004
void test_StatusIndicator_SetLED_ShouldReturnErrorOnHALFailure(void)
{
    HAL_SetLED_ExpectAndReturn(HAL_LED_OFF, HAL_ERROR);

    int result = StatusIndicator_SetLED(HAL_LED_OFF);

    TEST_ASSERT_EQUAL(-2, result);
}

// Test ID: TC_SI_005
void test_StatusIndicator_Beep_ShouldBeepOnce(void)
{
    HAL_BuzzerBeep_ExpectAndReturn(HAL_BUZZER_BEEP_ONCE, HAL_OK);

    int result = StatusIndicator_Beep(HAL_BUZZER_BEEP_ONCE);

    TEST_ASSERT_EQUAL(0, result);
}

// Test ID: TC_SI_006
void test_StatusIndicator_Beep_ShouldRejectInvalidType(void)
{
    int result = StatusIndicator_Beep(99);

    TEST_ASSERT_EQUAL(-1, result);
}
