/*
#  * command_handler.c
#  * Implements the Command Handler for UART command parsing/validation
#  */
 #include "command_handler.h"
#include "stm32f4xx_hal.h"
extern UART_HandleTypeDef huart2;
 
 
 
 uint8_t CommandHandler_PollCommand(uint8_t *cmd_out)
{
    uint8_t rx;
    
    /* CH-06: NULL pointer protection */
    if (cmd_out == NULL)
    {
        return 0U;
    }
    
    if (HAL_UART_Receive(&huart2, &rx, 1, 10) == HAL_OK)
    {
        if ((rx >= '0') && (rx <= '5'))
        {
            *cmd_out = (uint8_t)(rx - '0');
            return 1U;
        }
    }
    return 0U;
}




