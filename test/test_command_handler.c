/* ===== test_command_handler.c ===== */
#include "unity.h"
#include "command_handler.h"
#include "mock_stm32f4xx_hal.h"

extern UART_HandleTypeDef huart2;

void setUp(void) {}
void tearDown(void) {}

/*
 * TestPlan: Command Handler
 * CH_TC01: Accept valid command '0'
 * CH_TC02: Accept valid command '5'
 * CH_TC03: Reject numeric out-of-range command ('8')
 * CH_TC04: Reject non-numeric command ('x')
 * CH_TC05: Reject UART receive failure
 * CH_TC06: Handle NULL pointer safely
 * CH_TC07: Do not modify output on invalid data
 */

void test_CH_TC01_AcceptValidCommand_Zero(void)
{
    uint8_t result = 0xAA;
    uint8_t rx = '0';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    uint8_t ret = CommandHandler_PollCommand(&result);

    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(0, result);
}

void test_CH_TC02_AcceptValidCommand_Five(void)
{
    uint8_t result = 0xAA;
    uint8_t rx = '5';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    uint8_t ret = CommandHandler_PollCommand(&result);

    TEST_ASSERT_EQUAL_UINT8(1, ret);
    TEST_ASSERT_EQUAL_UINT8(5, result);
}

void test_CH_TC03_RejectNumericOutOfRange(void)
{
    uint8_t result = 0xAB;
    uint8_t rx = '8';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    uint8_t ret = CommandHandler_PollCommand(&result);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0xAB, result);
}

void test_CH_TC04_RejectNonNumericCommand(void)
{
    uint8_t result = 0xAD;
    uint8_t rx = 'x';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    uint8_t ret = CommandHandler_PollCommand(&result);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0xAD, result);
}

void test_CH_TC05_RejectUARTRxFailure(void)
{
    uint8_t result = 0x12;
    uint8_t rx = 0x00;

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_ERROR);

    uint8_t ret = CommandHandler_PollCommand(&result);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0x12, result);
}

void test_CH_TC06_HandleNullPointer(void)
{
    uint8_t ret = CommandHandler_PollCommand(NULL);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
}

void test_CH_TC07_OutputUnchangedOnInvalidData(void)
{
    uint8_t result = 0x55;
    uint8_t rx = '9';

    HAL_UART_Receive_ExpectAndReturn(&huart2, &rx, 1, 10, HAL_OK);

    uint8_t ret = CommandHandler_PollCommand(&result);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(0x55, result);
}

/* ===== test_motor_controller.c ===== */
#include "unity.h"
#include "motor_controller.h"
#include "mock_stm32f4xx_hal.h"
#include "mock_position_sensing.h"

extern TIM_HandleTypeDef htim3;

void setUp(void) {}
void tearDown(void) {}

/*
 * TestPlan: Motor Controller
 * MC_TC01: Init sets safe state and starts PWM
 * MC_TC02: Move forward when current < target
 * MC_TC03: Move reverse when current > target
 * MC_TC04: No movement when already at target (stop)
 * MC_TC05: Abort if current position is invalid at start of MoveTo
 * MC_TC06: Update does nothing when motor not moving
 * MC_TC07: While moving, if GetPosition becomes invalid in Update -> Abort
 * MC_TC08: While moving, stop when target reached
 * MC_TC09: While moving, continue if not at target
 * MC_TC10: Abort sets safe outputs/pins/PWM
 * MC_TC11: GetTarget returns stored if valid and stopped
 * MC_TC12: GetTarget returns 0 when moving
 * MC_TC13: GetTarget returns 0 if NULL pointer
 * MC_TC14: GetTarget returns 0 if stored target is >= 6
 */

void test_MC_TC01_InitSetsSafeAndStartsPWM(void)
{
    HAL_GPIO_Init_Ignore();
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Init();

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());

    uint8_t tgt = 0xFE;
    TEST_ASSERT_EQUAL(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL(0, tgt);
}

void test_MC_TC02_MoveForward_CurrentLessThanTarget(void)
{
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(4);

    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_FWD, MotorController_GetState());
}

void test_MC_TC03_MoveReverse_CurrentGreaterThanTarget(void)
{
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(2);

    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_MOVING_REV, MotorController_GetState());
}

