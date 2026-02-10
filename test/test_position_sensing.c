/* test_position_sensing.c */
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

ADC_HandleTypeDef hadc1;

void setUp(void)
{
    mock_stm32f4xx_hal_Init();
}

void tearDown(void)
{
    mock_stm32f4xx_hal_Verify();
    mock_stm32f4xx_hal_Destroy();
}

void test_PositionSensing_Init_ResetsState_PS01(void)
{
    PositionSensing_Init();
}

void test_PositionSensing_Update_ValidADC_PS02(void)
{
    uint16_t adc_value = 4100;
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, adc_value);

    PositionSensing_Update();

    uint8_t pos;
    uint8_t valid = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL_UINT8(1, valid);
    TEST_ASSERT_EQUAL_UINT8(0, pos);
    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());
}

void test_PositionSensing_Update_PollErrorInvalid_PS03(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_TIMEOUT);

    PositionSensing_Update();

    uint8_t pos;
    uint8_t valid = PositionSensing_GetPosition(&pos);
    TEST_ASSERT_EQUAL_UINT8(0, valid);
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
}

void test_PositionSensing_IsAtTarget_TargetInvalidRange_PS04(void)
{
    uint8_t ret = PositionSensing_IsAtTarget(7);
    TEST_ASSERT_EQUAL_UINT8(0, ret);
}

void test_PositionSensing_IsAtTarget_WithinRange_PS05(void)
{
    uint16_t adc_value = 4057;
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, adc_value);

    PositionSensing_Update();

    uint8_t ret = PositionSensing_IsAtTarget(0);
    TEST_ASSERT_EQUAL_UINT8(1, ret);
}

void test_PositionSensing_IsAtTarget_ADCOutsideStopRange_PS06(void)
{
    uint16_t adc_value = 3500;
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, adc_value);

    PositionSensing_Update();

    uint8_t ret = PositionSensing_IsAtTarget(0);
    TEST_ASSERT_EQUAL_UINT8(0, ret);
}

void test_PositionSensing_GetPosition_NullPointer_PS07(void)
{
    uint8_t ret = PositionSensing_GetPosition(NULL);
    TEST_ASSERT_EQUAL_UINT8(0, ret);
}

void test_PositionSensing_GetPosition_FailsIfInvalid_PS08(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_TIMEOUT);

    PositionSensing_Update();

    uint8_t tgt = 0xFF;
    uint8_t ret = PositionSensing_GetPosition(&tgt);
    TEST_ASSERT_EQUAL_UINT8(0, ret);
}

void test_PositionSensing_IsValid_ReturnsFlag_PS09(void)
{
    uint16_t adc_value = 3300;
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, adc_value);

    PositionSensing_Update();

    TEST_ASSERT_EQUAL_UINT8(1, PositionSensing_IsValid());
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);
    PositionSensing_Update();
    TEST_ASSERT_EQUAL_UINT8(0, PositionSensing_IsValid());
}
