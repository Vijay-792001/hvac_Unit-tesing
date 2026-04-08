/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

extern ADC_HandleTypeDef hadc1;

void setUp(void) {}
void tearDown(void) {}

/* PS_01: Init resets internal state (invalid + not valid) */
void test_position_sensing_PS_01(void)
{
    PositionSensing_Init();

    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());

    uint8_t pos = 99;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(99, pos);
}

/* PS_02: PositionSensing_GetPosition() rejects NULL pointer */
void test_position_sensing_PS_02(void)
{
    PositionSensing_Init();

    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(NULL));
}

/* PS_03: Update sets valid when ADC poll OK */
void test_position_sensing_PS_03(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());

    uint8_t pos = 0xAA; // should be mapped position index 0
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(0, pos);
}

/* PS_04: Map ADC to position 1 region */
void test_position_sensing_PS_04(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);

    PositionSensing_Update();

    uint8_t pos = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(1, pos);
}

/* PS_05: Map ADC to position 2 region */
void test_position_sensing_PS_05(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);

    PositionSensing_Update();

    uint8_t pos = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(2, pos);
}

/* PS_06: Map ADC to position 3 region */
void test_position_sensing_PS_06(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Update();

    uint8_t pos = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}

/* PS_07: Map ADC to position 4 region */
void test_position_sensing_PS_07(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);

    PositionSensing_Update();

    uint8_t pos = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(4, pos);
}

/* PS_08: Map ADC to position 5 region (lowest range) */
void test_position_sensing_PS_08(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);

    PositionSensing_Update();

    uint8_t pos = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(5, pos);
}

/* PS_09: Update sets invalid when ADC poll fails */
void test_position_sensing_PS_09(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());

    uint8_t pos = 88;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
}

/* PS_10: GetPosition returns last stored mapped value after successful update */
void test_position_sensing_PS_10(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Update();

    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}

/* PS_11: IsAtTarget rejects out-of-range target (>=6) */
void test_position_sensing_PS_11(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(6));
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(7));
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(255));
}

/* PS_12: IsAtTarget returns 1 when adc is inside stop range */
void test_position_sensing_PS_12(void)
{
    // Setup for Update: place adc in stop range for target 0 (4055 to 4065)
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(0));
}

/* PS_13: IsAtTarget returns 0 when adc is outside stop range */
void test_position_sensing_PS_13(void)
{
    // Setup for Update: set adc just outside stop range for target 0
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(0));
}

/* PS_14: IsAtTarget works for another target stop window */
void test_position_sensing_PS_14(void)
{
    // Target 3 stop range {3309, 3329}
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3320);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(3));
}
