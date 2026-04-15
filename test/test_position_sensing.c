/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* PS_01: Init resets internal state (invalid + not valid) */
void test_position_sensing_PS_01(void)
{
    /* When */
    PositionSensing_Init();

    /* Then */
    TEST_ASSERT_EQUAL(0U, PositionSensing_IsValid());
    uint8_t pos = 0xAB;
    TEST_ASSERT_EQUAL(0U, PositionSensing_GetPosition(&pos));
}

/* PS_02: GetPosition rejects NULL pointer */
void test_position_sensing_PS_02(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL(0U, PositionSensing_GetPosition(NULL));
}

/* PS_03: Update sets valid when ADC poll OK and GetPosition=0 */
void test_position_sensing_PS_03(void)
{
    PositionSensing_Init();
    extern ADC_HandleTypeDef hadc1;
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060U);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL(1U, PositionSensing_IsValid());
    uint8_t pos = 0xAB;
    TEST_ASSERT_EQUAL(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(0U, pos);
}

/* PS_04: Map ADC to position 1 region */
void test_position_sensing_PS_04(void)
{
    PositionSensing_Init();
    extern ADC_HandleTypeDef hadc1;
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800U);

    PositionSensing_Update();

    uint8_t pos = 98;
    TEST_ASSERT_EQUAL(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(1U, pos);
}

/* PS_05: Map ADC to position 2 region */
void test_position_sensing_PS_05(void)
{
    PositionSensing_Init();
    extern ADC_HandleTypeDef hadc1;
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500U);

    PositionSensing_Update();

    uint8_t pos = 99;
    TEST_ASSERT_EQUAL(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(2U, pos);
}

/* PS_06: Map ADC to position 3 region */
void test_position_sensing_PS_06(void)
{
    PositionSensing_Init();
    extern ADC_HandleTypeDef hadc1;
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000U);

    PositionSensing_Update();

    uint8_t pos = 111;
    TEST_ASSERT_EQUAL(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(3U, pos);
}

/* PS_07: Map ADC to position 4 region */
void test_position_sensing_PS_07(void)
{
    PositionSensing_Init();
    extern ADC_HandleTypeDef hadc1;
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500U);

    PositionSensing_Update();

    uint8_t pos = 87;
    TEST_ASSERT_EQUAL(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(4U, pos);
}

/* PS_08: Map ADC to position 5 region (lowest range) */
void test_position_sensing_PS_08(void)
{
    PositionSensing_Init();
    extern ADC_HandleTypeDef hadc1;
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500U);

    PositionSensing_Update();

    uint8_t pos = 123;
    TEST_ASSERT_EQUAL(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(5U, pos);
}

/* PS_09: Update sets invalid when ADC poll fails */
void test_position_sensing_PS_09(void)
{
    PositionSensing_Init();
    extern ADC_HandleTypeDef hadc1;
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL(0U, PositionSensing_IsValid());
    uint8_t pos = 42;
    TEST_ASSERT_EQUAL(0U, PositionSensing_GetPosition(&pos));
}

/* PS_10: GetPosition returns last stored mapped value after successful update */
void test_position_sensing_PS_10(void)
{
    PositionSensing_Init();
    extern ADC_HandleTypeDef hadc1;
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000U);

    PositionSensing_Update();

    uint8_t pos = 5;
    TEST_ASSERT_EQUAL(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(3U, pos);
}

/* PS_11: IsAtTarget rejects out-of-range target (>=6) */
void test_position_sensing_PS_11(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL(0U, PositionSensing_IsAtTarget(6));
}

/* PS_12: IsAtTarget returns 1 when adc in stop range for target 0 */
void test_position_sensing_PS_12(void)
{
    PositionSensing_Init();
    extern ADC_HandleTypeDef hadc1;
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060U);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL(1U, PositionSensing_IsAtTarget(0));
}

/* PS_13: IsAtTarget returns 0 when adc outside stop range for target 0 */
void test_position_sensing_PS_13(void)
{
    PositionSensing_Init();
    extern ADC_HandleTypeDef hadc1;
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040U);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL(0U, PositionSensing_IsAtTarget(0));
}

/* PS_14: IsAtTarget works for another target stop window */
void test_position_sensing_PS_14(void)
{
    PositionSensing_Init();
    extern ADC_HandleTypeDef hadc1;
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3320U);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL(1U, PositionSensing_IsAtTarget(3));
}
