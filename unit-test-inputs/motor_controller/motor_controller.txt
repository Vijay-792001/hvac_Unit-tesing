/*
#  * motor_controller.h
#  * Software Unit: Motor Controller
#  * Responsibility: Motor enable/disable, direction control, move to logical position
#  * Trace: SWE-REQ-003, SWE-REQ-004, SWE-REQ-008, SWE-REQ-009, SWE-REQ-010, SWE-REQ-013, SWE-REQ-014, SWE-REQ-015, SWE-REQ-035, SWE-REQ-047, SWE-REQ-054
#  */
 #ifndef MOTOR_CONTROLLER_H
 #define MOTOR_CONTROLLER_H
 #include <stdint.h>
 #include <stddef.h>
 /* Motor movement states */
 typedef enum 
 {
     MOTOR_STATE_STOPPED = 0,
     MOTOR_STATE_MOVING_FWD,
     MOTOR_STATE_MOVING_REV
 } MotorState_t;
 void MotorController_Init(void);
 void MotorController_MoveTo(uint8_t target_position);
 void MotorController_Update(void);
 void MotorController_Abort(void);
 uint8_t MotorController_IsMoving(void);
 uint8_t MotorController_GetTarget(uint8_t *target);
 MotorState_t MotorController_GetState(void);
 #endif /* MOTOR_CONTROLLER_H */


