/*
#  * position_sensing.c
#  * Implements ADC reading, maps to logical flap position indices
#  * Trace: SWE-REQ-013, SWE-REQ-014, SWE-REQ-015, SWE-REQ-016, SWE-REQ-017, SWE-REQ-018, SWE-REQ-019, SWE-REQ-020, SWE-REQ-040, SWE-REQ-041
#  */
 #include "position_sensing.h"
 #include "stm32f4xx_hal.h"
 
extern ADC_HandleTypeDef hadc1;
 
 typedef struct
{
    uint16_t min;
    uint16_t max;
} PositionStopRange_t;

static const PositionStopRange_t s_stop_ranges[6] =
{
    {4055, 4065},
    {3837, 3857},
    {3563, 3593},
    {3309, 3329},
    {2022, 2042},
    { 315,  335}
};


 /* Internal state */
 static uint16_t s_adc_value = 0U;
 static uint8_t s_logical_position = FLAP_POSITION_INVALID;
 static uint8_t s_position_valid = 0U;
 void PositionSensing_Init(void)
 {
     /* SWE-REQ-040: Initialize ADC state */
     s_adc_value = 0U; //last adc raw value
     s_logical_position = FLAP_POSITION_INVALID; //mapped 0..5 when Poll OK, else FLAP_POSITION_INVALID
     s_position_valid = 0U; //1 only when Poll OK, else 0
 }
 
 static uint8_t Position_GetFromADC(uint16_t adc)
{
    if (adc > 4000) return 0;
    if (adc > 3750) return 1;
    if (adc > 3480) return 2;
    if (adc > 2500) return 3;
    if (adc > 1000) return 4;
    return 5;
}

uint8_t PositionSensing_IsAtTarget(uint8_t target)
{
    if (target >= 6U)
    {
        return 0U;
    }

    if ((s_adc_value >= s_stop_ranges[target].min) &&
        (s_adc_value <= s_stop_ranges[target].max))
    {
        return 1U;
    }

    return 0U;
}


 void PositionSensing_Update(void)
{
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 2) == HAL_OK)
    {
        s_adc_value = (uint16_t)HAL_ADC_GetValue(&hadc1);

        s_logical_position = Position_GetFromADC(s_adc_value);
        s_position_valid = 1U;
    }
    else
    {
        s_logical_position = FLAP_POSITION_INVALID;
        s_position_valid = 0U;
    }
}

 uint8_t PositionSensing_GetPosition(uint8_t *pos_out)
 {
     /* SWE-REQ-019: Provide current logical position if valid */
     if((s_position_valid != 0U) && (pos_out != NULL))
     {
         *pos_out = s_logical_position;
         return 1U;
     }
     return 0U;
 }
 uint8_t PositionSensing_IsValid(void)
 {
     /* SWE-REQ-017: Flag for position validity */
     return s_position_valid;
 }


