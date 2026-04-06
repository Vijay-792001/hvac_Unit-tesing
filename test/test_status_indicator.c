/***** ===== test_status_indicator.c ===== *****/

#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

/* ------ Required for strict symbol presence ------ */
GPIO_TypeDef GPIOC_inst;

/* Not strictly necessary for this module but defined for completeness of externals */
#define GPIOC (&GPIOC_inst)

void setUp(void)
{
    /* no persistent state */
}

void tearDown(void)
{
}

/*
 * SI_01: Power LED turns ON after init
 * - StatusIndicator_Init()
 * - Power LED ON, all position LEDs OFF
 * - HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, SET)
 * - HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, RESET) ... PIN_5 RESET
 */
void test_SI_01_Power_LED_ON_after_init(void)
{
    /* HAL_GPIO_Init is called, but we do not check this here (real register side effect). */
    HAL_GPIO_Init_Expect(GPIOC, NULL); /* GPIO_InitTypeDef * is local, NULL accepted */

    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Init();
}

/*
 * SI_02: Position 0 shows no green LED
 * - StatusIndicator_Update(position_valid=1, logical_position=0)
 * - All position LEDs OFF: Only RESET calls GPIO_PIN_1..GPIO_PIN_5, no SET
 */
void test_SI_02_Position0_shows_no_green_LED(void)
{
    /* All green LEDs off: RESET all position LEDs */
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Update(1, 0);
}

/*
 * SI_03: Display position 5
 * - StatusIndicator_Update(position_valid=1, logical_position=5)
 * - Only LED for position 5 ON: RESET all, then SET PIN5
 */
void test_SI_03_Display_position_5_only_LED_5_ON(void)
{
    /* First: reset all, then SET PIN_5 */
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);

    StatusIndicator_Update(1, 5);
}

/*
 * SI_04: Invalid position → all OFF
 * - StatusIndicator_Update(position_valid=0, logical_position=0xFF)
 * - All position LEDs OFF: RESET all (no sets)
 */
void test_SI_04_Invalid_position_all_OFF(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Update(0, 0xFF);
}

/*
 * SI_05: Power LED software control
 * - StatusIndicator_SetPowerLED(0) then (1)
 * - Power LED OFF then ON: RESET then SET on PIN_0
 */
void test_SI_05_Power_LED_software_control(void)
{
    /* Power OFF */
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    StatusIndicator_SetPowerLED(0);

    /* Power ON */
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    StatusIndicator_SetPowerLED(1);
}

/*
 * SI_06: Boundary out-of-range (pos=6) → all OFF
 * - StatusIndicator_Update(position_valid=1, logical_position=6)
 * - All position LEDs OFF, only RESETs, no SETs
 */
void test_SI_06_Boundary_out_of_range_pos6_all_OFF(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Update(1, 6);
}
