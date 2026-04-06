/* ===== test_position_sensing.c ===== */

#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

ADC_HandleTypeDef hadc1;

void setUp(void)
{
    // Reset state between tests if needed
    PositionSensing_Init();
}

void tearDown(void)
{
    // Nothing needed yet
}

/* PS_01 - Init resets internal state (invalid + not valid) */
void test_PS_01_Init_resets_internal_state_to_invalid(void)
{
    // When initialized, GetPosition returns 0 (invalid) and IsValid returns 0
    uint8_t pos = 0xAB;
    PositionSensing_Init();
    int getResult = PositionSensing_GetPosition(&pos);
    int validFlag = PositionSensing_IsValid();

    TEST_ASSERT_EQUAL(0, getResult);
    TEST_ASSERT_EQUAL(0xAB, pos); // output unchanged on error
    TEST_ASSERT_EQUAL(0, validFlag);
}

/* PS_02 - GetPosition rejects NULL pointer */
void test_PS_02_GetPosition_rejects_NULL_pointer(void)
{
    PositionSensing_Init();
    int ret = PositionSensing_GetPosition(NULL);
    TEST_ASSERT_EQUAL(0, ret);
}

/* PS_03 - Update sets valid when ADC poll OK; pos=0 (4060) */
void test_PS_03_Update_sets_valid_and_maps_stop0(void)
{
    // HAL expectations for Update (simulate Poll OK, value 4060)
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL(1, PositionSensing_IsValid());
    uint8_t pos = 0xAA;
    int ret = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL_UINT8(0, pos);
}

/* PS_04 - Map ADC to position 1 region (3800) */
void test_PS_04_Update_maps_adc_to_pos_1(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);

    PositionSensing_Update();

    uint8_t pos = 0xEE;
    int ret = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL_UINT8(1, pos);
}

/* PS_05 - Map ADC to position 2 region (3500) */
void test_PS_05_Update_maps_adc_to_pos_2(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);

    PositionSensing_Update();

    uint8_t pos = 0xEE;
    int ret = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL_UINT8(2, pos);
}

/* PS_06 - Map ADC to position 3 region (3000) */
void test_PS_06_Update_maps_adc_to_pos_3(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Update();

    uint8_t pos = 0xEE;
    int ret = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}

/* PS_07 - Map ADC to position 4 region (1500) */
void test_PS_07_Update_maps_adc_to_pos_4(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);

    PositionSensing_Update();

    uint8_t pos = 0xEE;
    int ret = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL_UINT8(4, pos);
}

/* PS_08 - Map ADC to position 5 region (lowest, e.g. 500) */
void test_PS_08_Update_maps_adc_to_pos_5(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);

    PositionSensing_Update();

    uint8_t pos = 0xEE;
    int ret = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL_UINT8(5, pos);
}

/* PS_09 - Update sets invalid when ADC poll fails */
void test_PS_09_Update_sets_invalid_on_adc_poll_fail(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL(0, PositionSensing_IsValid());
    uint8_t pos = 0xCC;
    int getret = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL(0, getret);
    TEST_ASSERT_EQUAL_UINT8(0xCC, pos); // Not changed
}

/* PS_10 - GetPosition returns last mapped value after update */
void test_PS_10_GetPosition_returns_last_stored_value_after_update(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Update();

    uint8_t pos = 0x77;
    int ret = PositionSensing_GetPosition(&pos);

    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}

/* PS_11 - IsAtTarget rejects out-of-range target (>=6) */
void test_PS_11_IsAtTarget_rejects_target_out_of_range(void)
{
    int result = PositionSensing_IsAtTarget(6);
    TEST_ASSERT_EQUAL(0, result);

    result = PositionSensing_IsAtTarget(99);
    TEST_ASSERT_EQUAL(0, result);
}

/* PS_12 - IsAtTarget returns 1 when adc is inside stop range */
void test_PS_12_IsAtTarget_returns_1_when_adc_in_stop0_range(void)
{
    // For stop 0: min = 4055, max = 4065
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Update();

    // Now 4060 is in stop 0 window
    int result = PositionSensing_IsAtTarget(0);
    TEST_ASSERT_EQUAL(1, result);
}

/* PS_13 - IsAtTarget returns 0 when adc is outside stop range */
void test_PS_13_IsAtTarget_returns_0_when_not_in_stop0_range(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040);

    PositionSensing_Update();

    int result = PositionSensing_IsAtTarget(0);
    TEST_ASSERT_EQUAL(0, result);
}

/* PS_14 - IsAtTarget works for another target stop window (e.g., stop3) */
void test_PS_14_IsAtTarget_for_stop3(void)
{
    // stop 3: min = 3309, max = 3329 (use e.g. 3320)
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3320);

    PositionSensing_Update();

    int result = PositionSensing_IsAtTarget(3);
    TEST_ASSERT_EQUAL(1, result);
}
