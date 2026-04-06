/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

ADC_HandleTypeDef hadc1;

void setUp(void) {}
void tearDown(void) {}

/* PS_01: Init resets internal state (invalid + not valid) */
void test_PS_01_Init_resets_internal_state(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
    uint8_t pos = 87;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
}

/* PS_02: PositionSensing_GetPosition() rejects NULL pointer */
void test_PS_02_GetPosition_rejects_NULL_pointer(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(NULL));
}

/* PS_03: Update sets valid when ADC poll OK, GetValue=4060 */
void test_PS_03_Update_sets_valid_when_ADC_poll_OK(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Init();
    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());
    uint8_t pos = 0xAB;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(0, pos); // 4060 -> position 0
}

void test_PS_04_Map_ADC_to_position_1_region(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);
    PositionSensing_Init();
    PositionSensing_Update();
    uint8_t pos = 0xFF;
    PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());
    TEST_ASSERT_EQUAL_UINT8(1, pos);
}

void test_PS_05_Map_ADC_to_position_2_region(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);
    PositionSensing_Init();
    PositionSensing_Update();
    uint8_t pos = 0xFF;
    PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL_UINT8(2, pos);
}

void test_PS_06_Map_ADC_to_position_3_region(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);
    PositionSensing_Init();
    PositionSensing_Update();
    uint8_t pos = 0xFF;
    PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}

void test_PS_07_Map_ADC_to_position_4_region(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);
    PositionSensing_Init();
    PositionSensing_Update();
    uint8_t pos = 0xFF;
    PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL_UINT8(4, pos);
}

void test_PS_08_Map_ADC_to_position_5_region(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);
    PositionSensing_Init();
    PositionSensing_Update();
    uint8_t pos = 0xFF;
    PositionSensing_GetPosition(&pos);
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
    uint8_t pos = 0xAC;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
}

/* PS_10: GetPosition returns last stored mapped value after successful update */
void test_PS_10_GetPosition_returns_last_stored_after_update(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t pos = 0xFA;
    uint8_t ret = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}

/* PS_11: IsAtTarget rejects out-of-range target (>=6) */
void test_PS_11_IsAtTarget_rejects_out_of_range_target(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(6));
}

/* PS_12: IsAtTarget returns 1 when adc is inside stop range */
void test_PS_12_IsAtTarget_inside_stop_range(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Init();
    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(0));
}

/* PS_13: IsAtTarget returns 0 when adc is outside stop range */
void test_PS_13_IsAtTarget_outside_stop_range(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040);

    PositionSensing_Init();
    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(0));
}

/* PS_14: IsAtTarget works for another target stop window */
void test_PS_14_IsAtTarget_works_for_another_target_window(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3320);

    PositionSensing_Init();
    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(3));
}
