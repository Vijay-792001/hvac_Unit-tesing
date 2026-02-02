#include "unity.h"
#include "status_indicator.h"

/* Ceedling will auto-generate this mock */
#include "mock_stm32f4xx_hal.h"

/* =========================
 * Test Lifecycle Hooks
 * ========================= */
void setUp(void)
{
}

void tearDown(void)
{
}

/* -------------------------
 * HAL_GPIO_Init is a void function.
 * We'll stub it so it doesn't complain and we keep things simple.
 * ------------------------- */
static void HAL_GPIO_Init_Callback(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init, int cmock_num_calls)
{
    (void)GPIOx;
    (void)GPIO_Init;
    (void)cmock_num_calls;
}

/* =========================
 * SI_01
 * Init: Power LED ON, all position LEDs OFF
 * ========================= */
void test_StatusIndicator_Init_PowerOn_AllPosOff(void)
{
    HAL_GPIO_Init_StubWithCallback(HAL_GPIO_Init_Callback);

    /* Power LED ON */
    // Tells CMock expect the code to call exactly once (CMock stores in internal expectation list)
    /* CMock intercepts the call and checks:
        1. Was this call expected?
       2. Is this the next expected call?
       3. Do the arguments match?

    If yes: CMock marks that expectation as “fulfilled”, Execution continues
   If no: Unity immediately fails the test */
   
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);

    /* Position LEDs OFF */
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Init();
}

/* =========================
 * SI_02
 * Update(valid=1,pos=0): all position LEDs OFF (no green LED)
 * ========================= */
void test_StatusIndicator_Update_Valid_Pos0_AllOff(void)
{
    /* Update always clears all position LEDs */
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    /* No SET expected because pos=0 */
    StatusIndicator_Update(1U, 0U);
}

/* =========================
 * SI_03
 * Update(valid=1,pos=5): only LED5 ON
 * ========================= */
void test_StatusIndicator_Update_Valid_Pos5_OnlyLed5On(void)
{
    /* Clear all first */
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    /* Then set LED for position 5 */
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);

    StatusIndicator_Update(1U, 5U);
}

/* =========================
 * SI_04
 * Update(valid=0,pos=FF): all position LEDs OFF
 * ========================= */
void test_StatusIndicator_Update_Invalid_AllOff(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    StatusIndicator_Update(0U, 0xFFU);
}

/* =========================
 * SI_05
 * Power LED software control: OFF then ON
 * ========================= */
void test_StatusIndicator_SetPowerLED_OffThenOn(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    StatusIndicator_SetPowerLED(0U);

    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    StatusIndicator_SetPowerLED(1U);
}

/* =========================
 * SI_06
 * Update(valid=1,pos=6): out of range => all position LEDs OFF
 * ========================= */
void test_StatusIndicator_Update_Valid_Pos6_AllOff(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    /* No SET expected because pos=6 is invalid when NUM=5 */
    StatusIndicator_Update(1U, 6U);
}



