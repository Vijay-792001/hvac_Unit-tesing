/***** ===== test_position_sensing.c ===== *****/

#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

/* Mock the external ADC handle */
ADC_HandleTypeDef hadc1;

void setUp(void)
{
    /* Always re-init to predictable state */
    PositionSensing_Init();
}

void tearDown(void)
{
}

/* ----------- PS_01 ----------- */
/* PS_01: Init resets internal state (invalid + not valid) */
void test_PS_01_Init_resets_internal_state(void)
{
    /* After init: valid should be 0; GetPosition should return 0 when invalid */
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());

    uint8_t pos = 0xAB;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
    /* pos should remain unchanged or set to FLAP_POSITION_INVALID per implementation: test plan requires unchanged */
}

/* ----------- PS_02 ----------- */
/* PS_02: PositionSensing_GetPosition() rejects NULL pointer */
void test_PS_02_GetPosition_rejects_NULL_pointer(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(NULL));
}

/* ----------- PS_03 ----------- */
/* PS_03: Update sets valid when ADC poll OK; GetValue=4060; expect valid=1, pos=0 */
void test_PS_03_Update_sets_valid_when_ADC_poll_OK(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());

    uint8_t pos = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(0, pos); /* ADC 4060 -> region 0 */
}

/* ----------- PS_04 ----------- */
/* PS_04: Map ADC to position 1 region (ADC 3800) */
void test_PS_04_ADC_mapping_to_position_1(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);

    PositionSensing_Update();

    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(1, pos);
}

/* ----------- PS_05 ----------- */
/* PS_05: Map ADC to position 2 region (ADC 3500) */
void test_PS_05_ADC_mapping_to_position_2(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);

    PositionSensing_Update();

    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(2, pos);
}

/* ----------- PS_06 ----------- */
/* PS_06: Map ADC to position 3 region (ADC 3000) */
void test_PS_06_ADC_mapping_to_position_3(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Update();

    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}

/* ----------- PS_07 ----------- */
/* PS_07: Map ADC to position 4 region (ADC 1500) */
void test_PS_07_ADC_mapping_to_position_4(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);

    PositionSensing_Update();

    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(4, pos);
}

/* ----------- PS_08 ----------- */
/* PS_08: Map ADC to position 5 region (lowest, ADC 500) */
void test_PS_08_ADC_mapping_to_position_5_lowest(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);

    PositionSensing_Update();

    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(5, pos);
}

/* ----------- PS_09 ----------- */
/* PS_09: Update sets invalid when ADC poll fails */
void test_PS_09_Update_sets_invalid_when_ADC_poll_fails(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());

    uint8_t pos = 0xBB;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos)); /* Should be invalid, do not change */
    /* Value should remain unchanged */
}

/* ----------- PS_10 ----------- */
/* PS_10: GetPosition returns last stored mapped value after successful update (ADC 3000->3) */
void test_PS_10_GetPosition_returns_last_stored_after_update(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Update();

    uint8_t pos = 0xCC;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos); /* Should match mapping */
}

/* ----------- PS_11 ----------- */
/* PS_11: IsAtTarget rejects out-of-range target (>=6) */
void test_PS_11_IsAtTarget_rejects_out_of_range_target(void)
{
    /* Should always return 0 regardless of ADC state */
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(6));
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(100));
}

/* ----------- PS_12/13/14 ----------- */
/* PS_12: IsAtTarget returns 1 when adc is inside stop range (target 0, ADC==4060) */
/* PS_13: IsAtTarget returns 0 when adc is outside stop range (target 0, ADC==4040) */
/* PS_14: IsAtTarget works for another stop window (target 3, ADC==3320) */

/* Helper to run update and "manually" set ADC value for s_adc_value (state) by triggering update with mocked GetValue */
static void set_adc_and_update(uint32_t value)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, value);
    PositionSensing_Update();
}

void test_PS_12_IsAtTarget_inside_stop_range(void)
{
    set_adc_and_update(4060); /* Should put s_adc_value in stop window for target 0 */
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(0));
}

void test_PS_13_IsAtTarget_outside_stop_range(void)
{
    set_adc_and_update(4040); /* Should not be inside min=4055, expect 0 */
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(0));
}

void test_PS_14_IsAtTarget_for_another_target_stop_window(void)
{
    set_adc_and_update(3320); /* For target 3, min=3309 max=3329 */
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(3));
}
