#include "unity.h" #include "status_indicator.h" #include "mock_stm32f4xx_hal.h"

void setUp(void) {} void tearDown(void) {}

/* SI_01: Power LED turns ON after init / void test_status_indicator_SI_01(void) { / HAL_GPIO_Init must be called with the proper arguments, but details are not checked here */ HAL_GPIO_Init_Expect(LED_POWER_PORT, &GPIO_InitStruct);

/* Power LED ON */
HAL_GPIO_WritePin_Expect(LED_POWER_PORT, LED_POWER_PIN, GPIO_PIN_SET);
/* All position LEDs OFF */
HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_1, GPIO_PIN_RESET);
HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_2, GPIO_PIN_RESET);
HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_3, GPIO_PIN_RESET);
HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_4, GPIO_PIN_RESET);
HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_5, GPIO_PIN_RESET);

StatusIndicator_Init();

/* Strict order and call count asserted by CMock */
TEST_PASS();
}

/* SI_02: Position 0 shows no green LED / void test_status_indicator_SI_02(void) { / When position_valid=1, logical_position=0 → only RESET calls (no SET) */ HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_1, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_2, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_3, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_4, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_5, GPIO_PIN_RESET);

StatusIndicator_Update(1, 0);

TEST_PASS();
}

/* SI_03: Display position 5 / void test_status_indicator_SI_03(void) { / Reset all, then set correct LED */ HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_1, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_2, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_3, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_4, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_5, GPIO_PIN_RESET);

/* logical_position 5 → s_led_pos_pins[4] = GPIO_PIN_5 */
HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_5, GPIO_PIN_SET);

StatusIndicator_Update(1, 5);

TEST_PASS();
}

/* SI_04: Invalid position → all OFF / void test_status_indicator_SI_04(void) { / Only RESET (valid=0, pos irrelevant) */ HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_1, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_2, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_3, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_4, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_5, GPIO_PIN_RESET);

StatusIndicator_Update(0, 0xFF);

TEST_PASS();
}

/* SI_05: Power LED software control / void test_status_indicator_SI_05(void) { / Power LED OFF, then ON */ HAL_GPIO_WritePin_Expect(LED_POWER_PORT, LED_POWER_PIN, GPIO_PIN_RESET); StatusIndicator_SetPowerLED(0);

HAL_GPIO_WritePin_Expect(LED_POWER_PORT, LED_POWER_PIN, GPIO_PIN_SET);
StatusIndicator_SetPowerLED(1);

TEST_PASS();
}

/* SI_06: Boundary out-of-range (pos=6) → all OFF / void test_status_indicator_SI_06(void) { / Only RESET (valid=1, pos=6 > out of range) */ HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_1, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_2, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_3, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_4, GPIO_PIN_RESET); HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_5, GPIO_PIN_RESET);

StatusIndicator_Update(1, 6);

TEST_PASS();
}
