/*
#  * position_sensing.h
#  * Software Unit: Position Sensing
#  * Responsibility: Acquires ADC reading from potentiometer, maps to logical position (0-5)
#  * Trace: SWE-REQ-013, SWE-REQ-014, SWE-REQ-015, SWE-REQ-016, SWE-REQ-017, SWE-REQ-018, SWE-REQ-019, SWE-REQ-020, SWE-REQ-040, SWE-REQ-041
#  */
 #ifndef POSITION_SENSING_H
 #define POSITION_SENSING_H
 #include <stdint.h>
 #include <stddef.h> 
 #define FLAP_POSITION_NUM   (6U) /* 0 to 5 */
 #define FLAP_POSITION_INVALID (0xFFU)
 typedef struct
 {
     uint16_t adc_min;
     uint16_t adc_max;
 } PositionRange_t;
 void PositionSensing_Init(void);
 void PositionSensing_Update(void);
 uint8_t PositionSensing_GetPosition(uint8_t *pos_out);
 uint8_t PositionSensing_IsValid(void);
 uint8_t PositionSensing_IsAtTarget(uint8_t target);

 #endif /* POSITION_SENSING_H */


