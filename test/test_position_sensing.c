/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

extern ADC_HandleTypeDef hadc1;

void setUp(void) {}
void tearDown(void) {}

/*
 * TestPlan: Position Sensing
 * PS_TC01: Init resets internal state
 * PS_TC02: GetPosition() rejects NULL pointer
 * PS_TC03: Update sets valid when ADC poll OK (ADC=4060)
 * PS_TC04: Map ADC 3800 to position 1
 * PS_TC05: Map ADC 3500 to position 2
 * PS_TC06: Map ADC 3000 to position 3
 * PS_TC07: Map ADC 1500 to position 4
 * PS_TC08: Map ADC 500 to position 5
 * PS_TC09: Update sets invalid when ADC poll fails
 * PS_TC10: GetPosition returns last stored mapped value after update
 * PS_TC11: IsAtTarget rejects out-of-range targets
 * PS_TC12: IsAtTarget returns 1 when adc is inside stop range
 * PS_TC13: IsAtTarget returns 0 when adc is outside stop range
 * PS_TC14: IsAtTarget works for stop window at position 3
 */

void test_PS_TC01_InitResetsInternalState(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL(0, PositionSensing_IsValid());
    uint8_t val = 0x33;
    TEST_ASSERT_EQUAL(0, PositionSensing_GetPosition(&val));
}

void test_PS_TC02_GetPositionRejectsNull(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL(0, PositionSensing_GetPosition(NULL));
}

void test_PS_TC03_UpdateSetsValidWhenADCPollOK(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Init();
    PositionSensing_Update();

    TEST_ASSERT_EQUAL(1, PositionSensing_IsValid());
    uint8_t val = 0x66;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&val));
    TEST_ASSERT_EQUAL(0, val);
}

void test_PS_TC04_MapADCToPosition1(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t val = 0x77;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&val));
    TEST_ASSERT_EQUAL(1, val);
}

void test_PS_TC05_MapADCToPosition2(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t val = 0x88;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&val));
    TEST_ASSERT_EQUAL(2, val);
}

void test_PS_TC06_MapADCToPosition3(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t val = 0x99;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&val));
    TEST_ASSERT_EQUAL(3, val);
}

void test_PS_TC07_MapADCToPosition4(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t val = 0x11;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&val));
    TEST_ASSERT_EQUAL(4, val);
}

void test_PS_TC08_MapADCToPosition5(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t val = 0x22;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&val));
    TEST_ASSERT_EQUAL(5, val);
}

void test_PS_TC09_UpdateSetsInvalidWhenADCPollFail(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);

    PositionSensing_Init();
    PositionSensing_Update();

    TEST_ASSERT_EQUAL(0, PositionSensing_IsValid());
    uint8_t val = 0x44;
    TEST_ASSERT_EQUAL(0, PositionSensing_GetPosition(&val));
}

void test_PS_TC10_GetPositionAfterUpdate(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t val = 0xBB;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&val));
    TEST_ASSERT_EQUAL(3, val);
}

void test_PS_TC11_IsAtTargetRejectsOutOfRange(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL(0, PositionSensing_IsAtTarget(6));
}

void test_PS_TC12_IsAtTarget_InsideStopRange(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL(1, PositionSensing_IsAtTarget(0));
}

void test_PS_TC13_IsAtTarget_OutsideStopRange(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040);

    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL(0, PositionSensing_IsAtTarget(0));
}

void test_PS_TC14_IsAtTarget_StopWindow_Position3(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3320);

    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL(1, PositionSensing_IsAtTarget(3));
}
