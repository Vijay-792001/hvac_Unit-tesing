/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

// Provide instance for the ADC handle referenced in the module
ADC_HandleTypeDef hadc1;

/* ========== Utility for internal reset ========== */
// No internal reset is strictly needed, as Init() resets all static state

void setUp(void)
{
    // Always call init before each test for isolation
    PositionSensing_Init();
}

void tearDown(void)
{
    // Nothing required
}

/* === PS_01: SWE-REQ-040
   Init resets internal state (invalid + not valid)
   PositionSensing_IsValid()=0, PositionSensing_GetPosition()=0
*/
void test_PS_01_Init_ResetsInternalState_Invalid(void)
{
    // Already called in setUp()
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
    uint8_t pos = 55;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
    // pos should remain unchanged since return is 0
    TEST_ASSERT_EQUAL_UINT8(55, pos);
}

/* === PS_02: SWE-REQ-019
   PositionSensing_GetPosition() rejects NULL pointer
   Return = 0
*/
void test_PS_02_GetPosition_RejectsNullPointer(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(NULL));
}

/* === PS_03: SWE-REQ-018
   Update sets valid when ADC poll OK, GetValue=4060
   PositionSensing_IsValid()=1, PositionSensing_GetPosition()=1, pos=0
*/
void test_PS_03_Update_SetsValid_WhenADCPollOK_Value4060(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());

    uint8_t pos = 99;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(0, pos);
}

/* === PS_04: SWE-REQ-015
   Map ADC to position 1 region (e.g., 3800)
*/
void test_PS_04_Update_MapsADCToPosition1(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);

    PositionSensing_Update();

    uint8_t pos = 55;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(1, pos);
}

/* === PS_05: SWE-REQ-015
   Map ADC to position 2 region (e.g., 3500)
*/
void test_PS_05_Update_MapsADCToPosition2(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);

    PositionSensing_Update();

    uint8_t pos = 66;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(2, pos);
}

/* === PS_06: SWE-REQ-015
   Map ADC to position 3 region (e.g., 3000)
*/
void test_PS_06_Update_MapsADCToPosition3(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Update();

    uint8_t pos = 99;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}

/* === PS_07: SWE-REQ-015
   Map ADC to position 4 region (e.g., 1500)
*/
void test_PS_07_Update_MapsADCToPosition4(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);

    PositionSensing_Update();

    uint8_t pos = 3;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(4, pos);
}

/* === PS_08: SWE-REQ-015
   Map ADC to position 5 region (e.g., 500)
*/
void test_PS_08_Update_MapsADCToPosition5(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);

    PositionSensing_Update();

    uint8_t pos = 12;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(5, pos);
}

/* === PS_09: SWE-REQ-053
   Update sets invalid when ADC poll fails
   IsValid=0, GetPosition=0
*/
void test_PS_09_Update_ADCPollFails_SetsInvalid(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
    uint8_t pos = 7;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(7, pos); // Unchanged
}

/* === PS_10: SWE-REQ-019
   GetPosition returns last mapped value after successful update
*/
void test_PS_10_GetPosition_ReturnsLastMapped_AfterSuccess(void)
{
    // First update to 3000 maps to pos=3
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Update();

    uint8_t pos = 1;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}

/* === PS_11: SWE-REQ-016
   IsAtTarget rejects out-of-range target (>=6); returns 0
*/
void test_PS_11_IsAtTarget_RejectsOutOfRangeTarget(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(6));
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(99));
}

/* === PS_12: SWE-REQ-016
   IsAtTarget returns 1 if ADC inside stop range (target=0, value=4060)
*/
void test_PS_12_IsAtTarget_Returns1_IfInsideStopWindow(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Update();
    // 4060 is inside the stop range for target 0
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(0));
}

/* === PS_13: SWE-REQ-016
   IsAtTarget returns 0 when ADC is outside stop window (target=0, value=4040)
*/
void test_PS_13_IsAtTarget_Returns0_IfOutsideStopWindow(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040);

    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(0));
}

/* === PS_14: SWE-REQ-016
   IsAtTarget works for another target stop window (target=3, value=3320)
*/
void test_PS_14_IsAtTarget_WithinStopWindowForOtherTarget(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3320);

    PositionSensing_Update();
    // 3320 is in stop range for target 3
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(3));
}
