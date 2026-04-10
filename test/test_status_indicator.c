/* ===== test_status_indicator.c ===== */
#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* SI_01: SWE-REQ-025 Power LED turns ON after init */
void test_status_indicator_SI_01(void)
{
    GPIO_InitTypeDef any_struct; // For CMock struct passing
    HAL_GPIO_Init_Expect(GPIOC, &any_struct);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);    // Power LED ON
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);  // All position LEDs OFF
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Init();
    TEST_ASSERT_TRUE(1); // Executed if no failure in flow (sanity assert)
}

/* SI_02: SWE-REQ-026 Position 0 shows no green LED */
void test_status_indicator_SI_02(void)
{
    // All LEDs reset, no set for position 0
    for (uint8_t i = 1; i <= 5; ++i)
        HAL_GPIO_WritePin_Expect(GPIOC, (GPIO_PIN_0 << i), GPIO_PIN_RESET);

    StatusIndicator_Update(1, 0);
    TEST_ASSERT_TRUE(1);
}

/* SI_03: SWE-REQ-027 Display position 5 */
void test_status_indicator_SI_03(void)
{
    // RESET all, then SET only position 5
    for (uint8_t i = 1; i <= 5; ++i)
        HAL_GPIO_WritePin_Expect(GPIOC, (GPIO_PIN_0 << i), GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);

    StatusIndicator_Update(1, 5);
    TEST_ASSERT_TRUE(1);
}

/* SI_04: SWE-REQ-029 Invalid position → all OFF */
void test_status_indicator_SI_04(void)
{
    for (uint8_t i = 1; i <= 5; ++i)
        HAL_GPIO_WritePin_Expect(GPIOC, (GPIO_PIN_0 << i), GPIO_PIN_RESET);

    StatusIndicator_Update(0, 0xFF);
    TEST_ASSERT_TRUE(1);
}

/* SI_05: SWE-REQ-044 Power LED software control */
void test_status_indicator_SI_05(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    StatusIndicator_SetPowerLED(0);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    StatusIndicator_SetPowerLED(1);
    TEST_ASSERT_TRUE(1);
}

/* SI_06: SWE-REQ-024 Boundary out-of-range (pos=6) → all OFF */
void test_status_indicator_SI_06(void)
{
    for (uint8_t i = 1; i <= 5; ++i)
        HAL_GPIO_WritePin_Expect(GPIOC, (GPIO_PIN_0 << i), GPIO_PIN_RESET);

    StatusIndicator_Update(1, 6);
    TEST_ASSERT_TRUE(1);
}

