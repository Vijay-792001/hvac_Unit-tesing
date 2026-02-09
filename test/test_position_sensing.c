#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* PS_01: Init resets state (invalid+not valid) */
void test_PS_01_Init_resets_state(void)
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

/* PS_03: Update sets valid when ADC poll OK (maps to region 0) */
void test_PS_03_Update_sets_valid_poll_ok_region_0(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10U, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060U); // upper range, should map to 0
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());
    uint8_t pos=9;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(0, pos);
}

/* PS_04-PS_08: Map ADC to different positions */
void test_PS_04_Map_ADC_to_position_1_region(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10U, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800U);
    PositionSensing_Update();
    uint8_t pos=0;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(1, pos);
}
void test_PS_05_Map_ADC_to_position_2_region(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10U, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500U);
    PositionSensing_Update();
    uint8_t pos=0;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(2, pos);
}
void test_PS_06_Map_ADC_to_position_3_region(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10U, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000U);
    PositionSensing_Update();
    uint8_t pos=0;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}
void test_PS_07_Map_ADC_to_position_4_region(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10U, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500U);
    PositionSensing_Update();
    uint8_t pos=0;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(4, pos);
}
void test_PS_08_Map_ADC_to_position_5_region(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10U, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500U);
    PositionSensing_Update();
    uint8_t pos=0;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(5, pos);
}

/* PS_09: Update sets invalid when ADC poll fails */
void test_PS_09_Update_sets_invalid_on_poll_fail(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10U, HAL_ERROR);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
    uint8_t pos=42;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
}

/* PS_10: GetPosition returns last stored mapped value after update */
void test_PS_10_GetPosition_returns_last_stored_after_update(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10U, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000U);
    PositionSensing_Update();
    uint8_t pos=0;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}

/* PS_11: IsAtTarget rejects out-of-range target */
void test_PS_11_IsAtTarget_rejects_out_of_range(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(6));
}

/* PS_12: IsAtTarget returns 1 if inside window */
void test_PS_12_IsAtTarget_returns_1_inside_stop_range(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10U, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060U);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(0));
}

/* PS_13: IsAtTarget returns 0 when outside stop range */
void test_PS_13_IsAtTarget_returns_0_outside_stop_range(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10U, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040U);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(0));
}

/* PS_14: IsAtTarget works for window at 3 */
void test_PS_14_IsAtTarget_window_for_3(void)
{
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 10U, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3320U);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(3));
}
