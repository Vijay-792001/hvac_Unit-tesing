#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* PS_01: Init resets internal state */
void test_position_sensing_PS_01(void) {
    uint8_t pos_val = 0xAB;
    PositionSensing_Init();
    TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_IsValid());
    TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_GetPosition(&pos_val));
    TEST_ASSERT_EQUAL_UINT8(0xAB, pos_val);
}

/* Additional PS_xx tests here... */