/* ===== test_position_sensing.c ===== */

#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

ADC_HandleTypeDef hadc1;

void setUp(void)
{
    PositionSensing_Init();
}
void tearDown(void)
{
}

void test_PS_01_Init_resets_internal_state(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
    uint8_t pos = 0xA5;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(0xA5, pos);
}
void test_PS_02_GetPosition_NULL_pointer(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(NULL));
}
void test_PS_03_Update_valid_sets_flag_maps_adc_to_0(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());
    uint8_t pos = 0x00;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(0, pos);
}
void test_PS_04_Update_maps_adc_to_position_1(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);
    PositionSensing_Update();
    uint8_t pos = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(1, pos);
}
void test_PS_05_Update_maps_adc_to_position_2(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);
    PositionSensing_Update();
    uint8_t pos = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(2, pos);
}
void test_PS_06_Update_maps_adc_to_position_3(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);
    PositionSensing_Update();
    uint8_t pos = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}
void test_PS_07_Update_maps_adc_to_position_4(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);
    PositionSensing_Update();
    uint8_t pos = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(4, pos);
}
void test_PS_08_Update_maps_adc_to_position_5(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);
    PositionSensing_Update();
    uint8_t pos = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(5, pos);
}
void test_PS_09_Update_sets_invalid_when_PollForConversion_fails(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
    uint8_t pos = 0xBB;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(0xBB, pos);
}
void test_PS_10_GetPosition_returns_last_mapped_value_after_update(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);
    PositionSensing_Update();
    uint8_t pos = 0x42;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}
void test_PS_11_IsAtTarget_rejects_out_of_range_targets(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(6));
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(42));
}
void test_PS_12_IsAtTarget_returns_1_when_adc_within_stop_range_target_0(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(0));
}
void test_PS_13_IsAtTarget_returns_0_when_adc_outside_stop_range_target_0(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(0));
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4066);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(0));
}
void test_PS_14_IsAtTarget_returns_1_when_adc_within_stop_range_target_3(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3310);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(3));
}
void test_PS_12b_IsAtTarget_returns_1_when_adc_at_stop_range_min_or_max(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3837);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(1));
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3857);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(1));
}
