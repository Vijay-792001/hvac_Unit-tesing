ag-code-summarizer Agent

- Module/File: item / c_file_path — DC motor drive logic for HVAC flap positioning; traceable to SWE-REQ-003/004/008/009/010/013/014/015/035/047/054.
- Purpose: Control a DC motor via GPIO direction pins and a PWM channel to move a flap to a sensed target position, with safe startup/stop behavior and periodic supervision.
- Key dependencies/hardware bindings: STM32 HAL (stm32f4xx_hal.h), TIM3 PWM channel 1 (extern TIM_HandleTypeDef htim3), GPIOB pins PB0/PB1 for direction, and Position Sensing module (position_sensing.h: PositionSensing_Update/GetPosition/IsAtTarget). Public types/protos come from motor_controller.h (MotorState_t).
- Internal state: s_motor_state (STOPPED/MOVING_FWD/MOVING_REV), s_target_position (uint8_t), s_movement_active (uint8_t) to track command and motion lifecycle.
- Main functions:
  - MotorController_Init: Configures PB0/PB1 as push-pull outputs, drives both low (safe), initializes state/target/movement flags, and starts the PWM channel.
  - MotorController_MoveTo(target): Stores target; reads current position; if already at target, aborts; else selects direction by comparing current vs target (sets one dir pin high, the other low), marks movement active, and starts PWM; aborts if position read fails.
  - MotorController_Update: Periodic supervision; refreshes sensing, and if moving, stops the motor when PositionSensing_IsAtTarget(target) is true; aborts on invalid/out-of-range sensor feedback.
  - MotorController_Abort: Safe stop—state STOPPED, movement flag cleared, both dir pins low, PWM stopped.
  - Query helpers: MotorController_GetState, MotorController_IsMoving, MotorController_GetTarget(out) which only returns a value when not moving and s_target_position < 6.
- Control flow: Init -> MoveTo issues motion (sets direction + PWM) -> Update called periodically to stop on reaching target or on sensing error -> Abort used for immediate/safe stop and also by other paths (already at target, invalid sensor).
- Error handling/safety: Defaults to safe outputs at init and abort; movement is prevented/stopped if position read fails; avoids unnecessary motion if at target; uses s_movement_active to gate update logic.
- Notable assumptions/risks:
  - No validation of target range in MoveTo; only GetTarget enforces target < 6, so out-of-range targets could cause indefinite motion until external abort or sensor error.
  - PWM duty cycle/speed is assumed preconfigured elsewhere; this module only starts/stops PWM.
  - No timeout/stall detection or end-stop protection; relies solely on PositionSensing to detect achieving target.
  - HAL_TIM_PWM_Start is called in Init and again on MoveTo (typically harmless but redundant); concurrency/thread-safety not addressed (assumes single-threaded/main-loop).

---
