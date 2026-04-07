/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

extern ADC_HandleTypeDef hadc1;

// PS_01: Init resets internal state
void test_PS_01_Init_resets_internal_state(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL(0, PositionSensing_IsValid());
    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL(0, PositionSensing_GetPosition(&pos));
}

// PS_02: GetPosition rejects NULL pointer
void test_PS_02_GetPosition_NULL_pointer_should_return_0(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL(0, PositionSensing_GetPosition(NULL));
}

// PS_03: Update sets valid when ADC poll OK (GetValue = 4060 = pos 0)
void test_PS_03_Update_sets_valid_when_ADC_OK_value_4060_pos0(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);
    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL(1, PositionSensing_IsValid());
    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(0, pos);
}

// PS_04: Map ADC to position 1 region
void test_PS_04_Update_maps_ADC_to_position_1(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);
    PositionSensing_Init();
    PositionSensing_Update();
    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(1, pos);
}

// PS_05: Map ADC to position 2 region
void test_PS_05_Update_maps_ADC_to_position_2(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);
    PositionSensing_Init();
    PositionSensing_Update();
    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(2, pos);
}

// PS_06: Map ADC to position 3 region
void test_PS_06_Update_maps_ADC_to_position_3(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);
    PositionSensing_Init();
    PositionSensing_Update();
    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(3, pos);
}

// PS_07: Map ADC to position 4 region
void test_PS_07_Update_maps_ADC_to_position_4(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);
    PositionSensing_Init();
    PositionSensing_Update();
    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(4, pos);
}

// PS_08: Map ADC to position 5 region (lowest range)
void test_PS_08_Update_maps_ADC_to_position_5(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);
    PositionSensing_Init();
    PositionSensing_Update();
    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(5, pos);
}

// PS_09: Update sets invalid when ADC poll fails
void test_PS_09_Update_sets_invalid_when_ADC_fails(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);
    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL(0, PositionSensing_IsValid());
    uint8_t pos = 123;
    TEST_ASSERT_EQUAL(0, PositionSensing_GetPosition(&pos));
}

// PS_10: GetPosition returns last stored mapped value after update
void test_PS_10_GetPosition_returns_last_stored_value(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);
    PositionSensing_Init();
    PositionSensing_Update();
    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(3, pos);
}

// PS_11: IsAtTarget rejects out-of-range target >=6
void test_PS_11_IsAtTarget_rejects_out_of_range_target(void)
{
    TEST_ASSERT_EQUAL(0, PositionSensing_IsAtTarget(6));
    TEST_ASSERT_EQUAL(0, PositionSensing_IsAtTarget(10));
}

// PS_12: IsAtTarget returns 1 if ADC in stop window for target 0
void test_PS_12_IsAtTarget_returns_1_if_adc_in_stop_window_for_0(void)
{
    extern void set_ps_adc_value(uint16_t val);
    set_ps_adc_value(4060);
    TEST_ASSERT_EQUAL(1, PositionSensing_IsAtTarget(0));
}

// PS_13: IsAtTarget returns 0 if ADC outside stop window for target 0
void test_PS_13_IsAtTarget_returns_0_if_adc_outside_stop_for_0(void)
{
    extern void set_ps_adc_value(uint16_t val);
    set_ps_adc_value(4040);
    TEST_ASSERT_EQUAL(0, PositionSensing_IsAtTarget(0));
}

// PS_14: IsAtTarget works for another stop window (e.g., 3)
void test_PS_14_IsAtTarget_returns_1_in_stop_window_for_3(void)
{
    extern void set_ps_adc_value(uint16_t val);
    set_ps_adc_value(3320);
    TEST_ASSERT_EQUAL(1, PositionSensing_IsAtTarget(3));
}
