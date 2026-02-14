#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

void test_StatusIndicator_Init_Should_ConfigureGPIO(void) {
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
    StatusIndicator_Init();
}

void test_StatusIndicator_SetStatus_Should_LightLED_OnStatusOn(void) {
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
    StatusIndicator_SetStatus(STATUS_ON);
}

void test_StatusIndicator_SetStatus_Should_TurnOffLED_OnStatusOff(void) {
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
    StatusIndicator_SetStatus(STATUS_OFF);
}

void test_StatusIndicator_SetStatus_Should_IgnoreInvalidStatus(void) {
    StatusIndicator_SetStatus(123);
}

void test_StatusIndicator_Deinit_Should_TurnOffLED(void) {
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
    StatusIndicator_Deinit();
}
