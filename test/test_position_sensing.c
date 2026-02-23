/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

/* Globals provided via source */
extern ADC_HandleTypeDef hadc1;

void setUp(void)
{
    PositionSensing_Init();
}

void tearDown(void)
{
}

/* PS_01: Init resets state to invalid, not valid */
void test_PS_01_Init_resets_state_to_invalid_and_not_valid(void)
{
    /* After Init: IsValid() = 0, GetPosition() = 0, pos_out not changed */
    uint8_t before = 0xA5;
    uint8_t pos_out = before;

    PositionSensing_Init();

    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos_out));
    TEST_ASSERT_EQUAL_UINT8(before, pos_out);
}

/* PS_02: GetPosition rejects NULL pointer */
void test_PS_02_GetPosition_rejects_NULL_pointer(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(NULL));
}

/* PS_03: Update sets valid when ADC poll OK; maps 4060 to position 0 */
void test_PS_03_Update_sets_valid_when_ADC_poll_OK(void)
{
    /* HAL sequence: Start, PollForConversion (OK), GetValue (4060) */
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());
    uint8_t pos_out = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos_out));
    TEST_ASSERT_EQUAL_UINT8(0, pos_out);
}

/* PS_04: Map ADC 3800 to position 1 */
void test_PS_04_Maps_ADC_3800_to_position_1(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);

    PositionSensing_Update();

    uint8_t pos_out = 0xCC;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos_out));
    TEST_ASSERT_EQUAL_UINT8(1, pos_out);
}

/* PS_05: Map ADC 3500 to position 2 */
void test_PS_05_Maps_ADC_3500_to_position_2(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);

    PositionSensing_Update();

    uint8_t pos_out = 0xBB;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos_out));
    TEST_ASSERT_EQUAL_UINT8(2, pos_out);
}

/* PS_06: Map ADC 3000 to position 3 */
void test_PS_06_Maps_ADC_3000_to_position_3(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Update();

    uint8_t pos_out = 0xDD;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos_out));
    TEST_ASSERT_EQUAL_UINT8(3, pos_out);
}

/* PS_07: Map ADC 1500 to position 4 */
void test_PS_07_Maps_ADC_1500_to_position_4(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);

    PositionSensing_Update();

    uint8_t pos_out = 0xED;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos_out));
    TEST_ASSERT_EQUAL_UINT8(4, pos_out);
}

/* PS_08: Map ADC 500 to position 5 (lowest range) */
void test_PS_08_Maps_ADC_500_to_position_5(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);

    PositionSensing_Update();

    uint8_t pos_out = 0x55;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos_out));
    TEST_ASSERT_EQUAL_UINT8(5, pos_out);
}

/* PS_09: Update sets invalid when ADC poll fails */
void test_PS_09_Update_sets_invalid_when_poll_fails(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
    uint8_t pos_out = 0xF1;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos_out));
    TEST_ASSERT_EQUAL_UINT8(0xF1, pos_out);
}

/* PS_10: GetPosition returns last mapped position after update */
void test_PS_10_GetPosition_returns_last_mapped_after_update(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000); // This maps to 3

    PositionSensing_Update();

    uint8_t pos_out = 0x99;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos_out));
    TEST_ASSERT_EQUAL_UINT8(3, pos_out);
}

/* PS_11: IsAtTarget rejects out-of-range target (>=6) */
void test_PS_11_IsAtTarget_rejects_out_of_range_target(void)
{
    /* Out of range is >= 6 */
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(6));
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(255)); // check FLAP_POSITION_INVALID
}

/* PS_12: IsAtTarget returns 1 when adc is inside stop range */
void test_PS_12_IsAtTarget_returns_1_when_adc_inside_stop_range(void)
{
    /* Update with 4060 (maps to stop_ranges[0] 4055-4065), and test IsAtTarget(0) */
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(0));
}

/* PS_13: IsAtTarget returns 0 when adc is outside stop range */
void test_PS_13_IsAtTarget_returns_0_when_adc_outside_stop_range(void)
{
    /* Update with 4040 (outside 4055-4065), IsAtTarget(0) should return 0 */
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(0));
}

/* PS_14: IsAtTarget works for another target stop window */
void test_PS_14_IsAtTarget_works_for_another_stop_window(void)
{
    /* Update with 3320 (maps to stop_ranges[3]: 3309-3329), IsAtTarget(3) should return 1 */
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3320);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(3));
}
