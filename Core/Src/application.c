/*
* application.c
*
*  Created on: Apr 24, 2025
*      Author: elias
*/
/*====================  INCLUDES  ====================*/
#include  "application.h"
#include "main.h"
#include "display.h"
#include "vl53l1_api.h"
#include "vl53l1_platform.h"
#include "FSM_Handler.h"
#include "EventsManagement.h"

/*====================  TYPEDEFS & ENUMS  ====================*/

typedef enum {
    eEpreuve_1,
    eEpreuve_2,
    eEpreuve_3,

    eNbrofEpreuves
} selected_Epreuve_t;

/*====================  EXTERNAL VARIABLES  ====================*/

extern COM_InitTypeDef BspCOMInit;
extern SPI_HandleTypeDef hspi1;
extern I2C_HandleTypeDef hi2c3;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim1;
extern ADC_HandleTypeDef hadc1;

/*====================  GLOBAL VARIABLES  ====================*/

// Volatile globale Variablen
volatile int32_t encoderCount = 0;
volatile int32_t lastEncoderValue = 0;

// Nicht volatile globale Variablen
VL53L1_Dev_t dev;
VL53L1_DEV Dev = &dev;
VL53L1_DeviceInfo_t deviceInfo;
VL53L1_Error status;

/*====================  STATIC VARIABLES  ====================*/

static FSM_States_t myState = eTR_eStart;
static EventsBuffer_t myEventBuffer;
static selected_Epreuve_t currentEpreuve;
static uint32_t selected_height_cm;
static char height_str[15];
static volatile VL53L1_RangingMeasurementData_t RangingData;
static uint32_t duty_cycle_in_percent = 50;
static uint16_t counter_30ms = 0;
static uint16_t counter_1s = 0;
static uint16_t adc_buffer[ADC_BUFFER_SIZE];
static float voltage_condesator;

/*====================  STATIC FUNCTION PROTOTYPES  ====================*/

static void tran(FSM_States_t newState);
static void updateEpreuveDisplay(void);
static void set_PWM_DutyCycle(uint32_t duty_cycle_in_percent);


/*====================  FUNCTION DEFINITIONS  ====================*/
// (Hier würden dann die Funktionsdefinitionen folgen)






void application(void){
	  Display_Initialize();
	  SH1106_Clear();

	  dev.i2c_slave_address = 0x29;



	 VL53L1_Error status;


	 status = VL53L1_GetDeviceInfo(Dev, &deviceInfo);

	 if (status == VL53L1_ERROR_NONE) {
		 // connection success
		 printf("Connection success\n");
	 } else {
		// no connection
		 printf("No connection\n");
	 }

	 // Sensor initialisieren
	 status = VL53L1_DataInit(Dev);
	 if (status != VL53L1_ERROR_NONE) {
		 printf("DataInit fehlgeschlagen\n");
	 }

	 status = VL53L1_StaticInit(Dev);   // Initialisiert die Register des Sensors
	 if (status != VL53L1_ERROR_NONE) {
		 printf("StaticInit fehlgeschlagen\n");
	 }

	 VL53L1_SetDistanceMode(Dev, VL53L1_DISTANCEMODE_LONG);  // 1 = Short, 2 = Long
	 VL53L1_SetMeasurementTimingBudgetMicroSeconds(Dev, 50000); // z.B. 50 ms
	 VL53L1_SetInterruptPolarity(Dev, VL53L1_DEVICEINTERRUPTPOLARITY_ACTIVE_LOW);
	 VL53L1_set_GPIO_interrupt_config(
		 Dev,
		 VL53L1_GPIOINTMODE_DISABLED,   // intr_mode_distance
		 VL53L1_GPIOINTMODE_DISABLED,   // intr_mode_rate
		 1,            // interrupt bei neuer Messung
		 0,                             // no target
		 0,                             // combined mode
		 0, 0, 0, 0                     // Schwellenwerte
	 );
	 VL53L1_StartMeasurement(Dev);
	 //VL53L1_StopMeasurement(Dev);




	 // Starten des DMA für die Spannungsmessung
	 HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_BUFFER_SIZE);

	 // Starten des Timers für die FSM
	 HAL_TIM_Base_Start_IT(&htim2);


	  while(1){


		  // Event Producer

		  // Event Consumer

		  TrotinettControlTask(&myEventBuffer);
	  }
}


