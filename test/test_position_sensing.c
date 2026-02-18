/* ===== test_position_sensing.c ===== */

#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

// Provide a dummy global ADC handle for linkage
ADC_HandleTypeDef hadc1;

// Helper: Reset state before each test (PositionSensing_Init does this)
void setUp(void)
{
    PositionSensing_Init();
}

void tearDown(void)
{
}

/* PS_01: Init resets internal state (invalid + not valid)
   Check: PositionSensing_IsValid() = 0, PositionSensing_GetPosition() = 0 (any output pointer is untouched). */
void test_PS_01_Init_resets_internal_state_invalid_and_not_valid(void)
{
    // Already called in setUp
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());

    uint8_t out = 77;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&out));
    TEST_ASSERT_EQUAL_UINT8(77, out); // not changed if not valid
}

/* PS_02: PositionSensing_GetPosition() rejects NULL pointer.
   Return = 0 if NULL pointer. */
void test_PS_02_GetPosition_rejects_NULL_pointer_returns_0(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(NULL));
}

/* PS_03: Update sets valid when ADC poll OK (HAL_ADC_PollForConversion returns OK).
   HAL_ADC_GetValue = 4060, this maps to pos = 0 zone. Should set valid, pos = 0 region. */
void test_PS_03_Update_sets_valid_when_ADC_poll_OK_and_value_4060_maps_0(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());

    uint8_t out = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&out));
    TEST_ASSERT_EQUAL_UINT8(0, out); // 4060  pos 0 region
}

/* PS_04: Map ADC to position 1 region (3800+).
   HAL_ADC_GetValue = 3800, should map pos = 1. */
void test_PS_04_Update_ADC_value_3800_maps_to_position_1(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);

    PositionSensing_Update();

    uint8_t out = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&out));
    TEST_ASSERT_EQUAL_UINT8(1, out); // 3800  pos 1
}

/* PS_05: Map ADC to position 2 region (3500+)
   HAL_ADC_GetValue = 3500 (should map to 2) */
void test_PS_05_Update_ADC_value_3500_maps_to_position_2(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);

    PositionSensing_Update();

    uint8_t out = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&out));
    TEST_ASSERT_EQUAL_UINT8(2, out); // 3500  pos 2
}

/* PS_06: Map ADC to position 3 region (3000+)
   HAL_ADC_GetValue = 3000 (should map to 3) */
void test_PS_06_Update_ADC_value_3000_maps_to_position_3(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Update();

    uint8_t out = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&out));
    TEST_ASSERT_EQUAL_UINT8(3, out); // 3000  pos 3
}

/* PS_07: Map ADC to position 4 region (1500+)
   HAL_ADC_GetValue = 1500 (should map to 4) */
void test_PS_07_Update_ADC_value_1500_maps_to_position_4(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);

    PositionSensing_Update();

    uint8_t out = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&out));
    TEST_ASSERT_EQUAL_UINT8(4, out); // 1500  pos 4
}

/* PS_08: Map ADC to position 5 region (lowest range)
   HAL_ADC_GetValue = 500 (should map to 5) */
void test_PS_08_Update_ADC_value_500_maps_to_position_5(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);

    PositionSensing_Update();

    uint8_t out = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&out));
    TEST_ASSERT_EQUAL_UINT8(5, out); // 500  pos 5
}

/* PS_09: Update sets invalid when ADC poll fails (HAL_ERROR)
   IsValid=0, GetPosition=0 */
void test_PS_09_Update_sets_invalid_when_ADC_poll_fails(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());

    uint8_t out = 0xBB;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&out));
    TEST_ASSERT_EQUAL_UINT8(0xBB, out); // not written when invalid
}

/* PS_10: GetPosition returns last stored mapped value after successful update 
   HAL_ADC_GetValue = 3000  pos 3, then GetPosition gives 3 */
void test_PS_10_GetPosition_returns_last_stored_value_after_update(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Update();

    uint8_t out = 0x22;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&out));
    TEST_ASSERT_EQUAL_UINT8(3, out);
}

/* PS_11: IsAtTarget rejects out-of-range target (>=6) returns 0 */
void test_PS_11_IsAtTarget_rejects_out_of_range_target(void)
{
    // Any call, no HAL dependency.
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(6));
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(200));
}

/* PS_12: IsAtTarget=1 when adc is inside stop range (target 0, ADC in [4055,4065]) */
void test_PS_12_IsAtTarget_returns_1_when_adc_in_stop_range_target_0(void)
{
    // Simulate update with HAL_ADC_GetValue in stop range for target 0
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060); // in [4055,4065]

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(0));
}

/* PS_13: IsAtTarget=0 when adc is outside stop range (e.g., target 0, ADC 4040) */
void test_PS_13_IsAtTarget_returns_0_when_adc_outside_stop_range_target_0(void)
{
    // Simulate update with HAL_ADC_GetValue outside stop range for target 0
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040); // Below min for target 0

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(0));
}

/* PS_14: IsAtTarget works for another target stop window (target 3, ADC in [3309,3329]) */
void test_PS_14_IsAtTarget_returns_1_for_target_3_when_adc_in_stop_range(void)
{
    // ADC in stop range for target 3
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3320); // In [3309,3329] for pos 3

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(3));
}
