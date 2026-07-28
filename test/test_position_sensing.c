#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

ADC_HandleTypeDef hadc1;

void setUp(void) {}
void tearDown(void) {}

void test_position_sensing_PS_01(void)
{
    uint8_t pos = 7U;

    PositionSensing_Init();

    TEST_ASSERT_EQUAL_UINT8(0U, PositionSensing_IsValid());
}
