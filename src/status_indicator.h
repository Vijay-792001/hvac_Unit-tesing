/*
#  * status_indicator.h
#  * Software Unit: Status Indicator
#  * Responsibility: Manages power and position status LEDs
#  * Trace: SWE-REQ-021, SWE-REQ-022, SWE-REQ-023, SWE-REQ-024, SWE-REQ-025, SWE-REQ-026, SWE-REQ-027, SWE-REQ-029, SWE-REQ-044
#  */
 #ifndef STATUS_INDICATOR_H
 #define STATUS_INDICATOR_H
 #include <stdint.h>
 #define STATUS_INDICATOR_NUM  (5U) /* LEDs: 0 to 5, for positions */
 void StatusIndicator_Init(void);
 void StatusIndicator_Update(uint8_t position_valid, uint8_t logical_position);
 void StatusIndicator_SetPowerLED(uint8_t onoff);
 #endif /* STATUS_INDICATOR_H */