void test_MC_TC04_NoMovementWhenAtTarget(void)
{
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(3);

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

void test_MC_TC05_AbortIfPositionInvalidAtStartMoveTo(void)
{
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_MoveTo(5);

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

void test_MC_TC06_UpdateDoesNothingWhenNotMoving(void)
{
    MotorController_Abort();
    MotorController_Update();

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

void test_MC_TC07_Update_AbortIfPositionBecomesInvalid(void)
{
    MotorController_MoveTo(2);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
    TEST_ASSERT_EQUAL(MOTOR_STATE_STOPPED, MotorController_GetState());
}

void test_MC_TC08_Update_StopAtTarget(void)
{
    MotorController_MoveTo(4);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 1);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Update();

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

void test_MC_TC09_Update_ContinueIfNotAtTarget(void)
{
    MotorController_MoveTo(4);

    PositionSensing_Update_Expect();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(1);
    PositionSensing_IsAtTarget_ExpectAndReturn(4, 0);

    MotorController_Update();

    TEST_ASSERT_EQUAL(1, MotorController_IsMoving());
}

void test_MC_TC10_AbortSetsSafeOutputs(void)
{
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);

    MotorController_Abort();

    TEST_ASSERT_EQUAL(0, MotorController_IsMoving());
}

void test_MC_TC11_GetTargetWorksWhenStoppedAndValid(void)
{
    MotorController_Init();
    MotorController_MoveTo(2);
    MotorController_Abort();
    uint8_t tgt = 0xAB;
    TEST_ASSERT_EQUAL(1, MotorController_GetTarget(&tgt));
    TEST_ASSERT_EQUAL(2, tgt);
}

void test_MC_TC12_GetTargetFailsWhenMoving(void)
{
    MotorController_MoveTo(2);
    uint8_t tgt = 0;
    TEST_ASSERT_EQUAL(0, MotorController_GetTarget(&tgt));
}

void test_MC_TC13_GetTargetFailsOnNULLPointer(void)
{
    MotorController_Abort();
    TEST_ASSERT_EQUAL(0, MotorController_GetTarget(NULL));
}

void test_MC_TC14_GetTargetFailsOnTargetOutOfRange(void)
{
    MotorController_Init();
    PositionSensing_GetPosition_ExpectAnyArgsAndReturn(0);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop_Expect(&htim3, TIM_CHANNEL_1);
    MotorController_MoveTo(6);
    uint8_t tgt = 0;
    TEST_ASSERT_EQUAL(0, MotorController_GetTarget(&tgt));
}


/***** ===== test_position_sensing.c ===== *****/
#include "unity.h"
#include "position_sensing.h"
#include "mock_stm32f4xx_hal.h"

extern ADC_HandleTypeDef hadc1;

void setUp(void) {}
void tearDown(void) {}

/*
 * TestPlan: Position Sensing
 * PS_TC01: Init resets internal state
 * PS_TC02: GetPosition() rejects NULL pointer
 * PS_TC03: Update sets valid when ADC poll OK (ADC=4060)
 * PS_TC04: Map ADC 3800 to position 1
 * PS_TC05: Map ADC 3500 to position 2
 * PS_TC06: Map ADC 3000 to position 3
 * PS_TC07: Map ADC 1500 to position 4
 * PS_TC08: Map ADC 500 to position 5
 * PS_TC09: Update sets invalid when ADC poll fails
 * PS_TC10: GetPosition returns last stored mapped value after update
 * PS_TC11: IsAtTarget rejects out-of-range targets
 * PS_TC12: IsAtTarget returns 1 when adc is inside stop range
 * PS_TC13: IsAtTarget returns 0 when adc is outside stop range
 * PS_TC14: IsAtTarget works for stop window at position 3
 */

void test_PS_TC01_InitResetsInternalState(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL(0, PositionSensing_IsValid());
    uint8_t val = 0x33;
    TEST_ASSERT_EQUAL(0, PositionSensing_GetPosition(&val));
}

void test_PS_TC02_GetPositionRejectsNull(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL(0, PositionSensing_GetPosition(NULL));
}

void test_PS_TC03_UpdateSetsValidWhenADCPollOK(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Init();
    PositionSensing_Update();

    TEST_ASSERT_EQUAL(1, PositionSensing_IsValid());
    uint8_t val = 0x66;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&val));
    TEST_ASSERT_EQUAL(0, val);
}

void test_PS_TC04_MapADCToPosition1(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3800);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t val = 0x77;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&val));
    TEST_ASSERT_EQUAL(1, val);
}

