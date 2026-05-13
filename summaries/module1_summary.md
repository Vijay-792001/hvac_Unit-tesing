ag-code-summarizer Agent

- Purpose: Provides ADC-based flap position sensing. Reads an STM32F4 ADC channel, maps raw readings to logical flap indices 0–5, and checks if the actuator is within predefined stop ranges. Traces to SWE-REQ-013..020, 040, 041.

- Key data and mapping: 
  - s_stop_ranges[6]: inclusive min/max raw ADC bands for each target position: {4055–4065, 3837–3857, 3563–3593, 3309–3329, 2022–2042, 315–335}.
  - Internal state: s_adc_value (last raw), s_logical_position (0–5 or FLAP_POSITION_INVALID), s_position_valid (flag).
  - Static helper Position_GetFromADC(adc): threshold mapping using descending > checks: >4000→0, >3750→1, >3480→2, >2500→3, >1000→4, else→5.

- Public API:
  - PositionSensing_Init(): Resets ADC value, sets position to FLAP_POSITION_INVALID, clears validity (SWE-REQ-040).
  - PositionSensing_Update(): Starts ADC, polls with timeout 2, on success reads value, maps to logical position, sets valid; on failure sets invalid state.
  - PositionSensing_GetPosition(uint8_t* pos_out): If valid and pos_out non-null, writes logical position and returns 1; else returns 0 (SWE-REQ-019).
  - PositionSensing_IsValid(): Returns s_position_valid (SWE-REQ-017).
  - PositionSensing_IsAtTarget(uint8_t target): For target 0–5, returns 1 if s_adc_value within that target’s stop range; else 0.

- Control flow:
  - Initialize -> periodic Update reads ADC and updates state -> clients query IsValid/GetPosition for current mapped index -> IsAtTarget can be used to determine when a target stop is reached based on raw reading bands.

- Dependencies and external interfaces:
  - Headers: position_sensing.h (declares FLAP_POSITION_INVALID and API), stm32f4xx_hal.h.
  - Uses STM32 HAL ADC APIs: HAL_ADC_Start, HAL_ADC_PollForConversion(&hadc1, 2), HAL_ADC_GetValue.
  - Relies on external ADC_HandleTypeDef hadc1.

- Validation and bounds:
  - IsAtTarget guards against invalid target indices (>=6).
  - GetPosition guards against null pointer and invalid state.
  - No explicit clamping/sanity check of the raw ADC value beyond mapping thresholds.

- Error handling behavior:
  - ADC poll failure marks position invalid and clears last mapped position to FLAP_POSITION_INVALID; consumers must check IsValid or GetPosition’s return value.
  - IsAtTarget does not check s_position_valid; it evaluates the last s_adc_value regardless of validity/staleness.

- Notable assumptions/risks:
  - Single-sample, no filtering/averaging/hysteresis; may chatter near thresholds and stop ranges.
  - Very short poll timeout (2) may be inadequate depending on ADC configuration, causing frequent invalid states.
  - Mapping thresholds are independent of the stop range bands; potential mismatch if calibration drifts.
  - Potential race conditions if Update and getters are called from different contexts without synchronization.
  - Uses inclusive stop bands; bands are narrow—requires tight ADC calibration/stability.

---

Task is completed