/*
#  * command_handler.h
#  * Software Unit: Command Handler
#  * Responsibility: Parses and validates user UART commands, manages target position requests
#  * Trace: SWE-REQ-001, SWE-REQ-002, SWE-REQ-003, SWE-REQ-004, SWE-REQ-005, SWE-REQ-021, SWE-REQ-046, SWE-REQ-053, SWE-REQ-058
#  */
 #ifndef COMMAND_HANDLER_H
 #define COMMAND_HANDLER_H
 #include <stdint.h>
 #include <stddef.h>
 #define CMD_INVALID   (0xFFU)
 #define CMD_MIN       (0U)
 #define CMD_MAX       (5U)
 #define CMD_UART_BUF_LEN (8U)
 typedef enum {
     CMD_STATE_IDLE = 0,
     CMD_STATE_COMMAND_RECEIVED,
     CMD_STATE_VALIDATED
 } CmdState_t;
 uint8_t CommandHandler_PollCommand(uint8_t *cmd_out);

 #endif /* COMMAND_HANDLER_H */