// === eStart ===
static void handle_eStart_EntryFct(void) {

}

static void handle_eStart(FSM_States_t state, EventsTypes_t event) {
	switch(event){
		case eTimeTickElapsed_10ms:
			tran(eTR_eSelectEpreuve);
			break;
		default:
			break;
	}
}

// === eSelectEpreuve ===
static void handle_eSelectEpreuve_EntryFct(void) {
	currentEpreuve = eEpreuve_1;
	SH1106_WriteString_AllAtOnce(0,0,"Select Epreuve:",FONT_6x8);
	SH1106_WriteString_AllAtOnce(0,2,"-> Epreuve 1",FONT_6x8);
	SH1106_WriteString_AllAtOnce(0,4,"   Epreuve 2",FONT_6x8);
	SH1106_WriteString_AllAtOnce(0,6,"   Epreuve 3",FONT_6x8);
}

static void handle_eSelectEpreuve(FSM_States_t state, EventsTypes_t event) {
	switch(event){
		case eTimeTickElapsed_10ms:
			break;
		case eRotaryEncoder_moved_right:
			currentEpreuve = (currentEpreuve < eNbrofEpreuves - 1) ? currentEpreuve + 1 : eEpreuve_1;
			updateEpreuveDisplay();
			break;
		case eRotaryEncoder_moved_left:
			currentEpreuve = (currentEpreuve > eEpreuve_1) ? currentEpreuve -1 : eEpreuve_3;
			updateEpreuveDisplay();
			break;
		case eRotaryEncoder_pressed:
			switch(currentEpreuve){
				case eEpreuve_1:
					tran(eTR_eEpreuve_1);
					break;
				case eEpreuve_2:
					tran(eTR_eEpreuve_2);
					break;
				case eEpreuve_3:
					tran(eTR_eEpreuve_3);
					break;
				default:
					break;
			}
		default:
			break;
	}
}

// === eEpreuve_1 ===
static void handle_eEpreuve_1_EntryFct(void) {
	selected_height_cm = 100;
	SH1106_ClearDisplay();
	SH1106_WriteString_AllAtOnce(0, 0, "select height in cm:", FONT_6x8);
	snprintf(height_str, sizeof(height_str), "%3ld cm", selected_height_cm);
	SH1106_WriteString_AllAtOnce(0, 2, height_str, FONT_6x8);
}

static void handle_eEpreuve_1(FSM_States_t state, EventsTypes_t event) {
	switch(event){
		case eTimeTickElapsed_10ms:
			break;
		case eRotaryEncoder_moved_left:
			selected_height_cm = (selected_height_cm > 1) ? selected_height_cm - 1 : 175;
			snprintf(height_str, sizeof(height_str), "%3ld cm", selected_height_cm);
			SH1106_WriteString_AllAtOnce(0, 2, height_str, FONT_6x8);
			break;
		case eRotaryEncoder_moved_right:
			selected_height_cm = (selected_height_cm < 175) ? selected_height_cm + 1 : 1;
			snprintf(height_str, sizeof(height_str), "%3ld cm", selected_height_cm);
			SH1106_WriteString_AllAtOnce(0, 2, height_str, FONT_6x8);
			break;
		case eRotaryEncoder_pressed:
			tran(eTR_eEpreuve_1_ChargeEnergy);
			break;
		default:
			break;
	}
}

// === eEpreuve_1_ChargeEnergy ===
static void handle_eEpreuve_1_ChargeEnergy_EntryFct(void) {
	SH1106_ClearDisplay();
	SH1106_WriteString_AllAtOnce(0, 0, "Charge Energy", FONT_6x8);
}

static void handle_eEpreuve_1_ChargeEnergy(FSM_States_t state, EventsTypes_t event) {
	switch(event){
		case eTimeTickElapsed_10ms:
			break;
		case eRotaryEncoder_pressed:
			tran(eTR_eEpreuve_1_StartLiftingProcess);
			break;
		default:
			break;
	}
}

// === eEpreuve_1_StartLiftingProcess ===
static void handle_eEpreuve_1_StartLiftingProcess_EntryFct(void) {
	SH1106_ClearDisplay();
	SH1106_WriteString_AllAtOnce(0, 0, "Lifting Process", FONT_6x8);
	// Duty Cycle setzen (Buck langsam starten)
	set_PWM_DutyCycle(DUTY_CYCLE_0_Percent);
	// Start PWM
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	counter_1s = 0;
	counter_30ms = 0;
	duty_cycle_in_percent = 0;
}

