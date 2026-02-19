/* ===== test_status_indicator.c ===== */
#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

// Provide instance for the GPIOC referenced in the module
GPIO_TypeDef GPIOC_inst;
GPIO_TypeDef *GPIOC = &GPIOC_inst;

// ----- setUp/tearDown -----
void setUp(void)
{
    // Nothing needed
}

void tearDown(void)
{
    // Nothing needed
}

/* ============= SI_01: SWE-REQ-025
   Power LED turns ON after init; all position LEDs OFF
   Power LED ON, all pos LEDs OFF: PIN_0 SET, PIN_1..5 RESET
*/
void test_SI_01_PowerLED_On_AllPositionLEDs_Off_OnInit(void)
{
    GPIO_InitTypeDef expected_cfg;

    // Expect HAL_GPIO_Init for all 6 pins on GPIOC
    HAL_GPIO_Init_Expect(GPIOC, &expected_cfg);

    // Power LED ON
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);

    // All position LEDs OFF (pins 1-5)
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Init();
}

/* ============= SI_02: SWE-REQ-026
   Position 0 shows no green LED (position LEDs OFF)
   Only RESET calls for GPIO_PIN_1..5; no SET calls
*/
void test_SI_02_Pos0_NoGreenLED_AllOff(void)
{
    // All position LEDs OFF
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Update(1, 0);
}

/* ============= SI_03: SWE-REQ-027
   Display position 5, only LED for pos 5 ON (after all reset)
   RESET all pins then SET GPIO_PIN_5 (for pos=5)
*/
void test_SI_03_DisplayPosition5_OnlyLED5_ON(void)
{
    // RESET all LEDs first
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    // Only LED for pos=5 ON, s_led_pos_pins[4] = GPIO_PIN_5
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);

    StatusIndicator_Update(1, 5);
}

/* ============= SI_04: SWE-REQ-029
   Invalid position -> all position LEDs OFF
   Only RESET calls for GPIO_PIN_1..5
*/
void test_SI_04_InvalidPosition_AllLEDsOFF(void)
{
    // All position LEDs OFF
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Update(0, 0xFF);
}

/* ============= SI_05: SWE-REQ-044
   Power LED software control: OFF then ON
   GPIO_PIN_0 RESET, then GPIO_PIN_0 SET
*/
void test_SI_05_PowerLED_SoftwareControl_OffThenOn(void)
{
    // Turn OFF
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    StatusIndicator_SetPowerLED(0);

    // Turn ON
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    StatusIndicator_SetPowerLED(1);
}

/* ============= SI_06: SWE-REQ-024
   Boundary out-of-range (pos=6): all OFF
   Only RESET calls for GPIO_PIN_1..5
*/
void test_SI_06_BoundaryOutOfRange_Pos6_AllOFF(void)
{
    // All position LEDs OFF
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Update(1, 6);
}
