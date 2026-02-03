#ifndef STM32F4XX_HAL_H
#define STM32F4XX_HAL_H

#include <stdint.h>

/* Fake HAL definitions */
#define HAL_OK 0
#define HAL_ERROR 1

typedef uint32_t HAL_StatusTypeDef;

/* Dummy UART handle */
typedef struct {
    int dummy;
} UART_HandleTypeDef;

/* Function prototype to be mocked */
HAL_StatusTypeDef HAL_UART_Receive(
    UART_HandleTypeDef *huart,
    uint8_t *pData,
    uint16_t Size,
    uint32_t Timeout
);

/* =========================
 * GPIO (needed for status_indicator.c)
 * ========================= */

/* Fake GPIO port type */
typedef struct {
    int dummy;
} GPIO_TypeDef;

/* Fake pin state */
typedef enum {
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET
} GPIO_PinState;

/* GPIO init struct (only fields you use in status_indicator.c) */
typedef struct {
    uint32_t Pin;
    uint32_t Mode;
    uint32_t Pull;
    uint32_t Speed;
} GPIO_InitTypeDef;

/* Fake GPIO port instance for GPIOC */
extern GPIO_TypeDef GPIOC_inst;
#define GPIOC (&GPIOC_inst)

/* GPIO pin bit masks (enough for your LEDs) */
#define GPIO_PIN_0  ((uint16_t)0x0001)
#define GPIO_PIN_1  ((uint16_t)0x0002)
#define GPIO_PIN_2  ((uint16_t)0x0004)
#define GPIO_PIN_3  ((uint16_t)0x0008)
#define GPIO_PIN_4  ((uint16_t)0x0010)
#define GPIO_PIN_5  ((uint16_t)0x0020)

/* Mode/Pull/Speed constants used by your code; HAL Configuration macros */
#define GPIO_MODE_OUTPUT_PP   (0x00000001U)
#define GPIO_NOPULL           (0x00000000U)
#define GPIO_SPEED_FREQ_LOW   (0x00000001U)

/* HAL GPIO APIs to be mocked */
void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);


/* =========================
 * ADC (needed for position_sensing.c)
 * ========================= */

/* Dummy ADC handle */
typedef struct {
    int dummy;
} ADC_HandleTypeDef;

/* HAL ADC APIs to be mocked by CMock */
HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef *hadc, uint32_t Timeout);
uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef *hadc);



#endif