static void handle_eEpreuve_1_StartLiftingProcess(FSM_States_t state, EventsTypes_t event) {
	switch(event){
		case eTimeTickElapsed_10ms:
			if(++counter_30ms >= 3)
			{
				if(duty_cycle_in_percent <= 100)
				{
					duty_cycle_in_percent += 1;
					set_PWM_DutyCycle(duty_cycle_in_percent);
				}
				counter_30ms = 0;
			}

			if(++counter_1s >= 100)
			{
				int16_t mm = RangingData.RangeMilliMeter;
				int16_t cm_ganz = mm / 10;
				int16_t cm_nachkomma = abs(mm % 10);  // Vorzeichenfrei für Anzeige
				snprintf(height_str, sizeof(height_str), "%d.%d cm", cm_ganz, cm_nachkomma);
				SH1106_WriteString_AllAtOnce(0, 2, height_str, FONT_6x8);
				counter_1s = 0;
			}
			HAL_GPIO_TogglePin(Test_10ms_delay_GPIO_Port, Test_10ms_delay_Pin);
			break;
		case eRotaryEncoder_pressed:
			tran(eTR_eEpreuve_1_StopLiftingProcess);
			break;
		default:
			break;
	}
}

// === eEpreuve_1_StopLiftingProcess ===
static void handle_eEpreuve_1_StopLiftingProcess_EntryFct(void) {
	SH1106_ClearDisplay();
	SH1106_WriteString_AllAtOnce(0, 0, "Stop Lifting", FONT_6x8);
	// stoppen des pwm für den boost converter
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
}

static void handle_eEpreuve_1_StopLiftingProcess(FSM_States_t state, EventsTypes_t event) {
	switch(event){
		case eTimeTickElapsed_10ms:
			break;
		default:
			break;
	}
}

// === eEpreuve_1_LowerProcess ===
static void handle_eEpreuve_1_LowerProcess_EntryFct(void) {

}

static void handle_eEpreuve_1_LowerProcess(FSM_States_t state, EventsTypes_t event) {
	switch(event){
		case eTimeTickElapsed_10ms:
			break;
		default:
			break;
	}
}

// === eEpreuve_2 ===
static void handle_eEpreuve_2_EntryFct(void) {
	selected_height_cm = 25;
	SH1106_ClearDisplay();
	SH1106_WriteString_AllAtOnce(0, 0, "select height in cm:", FONT_6x8);
	snprintf(height_str, sizeof(height_str), "%3ld cm", selected_height_cm);
	SH1106_WriteString_AllAtOnce(0, 2, height_str, FONT_6x8);
}

static void handle_eEpreuve_2(FSM_States_t state, EventsTypes_t event) {
	switch(event){
		case eTimeTickElapsed_10ms:
			break;
		case eRotaryEncoder_moved_left:
				selected_height_cm = (selected_height_cm > 25) ? selected_height_cm - 1 : 75;
				snprintf(height_str, sizeof(height_str), "%3ld cm", selected_height_cm);
				SH1106_WriteString_AllAtOnce(0, 2, height_str, FONT_6x8);
				break;
		case eRotaryEncoder_moved_right:
				selected_height_cm = (selected_height_cm < 75) ? selected_height_cm + 1 : 25;
				snprintf(height_str, sizeof(height_str), "%3ld cm", selected_height_cm);
				SH1106_WriteString_AllAtOnce(0, 2, height_str, FONT_6x8);
				break;
		default:
			break;
	}
}

// === eEpreuve_3 ===
static void handle_eEpreuve_3_EntryFct(void) {

}

static void handle_eEpreuve_3(FSM_States_t state, EventsTypes_t event) {
	switch(event){
		case eTimeTickElapsed_10ms:
			break;
		default:
			break;
	}
}

// === eEpreuve_3_ChargeEnergy ===
static void handle_eEpreuve_3_ChargeEnergy_EntryFct(void) {

}

static void handle_eEpreuve_3_ChargeEnergy(FSM_States_t state, EventsTypes_t event) {
	switch(event){
		case eTimeTickElapsed_10ms:
			break;
		default:
			break;
	}
}

