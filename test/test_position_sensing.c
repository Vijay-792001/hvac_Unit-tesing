/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_position_sensing_PS_01(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
    uint8_t pos = 123;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(123, pos);
}

void test_position_sensing_PS_02(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(NULL));
}

void test_position_sensing_PS_03(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Init();
    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());
    uint8_t pos = 0xAA;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(0, pos);
}

void test_position_sensing_PS_04(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t pos = 100;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(1, pos);
}

void test_position_sensing_PS_05(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t pos = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(2, pos);
}

void test_position_sensing_PS_06(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t pos = 20;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}

void test_position_sensing_PS_07(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t pos = 0;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(4, pos);
}

void test_position_sensing_PS_08(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t pos = 0x0A;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(5, pos);
}

void test_position_sensing_PS_09(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);

    PositionSensing_Init();
    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
    uint8_t pos = 111;
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(111, pos);
}

void test_position_sensing_PS_10(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t pos = 0;
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}

void test_position_sensing_PS_11(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(6));
}

void test_position_sensing_PS_12(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(0));
}

void test_position_sensing_PS_13(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040);

    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(0));
}

void test_position_sensing_PS_14(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3320);

    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(3));
}
