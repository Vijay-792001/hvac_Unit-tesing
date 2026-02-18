/* ===== test_position_sensing.c ===== */

#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

// Required by the tested module (extern)
ADC_HandleTypeDef hadc1;

// Helper macros for clarity
#define SETUP_ADC_OK_RETURN(val)                      \
    HAL_ADC_Start_Expect(&hadc1);                     \
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK); \
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, (val))

#define SETUP_ADC_ERROR()                             \
    HAL_ADC_Start_Expect(&hadc1);                     \
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR)

void setUp(void)
{
    mock_stm32f4xx_hal_Init();
    PositionSensing_Init();
}

void tearDown(void)
{
    mock_stm32f4xx_hal_Verify();
    mock_stm32f4xx_hal_Destroy();
}

/* PS_01: Init resets internal state (invalid + not valid) */
void test_PS_01_Init_resets_internal_state_and_invalid(void)
{
    // After init, IsValid should be 0, GetPosition returns 0 and does not write output
    PositionSensing_Init();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());

    uint8_t pos = 123; // Dummy initial
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(123, pos); // Should remain unchanged if invalid
}

/* PS_02: GetPosition rejects NULL pointer */
void test_PS_02_GetPosition_NULL_pointer_returns_0(void)
{
    // IsValid default 0 after init, but test null always returns 0
    int ret = PositionSensing_GetPosition(NULL);
    TEST_ASSERT_EQUAL_UINT8(0, ret);
}

/* PS_03: Update sets valid when ADC poll OK, GetValue=4060, pos=0 */
void test_PS_03_Update_sets_valid_maps_4060_to_position_0(void)
{
    SETUP_ADC_OK_RETURN(4060U);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());

    uint8_t pos = 0xFF;
    int ret = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(0, pos); // 4060 > 4000 → 0
}

/* PS_04: Update maps ADC=3800 to pos=1 */
void test_PS_04_Update_ADC_3800_maps_to_position_1(void)
{
    SETUP_ADC_OK_RETURN(3800U);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());

    uint8_t pos = 0xFF;
    int ret = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(1, pos); // 3800 > 3750, <= 4000 → 1
}

/* PS_05: Update maps ADC=3500 to pos=2 */
void test_PS_05_Update_ADC_3500_maps_to_position_2(void)
{
    SETUP_ADC_OK_RETURN(3500U);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());

    uint8_t pos = 0xFF;
    int ret = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(2, pos); // 3500 > 3480, <= 3750 → 2
}

/* PS_06: Update maps ADC=3000 to pos=3 */
void test_PS_06_Update_ADC_3000_maps_to_position_3(void)
{
    SETUP_ADC_OK_RETURN(3000U);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());

    uint8_t pos = 0xFF;
    int ret = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(3, pos); // 3000 > 2500, <= 3480 → 3
}

/* PS_07: Update maps ADC=1500 to pos=4 */
void test_PS_07_Update_ADC_1500_maps_to_position_4(void)
{
    SETUP_ADC_OK_RETURN(1500U);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());

    uint8_t pos = 0xFF;
    int ret = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(4, pos); // 1500 > 1000, <= 2500 → 4
}

/* PS_08: Update maps ADC=500 to pos=5 (lowest) */
void test_PS_08_Update_ADC_500_maps_to_position_5(void)
{
    SETUP_ADC_OK_RETURN(500U);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());

    uint8_t pos = 0xFF;
    int ret = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(5, pos); // 500 <= 1000 → 5
}

/* PS_09: Update sets invalid when ADC poll fails */
void test_PS_09_Update_sets_invalid_on_HAL_ERROR(void)
{
    SETUP_ADC_ERROR();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());

    uint8_t pos = 0x55;
    int ret = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL_UINT8(0, ret); // position invalid
    TEST_ASSERT_EQUAL_UINT8(0x55, pos); // should remain unchanged
}

/* PS_10: GetPosition returns last stored mapped value after successful update */
void test_PS_10_GetPosition_returns_last_mapped_after_successful_update(void)
{
    SETUP_ADC_OK_RETURN(3000U);
    PositionSensing_Update();

    uint8_t pos = 0x99;
    int ret = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(3, pos);

    // Should persist until invalidated:
    // Invalidate
    SETUP_ADC_ERROR();
    PositionSensing_Update();

    pos = 0xAA;
    ret = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0xAA, pos); // Not updated
}

/* PS_11: IsAtTarget rejects out-of-range target (>=6) */
void test_PS_11_IsAtTarget_rejects_out_of_range_target(void)
{
    // No side effect, IsAtTarget returns 0 for targets >= 6
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(6));
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(10));
}

/* PS_12: IsAtTarget returns 1 when adc is within stop window for target=0 */
void test_PS_12_IsAtTarget_returns_1_in_stop_window_for_target0(void)
{
    // Drive state: ensure s_adc_value=4060
    SETUP_ADC_OK_RETURN(4060U);
    PositionSensing_Update();
    // Window for target 0: 4055–4065
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(0));
}

/* PS_13: IsAtTarget returns 0 if adc outside window for target=0 (e.g., 4040) */
void test_PS_13_IsAtTarget_returns_0_when_adc_outside_stop_window(void)
{
    // Drive state: s_adc_value = 4040, outside 4055–4065
    SETUP_ADC_OK_RETURN(4040U);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(0));
}

/* PS_14: IsAtTarget matches for other valid stop windows, e.g. target=3, value 3320 */
void test_PS_14_IsAtTarget_returns_1_in_stop_window_for_target3(void)
{
    // Window for target 3: 3309–3329; use 3320
    SETUP_ADC_OK_RETURN(3320U);
    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(3));
}

/* ===== end of test_position_sensing.c ===== */
