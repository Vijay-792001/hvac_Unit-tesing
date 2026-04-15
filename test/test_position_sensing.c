/* ===== test_position_sensing.c ===== */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_position_sensing_PS_01(void)
{
    uint8_t pos = 0xAB;
    PositionSensing_Init();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(0xAB, pos);
}

void test_position_sensing_PS_02(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(NULL));
}

void test_position_sensing_PS_03(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    uint8_t pos = 0xAA;
    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(0, pos);
}

void test_position_sensing_PS_04(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);

    uint8_t pos = 0xAA;
    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(1, pos);
}

void test_position_sensing_PS_05(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);

    uint8_t pos = 0xAA;
    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(2, pos);
}

void test_position_sensing_PS_06(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    uint8_t pos = 0xAA;
    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(3, pos);
}

void test_position_sensing_PS_07(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);

    uint8_t pos = 0xAA;
    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(4, pos);
}

void test_position_sensing_PS_08(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);

    uint8_t pos = 0xAA;
    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_GetPosition(&pos));
    TEST_ASSERT_EQUAL_UINT8(5, pos);
}

void test_position_sensing_PS_09(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);

    uint8_t pos = 0xEE;
    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_GetPosition(&pos));
}

void test_position_sensing_PS_10(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    uint8_t pos = 0xAB;
    PositionSensing_Init();
    PositionSensing_Update();
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
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(0));
}

void test_position_sensing_PS_13(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040);

    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsAtTarget(0));
}

void test_position_sensing_PS_14(void)
{
    HAL_ADC_Start_ExpectAndReturn(&hadc1, HAL_OK);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3320);

    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsAtTarget(3));
}
