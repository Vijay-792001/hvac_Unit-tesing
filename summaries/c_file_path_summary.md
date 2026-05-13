ag-code-summarizer Agent

- Purpose: DC motor drive logic for HVAC flap positioning using PWM and GPIO direction control; implements movement to a target position with safety stops (traces: SWE-REQ-003/004/008/009/010/013/014/015/035/047/054; explicitly enforced: SWE-REQ-009, SWE-REQ-010). Source: c_file_path.
- Dependencies & hardware: STM32 HAL (stm32f4xx_hal.h), TIM3 PWM channel 1 (extern TIM_HandleTypeDef htim3), GPIOB PB0/PB1 as direction pins, PositionSensing module (position_sensing.h), public API in motor_controller.h.
- Internal state: s_motor_state (MOTOR_STATE_STOPPED/MOVING_FWD/MOVING_REV), s_target_position (uint8), s_movement_active (flag).
- Key functions:
  - MotorController_Init: configures PB0/PB1 as push-pull outputs, sets safe outputs low, clears state/flags, starts PWM channel.
  - MotorController_MoveTo(uint8_t target): sets s_target_position; reads current via PositionSensing_GetPosition; if already at target -> Abort; else sets direction pins (current < target => REV pin set/FWD reset => MOVING_FWD; else opposite => MOVING_REV), marks moving, starts PWM; aborts if position read fails.
  - MotorController_Update: if moving, calls PositionSensing_Update, then stops (Abort) when PositionSensing_IsAtTarget(target) is true, or when position read fails.
  - MotorController_Abort: stops PWM, drives both direction pins low, sets state to STOPPED and clears movement flag.
  - Accessors: MotorController_GetState, MotorController_IsMoving, MotorController_GetTarget (returns 1 only if not moving, target < 6, and target pointer non-NULL; otherwise 0).
- Control flow: MoveTo decides direction and enables PWM; periodic Update monitors sensing and stops on reaching target or invalid sensing; Abort provides an immediate safe stop; state queries expose basic status.
- Error handling & safety: Safe outputs on init and abort; immediate abort when position cannot be read or is already at target; no HAL return code checks; Update guards all motor activity behind movement flag.
- Notable assumptions/risks:
  - Target range is not validated in MoveTo (only GetTarget enforces target < 6), so callers can command out-of-range targets; correctness depends on PositionSensing_IsAtTarget.
  - No timeout/stall detection; motor may run indefinitely if sensor reports valid but stagnant values and never indicates “at target.”
  - PWM duty cycle/speed not managed here; assumed configured elsewhere; PWM is started in Init and again in MoveTo (potential redundancy).
  - Direction semantics depend on wiring (FWD/REV pin mapping); no endstop/overcurrent handling; no concurrency/thread-safety protections around static state.
- Requirements trace usage: SWE-REQ-009 tagged on initiating movement/safe setup; SWE-REQ-010 tagged on stopping conditions; other listed requirements are not explicitly referenced within functions.

---

Task is completed

Action:
1) Derive module folder name from source_file_path.
   Example: source_file_path = unit-test-inputs/command_handler/command_handler.c
   module_name = command_handler
2) Build summary_path = summaries/<module_name>_summary.md
3) Use GitHub MCP push_files to create/update summary_path with content.