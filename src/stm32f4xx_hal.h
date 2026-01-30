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

#endif

