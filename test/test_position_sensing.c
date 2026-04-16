/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_position_sensing_PS_01(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL(0, PositionSensing_IsValid());
    uint8_t pos = 123;
    TEST_ASSERT_EQUAL(0, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(123, pos);
}

void test_position_sensing_PS_02(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL(0, PositionSensing_GetPosition(NULL));
}

void test_position_sensing_PS_03(void)
{
    PositionSensing_Init();
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL(1, PositionSensing_IsValid());
    uint8_t pos = 199;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(0, pos);
}

void test_position_sensing_PS_04(void)
{
    PositionSensing_Init();
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);
    PositionSensing_Update();
    uint8_t pos = 200;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(1, pos);
}

void test_position_sensing_PS_05(void)
{
    PositionSensing_Init();
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);
    PositionSensing_Update();
    uint8_t pos = 201;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(2, pos);
}

void test_position_sensing_PS_06(void)
{
    PositionSensing_Init();
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);
    PositionSensing_Update();
    uint8_t pos = 202;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(3, pos);
}

void test_position_sensing_PS_07(void)
{
    PositionSensing_Init();
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);
    PositionSensing_Update();
    uint8_t pos = 203;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(4, pos);
}

void test_position_sensing_PS_08(void)
{
    PositionSensing_Init();
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);
    PositionSensing_Update();
    uint8_t pos = 204;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(5, pos);
}

void test_position_sensing_PS_09(void)
{
    PositionSensing_Init();
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL(0, PositionSensing_IsValid());
    uint8_t pos = 205;
    TEST_ASSERT_EQUAL(0, PositionSensing_GetPosition(&pos));
}

void test_position_sensing_PS_10(void)
{
    PositionSensing_Init();
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);
    PositionSensing_Update();
    uint8_t pos = 206;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL(3, pos);
}

void test_position_sensing_PS_11(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL(0, PositionSensing_IsAtTarget(6));
}

void test_position_sensing_PS_12(void)
{
    PositionSensing_Init();
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL(1, PositionSensing_IsAtTarget(0));
}

void test_position_sensing_PS_13(void)
{
    PositionSensing_Init();
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL(0, PositionSensing_IsAtTarget(0));
}

void test_position_sensing_PS_14(void)
{
    PositionSensing_Init();
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3320);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL(1, PositionSensing_IsAtTarget(3));
}
