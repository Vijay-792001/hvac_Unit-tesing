/* ===== test_status_indicator.c ===== */
#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"
GPIO_TypeDef GPIOC_inst;
#define GPIOC (&GPIOC_inst)
void test_SI_01_Power_LED_ON_after_init_others_OFF(void){GPIO_InitTypeDef expected_init={.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5,.Mode=GPIO_MODE_OUTPUT_PP,.Pull=GPIO_NOPULL,.Speed=GPIO_SPEED_FREQ_LOW};HAL_GPIO_Init_Expect(GPIOC,&expected_init);HAL_GPIO_WritePin_Expect(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);for(uint8_t i=1;i<=5;i++){HAL_GPIO_WritePin_Expect(GPIOC,(uint16_t)(1U<<i),GPIO_PIN_RESET);}StatusIndicator_Init();}
void test_SI_02_Position_0_shows_no_green_LED(void){for(uint8_t i=1;i<=5;i++){HAL_GPIO_WritePin_Expect(GPIOC,(uint16_t)(1U<<i),GPIO_PIN_RESET);}StatusIndicator_Update(1,0);}
void test_SI_03_Display_position_5(void){for(uint8_t i=1;i<=5;i++){HAL_GPIO_WritePin_Expect(GPIOC,(uint16_t)(1U<<i),GPIO_PIN_RESET);}HAL_GPIO_WritePin_Expect(GPIOC,GPIO_PIN_5,GPIO_PIN_SET);StatusIndicator_Update(1,5);}
void test_SI_04_Invalid_position_all_OFF(void){for(uint8_t i=1;i<=5;i++){HAL_GPIO_WritePin_Expect(GPIOC,(uint16_t)(1U<<i),GPIO_PIN_RESET);}StatusIndicator_Update(0,0xFF);}
void test_SI_05_Power_LED_software_control(void){HAL_GPIO_WritePin_Expect(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);StatusIndicator_SetPowerLED(0);HAL_GPIO_WritePin_Expect(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);StatusIndicator_SetPowerLED(1);}
void test_SI_06_Boundary_out_of_range_pos6_all_OFF(void){for(uint8_t i=1;i<=5;i++){HAL_GPIO_WritePin_Expect(GPIOC,(uint16_t)(1U<<i),GPIO_PIN_RESET);}StatusIndicator_Update(1,6);}
