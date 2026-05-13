- Purpose: Implements a simple UART command poller that reads one byte from USART2 and maps ASCII digits '0'–'5' to numeric commands 0–5.
- Main function: CommandHandler_PollCommand(uint8_t* cmd_out) returns 1 if a valid command was read and stored, otherwise 0.
- Key flow: 
  1) Validate cmd_out is not NULL; 
  2) Attempt HAL_UART_Receive(&huart2, &rx, 1, 10) with a 10 ms timeout; 
  3) If receive OK and rx in '0'–'5', store rx - '0' into *cmd_out and return 1; 
  4) Otherwise return 0.
- Dependencies: 
  - STM32 HAL (stm32f4xx_hal.h) for HAL_UART_Receive and UART types. 
  - External UART handle huart2 (must be defined/initialized elsewhere). 
  - Local header command_handler.h (declares the function/interface).
- Interface/return convention: Boolean-like return (1 = success/valid command, 0 = no command/invalid/error). Outputs the parsed command via cmd_out.
- Error handling: 
  - Protects against NULL output pointer (immediately returns 0). 
  - Treats HAL receive failures/timeouts as no command (returns 0). 
  - Filters out non-digit or out-of-range bytes (returns 0).
- Assumptions/limitations: 
  - ASCII encoding for digit check. 
  - Only commands 0–5 are valid; all other bytes are ignored. 
  - Uses a fixed 10 ms blocking timeout; may impact timing in tight loops/RT contexts.
- Notable risks: 
  - Hard-coupled to huart2 (not easily reusable across UARTs). 
  - No differentiation between timeout and hard errors. 
  - Potential concurrency issues if other code/ISRs access huart2 simultaneously. 
  - Single-byte polling may leave extra bytes in the RX queue unprocessed per call.