// === eEpreuve_3_StartLiftingProcess ===
static void handle_eEpreuve_3_StartLiftingProcess_EntryFct(void) {

}

static void handle_eEpreuve_3_StartLiftingProcess(FSM_States_t state, EventsTypes_t event) {
	switch(event){
		case eTimeTickElapsed_10ms:
			break;
		default:
			break;
	}
}

// === eEpreuve_3_StopLiftingProcess ===
static void handle_eEpreuve_3_StopLiftingProcess_EntryFct(void) {

}

static void handle_eEpreuve_3_StopLiftingProcess(FSM_States_t state, EventsTypes_t event) {
	switch(event){
		case eTimeTickElapsed_10ms:
			break;
		default:
			break;
	}
}

// === eEpreuve_3_LowerProcess ===
static void handle_eEpreuve_3_LowerProcess_EntryFct(void) {

}

static void handle_eEpreuve_3_LowerProcess(FSM_States_t state, EventsTypes_t event) {
	switch(event){
		case eTimeTickElapsed_10ms:
			break;
		default:
			break;
	}
}

// === eEpreuve_3_EndofTime ===
static void handle_eEpreuve_3_EndofTime_EntryFct(void) {

}

static void handle_eEpreuve_3_EndofTime(FSM_States_t state, EventsTypes_t event) {
	switch(event){
		case eTimeTickElapsed_10ms:
			break;
		default:
			break;
	}
}


static const FSM_State_Handler_t FSM_State_Handler[eNbrOfFSMStates] =
{
    {eTR_eStart, handle_eStart_EntryFct, handle_eStart, nullptr},
    {eTR_eSelectEpreuve, handle_eSelectEpreuve_EntryFct, handle_eSelectEpreuve, nullptr},
    {eTR_eEpreuve_1, handle_eEpreuve_1_EntryFct, handle_eEpreuve_1, nullptr},
    {eTR_eEpreuve_1_ChargeEnergy, handle_eEpreuve_1_ChargeEnergy_EntryFct, handle_eEpreuve_1_ChargeEnergy, nullptr},
    {eTR_eEpreuve_1_StartLiftingProcess, handle_eEpreuve_1_StartLiftingProcess_EntryFct, handle_eEpreuve_1_StartLiftingProcess, nullptr},
    {eTR_eEpreuve_1_StopLiftingProcess, handle_eEpreuve_1_StopLiftingProcess_EntryFct, handle_eEpreuve_1_StopLiftingProcess, nullptr},
    {eTR_eEpreuve_1_LowerProcess, handle_eEpreuve_1_LowerProcess_EntryFct, handle_eEpreuve_1_LowerProcess, nullptr},
    {eTR_eEpreuve_2, handle_eEpreuve_2_EntryFct, handle_eEpreuve_2, nullptr},
    {eTR_eEpreuve_3, handle_eEpreuve_3_EntryFct, handle_eEpreuve_3, nullptr},
    {eTR_eEpreuve_3_ChargeEnergy, handle_eEpreuve_3_ChargeEnergy_EntryFct, handle_eEpreuve_3_ChargeEnergy, nullptr},
    {eTR_eEpreuve_3_StartLiftingProcess, handle_eEpreuve_3_StartLiftingProcess_EntryFct, handle_eEpreuve_3_StartLiftingProcess, nullptr},
    {eTR_eEpreuve_3_StopLiftingProcess, handle_eEpreuve_3_StopLiftingProcess_EntryFct, handle_eEpreuve_3_StopLiftingProcess, nullptr},
    {eTR_eEpreuve_3_LowerProcess, handle_eEpreuve_3_LowerProcess_EntryFct, handle_eEpreuve_3_LowerProcess, nullptr},
    {eTR_eEpreuve_3_EndofTime, handle_eEpreuve_3_EndofTime_EntryFct, handle_eEpreuve_3_EndofTime, nullptr}
};


static void tran(FSM_States_t newState)
{
    if (nullptr != FSM_State_Handler[myState].pExitHandler)
    {
        FSM_State_Handler[myState].pExitHandler();
    }
    myState = newState;
    if (nullptr != FSM_State_Handler[myState].pEntryHandler)
    {
        FSM_State_Handler[myState].pEntryHandler();
    }
}

void TrotinettControlTask(EventsBuffer_t* pBuff){
	EventsTypes_t event;
	while(eventsManagement_Pop(pBuff, &event)){
		Trotinette_FSM(event);
	}
}


