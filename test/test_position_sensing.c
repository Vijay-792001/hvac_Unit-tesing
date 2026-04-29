/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

extern ADC_HandleTypeDef hadc1;

void setUp(void) {}
void tearDown(void) {}

/* PS_01 */
void test_position_sensing_PS_01(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_IsValid());
    uint8_t pos;
    TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_GetPosition(&pos));
}

/* PS_02 */
void test_position_sensing_PS_02(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_GetPosition(NULL));
}

/* PS_03 */
void test_position_sensing_PS_03(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_IsValid());
    uint8_t pos;
    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(0U, pos);
}

/* PS_04 */
void test_position_sensing_PS_04(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);

    PositionSensing_Update();

    uint8_t pos;
    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(1U, pos);
}

/* PS_05 */
void test_position_sensing_PS_05(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);

    PositionSensing_Update();

    uint8_t pos;
    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(2U, pos);
}

/* PS_06 */
void test_position_sensing_PS_06(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Update();

    uint8_t pos;
    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3U, pos);
}

/* PS_07 */
void test_position_sensing_PS_07(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);

    PositionSensing_Update();

    uint8_t pos;
    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(4U, pos);
}

/* PS_08 */
void test_position_sensing_PS_08(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);

    PositionSensing_Update();

    uint8_t pos;
    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(5U, pos);
}

/* PS_09 */
void test_position_sensing_PS_09(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_IsValid());
    uint8_t pos;
    TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_GetPosition(&pos));
}

/* PS_10 */
void test_position_sensing_PS_10(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Update();

    uint8_t pos;
    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3U, pos);
}

/* PS_11 */
void test_position_sensing_PS_11(void)
{
    TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_IsAtTarget(6U));
}

/* PS_12 */
void test_position_sensing_PS_12(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_IsAtTarget(0U));
}

/* PS_13 */
void test_position_sensing_PS_13(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_IsAtTarget(0U));
}

/* PS_14 */
void test_position_sensing_PS_14(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3320);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_IsAtTarget(3U));
}