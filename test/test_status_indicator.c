/* ===== test_status_indicator.c ===== */
#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* SI_01: Power LED turns ON after init */
void test_status_indicator_SI_01(void)
{
    GPIO_InitTypeDef expected_init;
    expected_init.Pin = 0x0001 | 0x0002 | 0x0004 | 0x0008 | 0x0010 | 0x0020; // GPIO_PIN_0 | GPIO_PIN_1 | ... | GPIO_PIN_5
    expected_init.Mode = 0x00000001U;    // GPIO_MODE_OUTPUT_PP
    expected_init.Pull = 0x00000000U;    // GPIO_NOPULL
    expected_init.Speed = 0x00000001U;   // GPIO_SPEED_FREQ_LOW

    HAL_GPIO_Init_Expect(GPIOC, &expected_init);

    HAL_GPIO_WritePin_Expect(GPIOC, 0x0001, GPIO_PIN_SET); // Power LED ON

    HAL_GPIO_WritePin_Expect(GPIOC, 0x0002, GPIO_PIN_RESET); // Pos LED 1 OFF
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0004, GPIO_PIN_RESET); // Pos LED 2 OFF
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0008, GPIO_PIN_RESET); // Pos LED 3 OFF
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0010, GPIO_PIN_RESET); // Pos LED 4 OFF
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0020, GPIO_PIN_RESET); // Pos LED 5 OFF

    StatusIndicator_Init();

    // No explicit status return; test passes if calls/ordering are correct
    TEST_ASSERT_TRUE(1);
}

/* SI_02: Position 0 shows no green LED */
void test_status_indicator_SI_02(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0002, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0004, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0008, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0010, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0020, GPIO_PIN_RESET);

    StatusIndicator_Update(1, 0);

    TEST_ASSERT_TRUE(1);
}

/* SI_03: Display position 5 */
void test_status_indicator_SI_03(void)
{
    // First OFF all position LEDs
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0002, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0004, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0008, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0010, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0020, GPIO_PIN_RESET);

    // Only position 5 ON (logical_position == 5)
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0020, GPIO_PIN_SET);

    StatusIndicator_Update(1, 5);

    TEST_ASSERT_TRUE(1);
}

/* SI_04: Invalid position → all OFF */
void test_status_indicator_SI_04(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0002, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0004, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0008, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0010, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0020, GPIO_PIN_RESET);

    StatusIndicator_Update(0, 0xFF);

    TEST_ASSERT_TRUE(1);
}

/* SI_05: Power LED software control */
void test_status_indicator_SI_05(void)
{
    /* Turn OFF */
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0001, GPIO_PIN_RESET);
    StatusIndicator_SetPowerLED(0);
    /* Turn ON */
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0001, GPIO_PIN_SET);
    StatusIndicator_SetPowerLED(1);

    TEST_ASSERT_TRUE(1);
}

/* SI_06: Boundary out-of-range (pos=6) → all OFF */
void test_status_indicator_SI_06(void)
{
    // Position 6 is invalid: All LEDs OFF, no SET
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0002, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0004, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0008, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0010, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, 0x0020, GPIO_PIN_RESET);

    StatusIndicator_Update(1, 6);

    TEST_ASSERT_TRUE(1);
}