void Trotinette_FSM(EventsTypes_t event)
{
    //assert(myState<eNbrOfFSMStates);
    if (nullptr != FSM_State_Handler[myState].pRunHandler)
    {
        FSM_State_Handler[myState].pRunHandler(myState, event);
    }
}


FSM_States_t MotorControl_getActualState(void)
{
  return myState;
}

static void updateEpreuveDisplay(void) {
	SH1106_WriteString_AllAtOnce(0,2, (currentEpreuve == eEpreuve_1) ? "-> Epreuve 1" : "   Epreuve 1", FONT_6x8);
	SH1106_WriteString_AllAtOnce(0,4, (currentEpreuve == eEpreuve_2) ? "-> Epreuve 2" : "   Epreuve 2", FONT_6x8);
	SH1106_WriteString_AllAtOnce(0,6, (currentEpreuve == eEpreuve_3) ? "-> Epreuve 3" : "   Epreuve 3", FONT_6x8);
}

static void set_PWM_DutyCycle(uint32_t duty_cycle_in_percent){
	uint32_t period = htim1.Init.Period + 1;
	uint32_t duty = (duty_cycle_in_percent * period) / 100;
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, duty);
}


// --------------------------------------------------------------
// ------------------- Interrupts -------------------------------
// --------------------------------------------------------------

// Callback-Funktion (von HAL automatisch aufgerufen)
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static uint8_t last_state = 0;
    static int8_t step_accumulator = 0;
    static int8_t last_delta = 0;

    if (GPIO_Pin == Rotary_Encoder_SCK_Pin || GPIO_Pin == Rotary_Encoder_DT_Pin)
    {
    	uint8_t clk = HAL_GPIO_ReadPin(Rotary_Encoder_SCK_GPIO_Port, Rotary_Encoder_SCK_Pin);
        uint8_t dt  = HAL_GPIO_ReadPin(Rotary_Encoder_DT_GPIO_Port,  Rotary_Encoder_DT_Pin);

        uint8_t new_state = (clk << 1) | dt;

        static const int8_t transition_table[4][4] = {
            {  0,  -1,  1,   0 },
            {  1,   0,  0,  -1 },
            { -1,   0,  0,   1 },
            {  0,   1, -1,   0 }
        };

        int8_t delta = transition_table[last_state][new_state];

        if (delta != 0)
        {
            // 👉 Reset bei Richtungswechsel
            if ((last_delta > 0 && delta < 0) || (last_delta < 0 && delta > 0)) {
                step_accumulator = 0;
            }

            step_accumulator += delta;
            last_delta = delta;

            if (step_accumulator >= 4)
            {
                step_accumulator = 0;
                EventsBuffer_addData(&myEventBuffer, eRotaryEncoder_moved_right);
            }
            else if (step_accumulator <= -4)
            {
                step_accumulator = 0;
                EventsBuffer_addData(&myEventBuffer, eRotaryEncoder_moved_left);
            }
        }

        last_state = new_state;
    }

    if (GPIO_Pin == Rotary_Encoder_SW_Pin){
    	EventsBuffer_addData(&myEventBuffer, eRotaryEncoder_pressed);
    }

    if (GPIO_Pin == ToF_interrupt_Pin){
    	EventsBuffer_addData(&myEventBuffer, eNewDistanzValue);
    	 // Messdaten holen
    	 VL53L1_GetRangingMeasurementData(Dev, (VL53L1_RangingMeasurementData_t *) &RangingData);
    	 // Interrupt quittieren + neue Messung starten
    	 VL53L1_ClearInterruptAndStartMeasurement(Dev);
    	 BSP_LED_Toggle(LED_BLUE);
    }
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        //Buffer komplett gefüllt
    	uint32_t sum = 0;
		for (int i = 0; i < ADC_BUFFER_SIZE; i++)
		{
			sum += adc_buffer[i];
		}
		uint16_t avg = sum / ADC_BUFFER_SIZE;
		voltage_condesator = (3.3f * avg) / 4095.0f;
    }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        static uint8_t tickCount = 0;
        if (++tickCount >= TICK_COUNT_10ms){
        	EventsBuffer_addData(&myEventBuffer, eTimeTickElapsed_10ms);
        	tickCount = 0;
        }
    }
}