void test_PS_TC05_MapADCToPosition2(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3500);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t val = 0x88;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&val));
    TEST_ASSERT_EQUAL(2, val);
}

void test_PS_TC06_MapADCToPosition3(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t val = 0x99;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&val));
    TEST_ASSERT_EQUAL(3, val);
}

void test_PS_TC07_MapADCToPosition4(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1500);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t val = 0x11;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&val));
    TEST_ASSERT_EQUAL(4, val);
}

void test_PS_TC08_MapADCToPosition5(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 500);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t val = 0x22;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&val));
    TEST_ASSERT_EQUAL(5, val);
}

void test_PS_TC09_UpdateSetsInvalidWhenADCPollFail(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_ERROR);

    PositionSensing_Init();
    PositionSensing_Update();

    TEST_ASSERT_EQUAL(0, PositionSensing_IsValid());
    uint8_t val = 0x44;
    TEST_ASSERT_EQUAL(0, PositionSensing_GetPosition(&val));
}

void test_PS_TC10_GetPositionAfterUpdate(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3000);

    PositionSensing_Init();
    PositionSensing_Update();

    uint8_t val = 0xBB;
    TEST_ASSERT_EQUAL(1, PositionSensing_GetPosition(&val));
    TEST_ASSERT_EQUAL(3, val);
}

void test_PS_TC11_IsAtTargetRejectsOutOfRange(void)
{
    PositionSensing_Init();
    TEST_ASSERT_EQUAL(0, PositionSensing_IsAtTarget(6));
}

void test_PS_TC12_IsAtTarget_InsideStopRange(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4060);

    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL(1, PositionSensing_IsAtTarget(0));
}

void test_PS_TC13_IsAtTarget_OutsideStopRange(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 4040);

    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL(0, PositionSensing_IsAtTarget(0));
}

void test_PS_TC14_IsAtTarget_StopWindow_Position3(void)
{
    HAL_ADC_Start_Expect(&hadc1);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, 2, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 3320);

    PositionSensing_Init();
    PositionSensing_Update();
    TEST_ASSERT_EQUAL(1, PositionSensing_IsAtTarget(3));
}


/***** ===== test_status_indicator.c ===== *****/
#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

void setUp(void) {}
void tearDown(void) {}

/*
 * TestPlan: Status Indicator
 * SI_TC01: Power LED turns ON after init, all pos LEDs OFF
 * SI_TC02: Position 0 shows no green LED (all OFF)
 * SI_TC03: Display position 5 (only 5 ON, rest OFF)
 * SI_TC04: Invalid position—all LEDs OFF
 * SI_TC05: Power LED software control OFF/ON
 * SI_TC06: Boundary out-of-range (pos=6) - all LEDs OFF
 */

static const uint16_t s_led_pos_pins[STATUS_INDICATOR_NUM] = {
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5
};

void test_SI_TC01_PowerLEDonAfterInit(void)
{
    HAL_GPIO_Init_Ignore();
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    for (uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(GPIOC, s_led_pos_pins[i], GPIO_PIN_RESET);
    }
    StatusIndicator_Init();
}

void test_SI_TC02_Position0_NoGreenLED(void)
{
    for (uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(GPIOC, s_led_pos_pins[i], GPIO_PIN_RESET);
    }
    StatusIndicator_Update(1, 0);
}

void test_SI_TC03_DisplayPosition5(void)
{
    for (uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(GPIOC, s_led_pos_pins[i], GPIO_PIN_RESET);
    }
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
    StatusIndicator_Update(1, 5);
}

void test_SI_TC04_InvalidPosition_AllLEDsOff(void)
{
    for (uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(GPIOC, s_led_pos_pins[i], GPIO_PIN_RESET);
    }
    StatusIndicator_Update(0, 0xFF);
}

void test_SI_TC05_PowerLEDSoftwareControl_Off_On(void)
{
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    StatusIndicator_SetPowerLED(0);
    HAL_GPIO_WritePin_Expect(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    StatusIndicator_SetPowerLED(1);
}

void test_SI_TC06_PositionOutOfRange_AllLEDsOff(void)
{
    for (uint8_t i = 0; i < STATUS_INDICATOR_NUM; ++i) {
        HAL_GPIO_WritePin_Expect(GPIOC, s_led_pos_pins[i], GPIO_PIN_RESET);
    }
    StatusIndicator_Update(1, 6);
}
