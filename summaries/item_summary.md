ag-code-summarizer Agent

- Context: Module=item, File=c_file_path. Purpose: read raw ADC via STM32 HAL, map to a discrete “flap” position index (0–5), expose validity and target-hit checks. Traceability to multiple SWE-REQs, notably init (SWE-REQ-040), provide position (SWE-REQ-019), and validity flag (SWE-REQ-017).
- Key data/structures:
  - PositionStopRange_t {min,max}; s_stop_ranges[6] define tight ADC windows for each physical stop (0..5): e.g., [4055–4065], [3837–3857], ..., [315–335].
  - Internal state: s_adc_value (last raw), s_logical_position (mapped 0–5 or FLAP_POSITION_INVALID), s_position_valid (0/1).
  - Static mapper Position_GetFromADC(adc): thresholds >4000→0, >3750→1, >3480→2, >2500→3, >1000→4, else 5.
- Public API:
  - PositionSensing_Init(): clears state (raw=0, logical=FLAP_POSITION_INVALID, valid=0).
  - PositionSensing_Update(): HAL_ADC_Start, HAL_ADC_PollForConversion(timeout=2); on OK, captures ADC, maps to logical position, sets valid=1; on failure, sets logical=FLAP_POSITION_INVALID and valid=0 (raw remains unchanged).
  - PositionSensing_GetPosition(uint8_t* pos_out): if valid and pointer non-NULL, writes current logical position and returns 1; else returns 0.
  - PositionSensing_IsValid(): returns validity flag (1 if last update succeeded).
  - PositionSensing_IsAtTarget(uint8_t target): returns 1 if 0<=target<6 and s_adc_value within target’s stop range; else 0.
- Control/usage flow:
  - Call Init once.
  - Periodically call Update to sample ADC and refresh state.
  - Consumers read IsValid/GetPosition for logical position; optionally call IsAtTarget to confirm precise stop using tight ADC windows.
- Dependencies:
  - STM32F4 HAL ADC: stm32f4xx_hal.h, extern ADC handle hadc1; uses HAL_ADC_Start, HAL_ADC_PollForConversion, HAL_ADC_GetValue.
  - position_sensing.h for declarations and FLAP_POSITION_INVALID definition.
- Validation/error handling:
  - Guards: target index range in IsAtTarget; NULL pointer check in GetPosition.
  - Validity flag communicates success/failure of latest ADC poll; mapping only occurs on successful conversion.
  - No retry/backoff; Update uses a blocking poll with very short timeout (2).
- Notable assumptions:
  - Exactly six discrete positions with relatively narrow, stable ADC ranges (s_stop_ranges).
  - Coarse logical mapping thresholds need not exactly match tight stop windows; logical position is an approximate categorization, while IsAtTarget provides precise confirmation.
  - HAL is correctly initialized elsewhere; hadc1 configured for the channel/sequence used.
- Risks/edge cases:
  - Stale s_adc_value: on ADC failure, valid=0 but s_adc_value is not updated; IsAtTarget does not consult validity and may report “at target” based on old data.
  - No filtering/debouncing or averaging; susceptible to noise and ADC drift—especially since mapping thresholds and stop ranges are hard-coded “magic numbers.”
  - No handling of HAL_ADC_Start return or stopping the ADC; repeated starts may rely on HAL internals.
  - Tight stop windows may fail if ADC reference, temperature, or sensor tolerance shifts; thresholds/ranges likely require calibration.

---

Task is completed