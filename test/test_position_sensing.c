#include "unity.h" #include "position_sensing.h" #include "mock_stm32f4xx_hal.h"

void setUp(void) {} void tearDown(void) {}

/* PS_01: Init resets internal state (invalid + not valid) / void test_position_sensing_PS_01(void) { / Pre: None */ uint8_t pos_val = 0xAB; PositionSensing_Init();

TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_IsValid());
TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_GetPosition(&pos_val));
/* pos_val must remain unchanged */
TEST_ASSERT_EQUAL_UINT8(0xAB, pos_val);
}

/* PS_02: GetPosition rejects NULL pointer */ void test_position_sensing_PS_02(void) { PositionSensing_Init();

TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_GetPosition(NULL));
}

/* PS_03: Update sets valid when ADC poll OK, GetValue=4060 → position 0 */ void test_position_sensing_PS_03(void) { uint8_t pos_val = 0xAB; PositionSensing_Init();

HAL_ADC_Start_Expect(&hadc1);
HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

PositionSensing_Update();

TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_IsValid());
TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos_val));
TEST_ASSERT_EQUAL_UINT8(0U, pos_val);
}

/* PS_04: Map ADC to position 1 region (GetValue=3800) */ void test_position_sensing_PS_04(void) { uint8_t pos_val = 0xAA; PositionSensing_Init();

HAL_ADC_Start_Expect(&hadc1);
HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);

PositionSensing_Update();

TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos_val));
TEST_ASSERT_EQUAL_UINT8(1U, pos_val);
}

/* PS_05: Map ADC to position 2 region (GetValue=3500) */ void test_position_sensing_PS_05(void) { uint8_t pos_val = 0xAA; PositionSensing_Init();

HAL_ADC_Start_Expect(&hadc1);
HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);

PositionSensing_Update();

TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos_val));
TEST_ASSERT_EQUAL_UINT8(2U, pos_val);
}

/* PS_06: Map ADC to position 3 region (GetValue=3000) */ void test_position_sensing_PS_06(void) { uint8_t pos_val = 0xAA; PositionSensing_Init();

HAL_ADC_Start_Expect(&hadc1);
HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

PositionSensing_Update();

TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos_val));
TEST_ASSERT_EQUAL_UINT8(3U, pos_val);
}

/* PS_07: Map ADC to position 4 region (GetValue=1500) */ void test_position_sensing_PS_07(void) { uint8_t pos_val = 0xAA; PositionSensing_Init();

HAL_ADC_Start_Expect(&hadc1);
HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);

PositionSensing_Update();

TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos_val));
TEST_ASSERT_EQUAL_UINT8(4U, pos_val);
}

/* PS_08: Map ADC to position 5 region (lowest, GetValue=500) */ void test_position_sensing_PS_08(void) { uint8_t pos_val = 0xAA; PositionSensing_Init();

HAL_ADC_Start_Expect(&hadc1);
HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);

PositionSensing_Update();

TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos_val));
TEST_ASSERT_EQUAL_UINT8(5U, pos_val);
}

/* PS_09: Update sets invalid when ADC poll fails (HAL_ERROR) */ void test_position_sensing_PS_09(void) { uint8_t pos_val = 0xAA; PositionSensing_Init();

HAL_ADC_Start_Expect(&hadc1);
HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);

PositionSensing_Update();

TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_IsValid());
TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_GetPosition(&pos_val));
}

/* PS_10: GetPosition returns last stored mapped value after successful update (GetValue=3000, pos=3) */ void test_position_sensing_PS_10(void) { uint8_t pos_val = 0xAA; PositionSensing_Init();

HAL_ADC_Start_Expect(&hadc1);
HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

PositionSensing_Update();

TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_GetPosition(&pos_val));
TEST_ASSERT_EQUAL_UINT8(3U, pos_val);
}

/* PS_11: IsAtTarget rejects out-of-range target (>=6) */ void test_position_sensing_PS_11(void) { PositionSensing_Init();

TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_IsAtTarget(6));
}

/* PS_12: IsAtTarget returns 1 when adc is inside stop range (target 0, adc=4060) */ void test_position_sensing_PS_12(void) { PositionSensing_Init();

HAL_ADC_Start_Expect(&hadc1);
HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

PositionSensing_Update();

TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_IsAtTarget(0));
}

/* PS_13: IsAtTarget returns 0 when adc outside stop range (target 0, adc=4040) */ void test_position_sensing_PS_13(void) { PositionSensing_Init();

HAL_ADC_Start_Expect(&hadc1);
HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040);

PositionSensing_Update();

TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_IsAtTarget(0));
}

/* PS_14: IsAtTarget works for another target stop window (target 3, adc=3320) */ void test_position_sensing_PS_14(void) { PositionSensing_Init();

HAL_ADC_Start_Expect(&hadc1);
HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3320);

PositionSensing_Update();

TEST_ASSERT_EQUAL_UINT8(1U, PositionSensing_IsAtTarget(3));
}
