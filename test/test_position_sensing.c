/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"
ADC_HandleTypeDef hadc1;
void setUp(void) { PositionSensing_Init(); }
void tearDown(void) {}
void test_PS_01_Init_resets_internal_state_invalid_and_not_valid(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
    uint8_t pos = 123;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
}
void test_PS_02_GetPosition_rejects_NULL_pointer(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(NULL));
}
void test_PS_03_Update_sets_valid_when_ADC_poll_OK(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());
    uint8_t pos = 123;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(0, pos);
}
void test_PS_04_Map_ADC_to_position_1_region(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);
    PositionSensing_Update();
    uint8_t pos = 255;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(1, pos);
}
void test_PS_05_Map_ADC_to_position_2_region(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);
    PositionSensing_Update();
    uint8_t pos = 0;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(2, pos);
}
void test_PS_06_Map_ADC_to_position_3_region(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);
    PositionSensing_Update();
    uint8_t pos = 0;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}
void test_PS_07_Map_ADC_to_position_4_region(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);
    PositionSensing_Update();
    uint8_t pos = 0;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(4, pos);
}
void test_PS_08_Map_ADC_to_position_5_region(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);
    PositionSensing_Update();
    uint8_t pos = 0;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(5, pos);
}
void test_PS_09_Update_sets_invalid_when_poll_fails(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
    uint8_t pos = 111;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
}
void test_PS_10_GetPosition_returns_last_stored_after_update(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);
    PositionSensing_Update();
    uint8_t pos = 123;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}
void test_PS_11_IsAtTarget_rejects_target_greater_than_5(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(6));
}
void test_PS_12_IsAtTarget_true_for_exact_stop_window(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(0));
}
void test_PS_13_IsAtTarget_false_when_adc_outside_stop_window_0(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(0));
}
void test_PS_14_IsAtTarget_true_for_stop_window_3(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3320);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(3));
}
// End of test_position_sensing.c
