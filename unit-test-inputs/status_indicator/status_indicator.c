/*
#  * status_indicator.c
#  * Controls power and position indication LEDs
#  * Trace: SWE-REQ-021, SWE-REQ-022, SWE-REQ-023, SWE-REQ-024, SWE-REQ-025, SWE-REQ-026, SWE-REQ-027, SWE-REQ-029, SWE-REQ-044
#  */
 #include "status_indicator.h"
 #include "stm32f4xx_hal.h"
 #define LED_POWER_PORT         GPIOC
 #define LED_POWER_PIN          GPIO_PIN_0
 #define LED_POS_PORT           GPIOC
 
 static GPIO_InitTypeDef GPIO_InitStruct;
 static const uint16_t s_led_pos_pins[STATUS_INDICATOR_NUM] = {
      GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5
 };
 void StatusIndicator_Init(void)
 {
     GPIO_InitStruct.Pin = LED_POWER_PIN | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
     /* Turn power LED ON, all position LEDs off */
     HAL_GPIO_WritePin(LED_POWER_PORT, LED_POWER_PIN, GPIO_PIN_SET);
     for(uint8_t i = 0U; i < STATUS_INDICATOR_NUM; ++i)
     {
         HAL_GPIO_WritePin(LED_POS_PORT, s_led_pos_pins[i], GPIO_PIN_RESET);
     }
 }
 void StatusIndicator_Update(uint8_t position_valid, uint8_t logical_position)
 {
     /* Turn OFF all position LEDs */
     for (uint8_t i = 0U; i < STATUS_INDICATOR_NUM; ++i)
     {
         HAL_GPIO_WritePin(LED_POS_PORT, s_led_pos_pins[i], GPIO_PIN_RESET);
     }

     /* Position 0 → no green LED */
     if ((position_valid != 0U) && (logical_position > 0U) &&
         (logical_position <= STATUS_INDICATOR_NUM))
     {
         /* position 1 → LED 0
            position 2 → LED 1
            ...
            position 5 → LED 4 */
         HAL_GPIO_WritePin(LED_POS_PORT,
                           s_led_pos_pins[logical_position - 1U],
                           GPIO_PIN_SET);
     }
 }

 void StatusIndicator_SetPowerLED(uint8_t onoff)
 {
     /* 1 = ON, 0 = OFF */
     if(onoff)
     {
         HAL_GPIO_WritePin(LED_POWER_PORT, LED_POWER_PIN, GPIO_PIN_SET);
     }
     else
     {
         HAL_GPIO_WritePin(LED_POWER_PORT, LED_POWER_PIN, GPIO_PIN_RESET);
     }
 }


