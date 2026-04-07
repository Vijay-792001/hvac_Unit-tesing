/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

extern ADC_HandleTypeDef hadc1;

void setUp(void) { }
void tearDown(void) { }

/* === PS_01 (SWE-REQ-040): Init resets internal state (invalid + not valid) === */
void test_PositionSensing_Init_ResetsInternalState(void)
{
    PositionSensing_Init();

    /* After init, position is invalid and not valid */
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());

    /* Call GetPosition with non-NULL, should return 0 and not modify */
    uint8_t p = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&p));
    TEST_ASSERT_EQUAL_UINT8(0xAA, p);
}

/* === PS_02 (SWE-REQ-019): GetPosition rejects NULL pointer === */
void test_PositionSensing_GetPosition_RejectsNullPointer(void)
{
    PositionSensing_Init();

    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(NULL));
}

/* === PS_03 (SWE-REQ-018): Update sets valid when ADC poll OK (pos=0, ADC=4060) === */
void test_PositionSensing_Update_SetsValid_WhenAdcPollOk_StopPosition0(void)
{
    PositionSensing_Init();

    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());

    uint8_t pos = 0xEE;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(0, pos);
}

/* === PS_04 (SWE-REQ-015): ADC maps to logical position 1 (ADC=3800) === */
void test_PositionSensing_Update_MapsToLogicalPosition1(void)
{
    PositionSensing_Init();

    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);

    PositionSensing_Update();

    uint8_t pos = 0xEE;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(1, pos);
}

/* === PS_05 (SWE-REQ-015): ADC maps to logical position 2 (ADC=3500) === */
void test_PositionSensing_Update_MapsToLogicalPosition2(void)
{
    PositionSensing_Init();

    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);

    PositionSensing_Update();

    uint8_t pos = 0xEE;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(2, pos);
}

/* === PS_06 (SWE-REQ-015): ADC maps to logical position 3 (ADC=3000) === */
void test_PositionSensing_Update_MapsToLogicalPosition3(void)
{
    PositionSensing_Init();

    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Update();

    uint8_t pos = 0xEE;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}

/* === PS_07 (SWE-REQ-015): ADC maps to logical position 4 (ADC=1500) === */
void test_PositionSensing_Update_MapsToLogicalPosition4(void)
{
    PositionSensing_Init();

    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);

    PositionSensing_Update();

    uint8_t pos = 0xEE;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(4, pos);
}

/* === PS_08 (SWE-REQ-015): ADC maps to logical position 5 (ADC=500) === */
void test_PositionSensing_Update_MapsToLogicalPosition5(void)
{
    PositionSensing_Init();

    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);

    PositionSensing_Update();

    uint8_t pos = 0xEE;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(5, pos);
}

/* === PS_09 (SWE-REQ-053): Update sets invalid when ADC poll fails === */
void test_PositionSensing_Update_SetsInvalid_WhenAdcPollFails(void)
{
    PositionSensing_Init();

    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);

    PositionSensing_Update();

    /* After failure, must be invalid and GetPosition returns 0 */
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());

    uint8_t p = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&p));
    TEST_ASSERT_EQUAL_UINT8(0xAA, p);
}

/* === PS_10 (SWE-REQ-019): GetPosition returns last stored mapped value === */
void test_PositionSensing_GetPosition_ReturnsLastStoredMappedValue(void)
{
    PositionSensing_Init();

    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Update();

    uint8_t pos = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}

/* === PS_11 (SWE-REQ-016): IsAtTarget rejects out-of-range target (>=6) === */
void test_PositionSensing_IsAtTarget_RejectsOutOfRangeTarget(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(6));
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(8));
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(0xFF));
}

/* === PS_12 (SWE-REQ-016): IsAtTarget returns 1 when ADC in stop window for target 0 === */
void test_PositionSensing_IsAtTarget_Returns1_WhenAdcInStopRangeForTarget0(void)
{
    PositionSensing_Init();

    // Perform update and simulate ADC value in window for target 0 (range: 4055..4065)
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Update();

    // Target 0 stop range: min=4055, max=4065, expect success
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(0));
}

/* === PS_13 (SWE-REQ-016): IsAtTarget returns 0 when ADC outside stop window for target 0 === */
void test_PositionSensing_IsAtTarget_Returns0_WhenAdcOutsideStopRangeForTarget0(void)
{
    PositionSensing_Init();

    // Simulate ADC value just outside stop range for 0 (4040)
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040);

    PositionSensing_Update();

    // Test for target 0
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(0));
}

/* === PS_14 (SWE-REQ-016): IsAtTarget returns 1 in window for target 3 === */
void test_PositionSensing_IsAtTarget_Returns1_InStopWindowForTarget3(void)
{
    PositionSensing_Init();

    // Target 3 stop window: min=3309 max=3329
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3320);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(3));
}
