 /*
#  * motor_controller.c
#  * Implements DC motor drive logic for HVAC flap positioning
#  * Trace: SWE-REQ-003, SWE-REQ-004, SWE-REQ-008, SWE-REQ-009, SWE-REQ-010, SWE-REQ-013, SWE-REQ-014, SWE-REQ-015, SWE-REQ-035, SWE-REQ-047, SWE-REQ-054
#  */
 #include "motor_controller.h"
 #include "position_sensing.h"
 #include "stm32f4xx_hal.h"
extern TIM_HandleTypeDef htim3;
 #define MOTOR_PWM_HANDLE      htim3
 #define MOTOR_PWM_CHANNEL     TIM_CHANNEL_1    
 #define MOTOR_GPIO_PORT_DIR   GPIOB
 #define MOTOR_PIN_DIR_FWD     GPIO_PIN_0
 #define MOTOR_PIN_DIR_REV     GPIO_PIN_1
 
 //static GPIO_InitTypeDef GPIO_InitStruct;
 /* Static state */
 static MotorState_t s_motor_state = MOTOR_STATE_STOPPED;
 static uint8_t s_target_position = 0U;
 static uint8_t s_movement_active = 0U;
 void MotorController_Init(void)
 {
     /* Configure IN1 and IN2 as normal output */
     GPIO_InitTypeDef GPIO_InitStruct = {0};
     GPIO_InitStruct.Pin = MOTOR_PIN_DIR_FWD | MOTOR_PIN_DIR_REV;
     GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
     GPIO_InitStruct.Pull = GPIO_NOPULL;
     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
     HAL_GPIO_Init(MOTOR_GPIO_PORT_DIR, &GPIO_InitStruct);

     /* SWE-REQ-009: Set outputs safe */
     s_motor_state = MOTOR_STATE_STOPPED;
     s_target_position = 0U;
     s_movement_active = 0U;

     /* Ensure PWM/GPIOs are safe */
     HAL_GPIO_WritePin(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
     HAL_GPIO_WritePin(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);

     /* Start PWM channel for motor */
     HAL_TIM_PWM_Start(&MOTOR_PWM_HANDLE, MOTOR_PWM_CHANNEL);
 }

 void MotorController_MoveTo(uint8_t target_position)
 {
     /* SWE-REQ-009: Initiate movement to target position */
     s_target_position = target_position;
     uint8_t current_position = 0;
     if(PositionSensing_GetPosition(&current_position))
     {
         if(current_position == s_target_position)
         {
             /* At target already, no move */
             MotorController_Abort();
             return;
         }
         /* Decide direction */
         if(current_position < s_target_position)
         {
             
             HAL_GPIO_WritePin(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
             HAL_GPIO_WritePin(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_REV, GPIO_PIN_SET);
             s_motor_state = MOTOR_STATE_MOVING_FWD;
         }
         else
         {
             HAL_GPIO_WritePin(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_FWD, GPIO_PIN_SET);
             HAL_GPIO_WritePin(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
             s_motor_state = MOTOR_STATE_MOVING_REV;
         }
         s_movement_active = 1U;
         HAL_TIM_PWM_Start(&MOTOR_PWM_HANDLE, MOTOR_PWM_CHANNEL);
     }
     else
     {
         /* Invalid position, abort */
         MotorController_Abort();
     }
 }
 void MotorController_Update(void)
 {
     /* SWE-REQ-010: Stop at target or if invalid */
     if(s_movement_active)
     {
        PositionSensing_Update();
        uint8_t current_position = 0U;
        if(PositionSensing_GetPosition(&current_position))
         {
             
             if (PositionSensing_IsAtTarget(s_target_position))
		{
    			MotorController_Abort();
		}

         }
         else
         {
             /* Out-of-range, stop motor */
             MotorController_Abort();
         }
     }
 }
 void MotorController_Abort(void)
 {
     s_motor_state = MOTOR_STATE_STOPPED;
     s_movement_active = 0U;

     HAL_GPIO_WritePin(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_FWD, GPIO_PIN_RESET);
     HAL_GPIO_WritePin(MOTOR_GPIO_PORT_DIR, MOTOR_PIN_DIR_REV, GPIO_PIN_RESET);
     HAL_TIM_PWM_Stop(&MOTOR_PWM_HANDLE, MOTOR_PWM_CHANNEL);
 }

 MotorState_t MotorController_GetState(void)
 {
     return s_motor_state;
 }

 uint8_t MotorController_IsMoving(void)
 {
     return s_movement_active;
 }

 uint8_t MotorController_GetTarget(uint8_t *target)
 {
     if ((s_movement_active == 0U) &&
         (s_target_position < 6U) &&
         (target != NULL))
     {
         *target = s_target_position;
         return 1U;
     }
     return 0U;
 }


