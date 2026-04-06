/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

ADC_HandleTypeDef hadc1;

void setUp(void) {}
void tearDown(void) {}

/* PS_01: Init resets internal state (invalid + not valid) */
void test_PS_01_Init_resets_internal_state_invalid_and_not_valid(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
    uint8_t pos = 123;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
}

/* PS_02: GetPosition rejects NULL pointer */
void test_PS_02_GetPosition_rejects_NULL_pointer(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(NULL));
}

/* PS_03: Update sets valid when ADC poll OK, GetValue=4060 (maps to pos=0) */
void test_PS_03_Update_sets_valid_when_ADC_poll_OK_GetValue_4060(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Init();
    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());
    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(0, pos);
}

/* PS_04: Map ADC to position 1 region, GetValue=3800 → pos=1 */
void test_PS_04_Map_ADC_to_position_1_region_3800(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(1, pos);
}

/* PS_05: Map ADC to position 2 region, GetValue=3500 → pos=2 */
void test_PS_05_Map_ADC_to_position_2_region_3500(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(2, pos);
}

/* PS_06: Map ADC to position 3 region, GetValue=3000 → pos=3 */
void test_PS_06_Map_ADC_to_position_3_region_3000(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}

/* PS_07: Map ADC to position 4 region, GetValue=1500 → pos=4 */
void test_PS_07_Map_ADC_to_position_4_region_1500(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(4, pos);
}

/* PS_08: Map ADC to position 5 region (lowest), GetValue=500 → pos=5 */
void test_PS_08_Map_ADC_to_position_5_region_500(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(5, pos);
}

/* PS_09: Update sets invalid when ADC poll fails */
void test_PS_09_Update_sets_invalid_when_ADC_poll_fails(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);

    PositionSensing_Init();
    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
    uint8_t pos = 0xBB;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
}

/* PS_10: GetPosition returns last stored mapped value after successful update */
void test_PS_10_GetPosition_returns_last_mapped_value_after_update(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t pos = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}

/* PS_11: IsAtTarget rejects out-of-range target (>=6) */
void test_PS_11_IsAtTarget_rejects_out_of_range_target(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(6U));
}

/* PS_12: IsAtTarget returns 1 when adc is inside stop range (target 0) */
void test_PS_12_IsAtTarget_returns_1_inside_stop_range_for_target_0(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Init();
    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(0U));
}

/* PS_13: IsAtTarget returns 0 when adc is outside stop range (target 0) */
void test_PS_13_IsAtTarget_returns_0_outside_stop_range_for_target_0(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040);

    PositionSensing_Init();
    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(0U));
}

/* PS_14: IsAtTarget works for another target stop window (target 3) */
void test_PS_14_IsAtTarget_returns_1_for_target_3_stop_range(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3320);

    PositionSensing_Init();
    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(3U));
}
