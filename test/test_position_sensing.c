#include "unity.h"
#include "position_sensing.h"

/* Ceedling will auto-generate this mock */
#include "mock_stm32f4xx_hal.h"

/* hadc1 is defined in test/support/fake_adc_handle.c */
extern ADC_HandleTypeDef hadc1;

/* =========================
 * Test Lifecycle Hooks
 * ========================= */
void setUp(void) {}
void tearDown(void) {}


/* -------------------------
 * Helper: run one successful update cycle with a chosen ADC value
 * ------------------------- */
static void helper_Update_OK(uint32_t adc_value)
{
    /* 1) SUT should start ADC */
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);

    /* 2) SUT should poll conversion with timeout=2 */
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);

    /* 3) SUT should read the value */
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, adc_value);

    /* 4) Call the function under test */
    PositionSensing_Update();
}

/* -------------------------
 * Helper: run one failed update cycle (Poll fails)
 * ------------------------- */
static void helper_Update_PollFail(HAL_StatusTypeDef poll_status)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, poll_status);

    /* IMPORTANT:
       When Poll fails, your code does NOT call HAL_ADC_GetValue().
       So we DO NOT set any HAL_ADC_GetValue_Expect... here. */

    PositionSensing_Update();
}

/* =========================
 * PS_01
 * Init resets state: valid=0 and GetPosition should fail
 * ========================= */
void test_PositionSensing_Init_ResetsStateToInvalid(void)
{
    uint8_t pos = 0U;

    PositionSensing_Init();

    TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_IsValid());
    TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_GetPosition(&pos));
}

/* =========================
 * PS_02
 * GetPosition(NULL) must return 0 (null protection)
 * ========================= */
void test_PositionSensing_GetPosition_NullPointer_Returns0(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_GetPosition(NULL));
}

/* =========================
 * PS_03
 * Update OK sets valid=1 and maps ADC>4000 -> pos 0
 * ========================= */
void test_PositionSensing_Update_OK_SetsValid_AndMapsPos0(void)
{
    uint8_t pos = 0xFFU;

    PositionSensing_Init();
    helper_Update_OK(4060U); /* >4000 => pos 0 */

    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_IsValid());
    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(0U, pos);
}

/* =========================
 * PS_04
 * Mapping: ADC in (3750,4000] => pos 1
 * Example: 3800 -> 1
 * ========================= */
void test_PositionSensing_Map_Pos1(void)
{
    uint8_t pos = 0xFFU;

    PositionSensing_Init();
    helper_Update_OK(3800U);

    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(1U, pos);
}

/* =========================
 * PS_05
 * Mapping: ADC in (3480,3750] => pos 2
 * Example: 3500 -> 2
 * ========================= */
void test_PositionSensing_Map_Pos2(void)
{
    uint8_t pos = 0xFFU;

    PositionSensing_Init();
    helper_Update_OK(3500U);

    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(2U, pos);
}

/* =========================
 * PS_06
 * Mapping: ADC in (2500,3480] => pos 3
 * Example: 3000 -> 3
 * ========================= */
void test_PositionSensing_Map_Pos3(void)
{
    uint8_t pos = 0xFFU;

    PositionSensing_Init();
    helper_Update_OK(3000U);

    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3U, pos);
}

/* =========================
 * PS_07
 * Mapping: ADC in (1000,2500] => pos 4
 * Example: 1500 -> 4
 * ========================= */
void test_PositionSensing_Map_Pos4(void)
{
    uint8_t pos = 0xFFU;

    PositionSensing_Init();
    helper_Update_OK(1500U);

    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(4U, pos);
}

/* =========================
 * PS_08
 * Mapping: ADC <= 1000 => pos 5
 * Example: 500 -> 5
 * ========================= */
void test_PositionSensing_Map_Pos5(void)
{
    uint8_t pos = 0xFFU;

    PositionSensing_Init();
    helper_Update_OK(500U);

    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(5U, pos);
}

/* =========================
 * PS_09
 * Poll fail sets valid=0 and GetPosition should fail
 * ========================= */
void test_PositionSensing_Update_PollFail_SetsInvalid(void)
{
    uint8_t pos = 0U;

    PositionSensing_Init();
    helper_Update_PollFail(HAL_ERROR);

    TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_IsValid());
    TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_GetPosition(&pos));
}

/* =========================
 * PS_10
 * GetPosition returns last value after a valid update
 * ========================= */
void test_PositionSensing_GetPosition_ReturnsLastValue_WhenValid(void)
{
    uint8_t pos = 0xFFU;

    PositionSensing_Init();
    helper_Update_OK(3000U); /* pos 3 */

    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3U, pos);
}

/* =========================
 * PS_11
 * IsAtTarget: target>=6 returns 0 immediately
 * ========================= */
void test_PositionSensing_IsAtTarget_TargetOutOfRange_Returns0(void)
{
    PositionSensing_Init();

    TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_IsAtTarget(6U));
    
}

/* =========================
 * PS_12
 * IsAtTarget: target 0 range = 4055..4065
 * Use ADC=4060 => inside range => true
 * ========================= */
void test_PositionSensing_IsAtTarget_Target0_InsideRange_Returns1(void)
{
    PositionSensing_Init();
    helper_Update_OK(4060U);

    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_IsAtTarget(0U));
}

/* =========================
 * PS_13
 * IsAtTarget: target 0 outside range
 * Use ADC=4040 => outside => false
 * ========================= */
void test_PositionSensing_IsAtTarget_Target0_OutsideRange_Returns0(void)
{
    PositionSensing_Init();
    helper_Update_OK(4040U);

    TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_IsAtTarget(0U));
}

/* =========================
 * PS_14
 * IsAtTarget: target 3 range = 3309..3329
 * Use ADC=3320 => inside => true
 * ========================= */
void test_PositionSensing_IsAtTarget_Target3_InsideRange_Returns1(void)
{
    PositionSensing_Init();
    helper_Update_OK(3320U);

    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_IsAtTarget(3U));
}

