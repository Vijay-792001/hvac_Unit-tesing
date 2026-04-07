/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

/* External handle per source */
ADC_HandleTypeDef hadc1;

/* -------- setUp and tearDown -------- */
void setUp(void) {
    mock_stm32f4xx_hal_Init();
    PositionSensing_Init(); // resets state for most tests
}

void tearDown(void) {
    mock_stm32f4xx_hal_Verify();
}

/* === PS_01: Init resets internal state (invalid + not valid) === */
void test_PositionSensing_Init_resets_internal_state_PS_01(void)
{
    // Set state dirty, then call Init
    PositionSensing_Init();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
    uint8_t pos = 0xA5;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(0xA5, pos); // pos param unchanged
}

/* === PS_02: GetPosition rejects NULL pointer === */
void test_PositionSensing_GetPosition_rejects_null_pointer_PS_02(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(NULL));
}

/* === PS_03: Update sets valid, pos=0 for ADC=4060 === */
void test_PositionSensing_Update_sets_valid_and_position_0_for_ADC_4060_PS_03(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());
    uint8_t pos = 99;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(0, pos);
}

/* === PS_04: Map ADC to position 1 region === */
void test_PositionSensing_Update_maps_ADC_to_position_1_PS_04(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);

    PositionSensing_Update();

    uint8_t pos = 99;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(1, pos);
}

/* === PS_05: Map ADC to position 2 region === */
void test_PositionSensing_Update_maps_ADC_to_position_2_PS_05(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);

    PositionSensing_Update();

    uint8_t pos = 99;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(2, pos);
}

/* === PS_06: Map ADC to position 3 region === */
void test_PositionSensing_Update_maps_ADC_to_position_3_PS_06(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Update();

    uint8_t pos = 99;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}

/* === PS_07: Map ADC to position 4 region === */
void test_PositionSensing_Update_maps_ADC_to_position_4_PS_07(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);

    PositionSensing_Update();

    uint8_t pos = 99;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(4, pos);
}

/* === PS_08: Map ADC to position 5 region === */
void test_PositionSensing_Update_maps_ADC_to_position_5_PS_08(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);

    PositionSensing_Update();

    uint8_t pos = 99;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(5, pos);
}

/* === PS_09: Update sets invalid when ADC poll fails === */
void test_PositionSensing_Update_sets_invalid_when_poll_fails_PS_09(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
    uint8_t pos = 55;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(55, pos); // unchanged
}

/* === PS_10: GetPosition returns last mapped value after successful update === */
void test_PositionSensing_GetPosition_returns_last_valid_value_PS_10(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Update();

    uint8_t pos = 0;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}

/* === PS_11: IsAtTarget rejects out-of-range target (>=6) === */
void test_PositionSensing_IsAtTarget_rejects_out_of_range_target_PS_11(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(6));
}

/* === PS_12: IsAtTarget returns 1 when ADC in stop range === */
void test_PositionSensing_IsAtTarget_returns_1_for_inrange_ADC_PS_12(void)
{
    // Simulate update with value 4060 (stop range[0]: min=4055 max=4065)
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(0));
}

/* === PS_13: IsAtTarget returns 0 when ADC outside stop window === */
void test_PositionSensing_IsAtTarget_returns_0_for_ADC_outside_stop_range_PS_13(void)
{
    // Out of stop window for target 0 (4055-4065), use 4040
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040);

    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(0));
}

/* === PS_14: IsAtTarget works for another window (target=3, value in range) === */
void test_PositionSensing_IsAtTarget_returns_1_when_in_stop_range_of_target3_PS_14(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3320); // target 3: min 3309 max 3329

    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(3));
}
