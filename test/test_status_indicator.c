#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/* SI_01: Power LED turns ON after init */
void test_status_indicator_SI_01(void) {
    HAL_GPIO_Init_Expect(LED_POWER_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin_Expect(LED_POWER_PORT, LED_POWER_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(LED_POS_PORT, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Init();
    TEST_PASS();
}

/* Additional SI_xx tests here... */