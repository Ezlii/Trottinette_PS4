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
static char height_str[8];

/*====================  STATIC FUNCTION PROTOTYPES  ====================*/

static void tran(FSM_States_t newState);
static void updateEpreuveDisplay(void);


/*====================  FUNCTION DEFINITIONS  ====================*/
// (Hier würden dann die Funktionsdefinitionen folgen)






void application(void){
	  Display_Initialize();
	  SH1106_Clear();

	  dev.i2c_slave_address = 0x29;

	  VL53L1_Error status;
	  VL53L1_RangingMeasurementData_t RangingData;

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

	     VL53L1_SetDistanceMode(Dev, 2);  // 1 = Short, 2 = Long
	     VL53L1_SetMeasurementTimingBudgetMicroSeconds(Dev, 50000); // z.B. 50 ms


	     VL53L1_StartMeasurement(Dev);


	     /*
	     while (1)
		 {
			 // Hier gibt es KEIN CheckForDataReady – also direkt Daten abholen
			 status = VL53L1_GetRangingMeasurementData(Dev, &RangingData);

			 if (status == VL53L1_ERROR_NONE && RangingData.RangeStatus == 0) {
				 printf("Distanz: %u mm\n", RangingData.RangeMilliMeter);
			 } else {
				 printf("Ungültige Messung (Status: %u)\n", RangingData.RangeStatus);
			 }

			 // Messung abschließen und neue starten
			 VL53L1_ClearInterruptAndStartMeasurement(Dev);

			 HAL_Delay(50);  // Warten bis nächste Messung fertig ist
		 }


		*/
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
			selected_height_cm--;
			snprintf(height_str, sizeof(height_str), "%3ld cm", selected_height_cm);
			SH1106_WriteString_AllAtOnce(0, 2, height_str, FONT_6x8);
			break;
		case eRotaryEncoder_moved_right:
			selected_height_cm++;
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

}

static void handle_eEpreuve_1_StartLiftingProcess(FSM_States_t state, EventsTypes_t event) {
	switch(event){
		case eTimeTickElapsed_10ms:
			break;
		default:
			break;
	}
}

// === eEpreuve_1_StopLiftingProcess ===
static void handle_eEpreuve_1_StopLiftingProcess_EntryFct(void) {

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
					(selected_height_cm >= 25) ? selected_height_cm-- : 75 ;
					snprintf(height_str, sizeof(height_str), "%3ld cm", selected_height_cm);
					SH1106_WriteString_AllAtOnce(0, 2, height_str, FONT_6x8);
					break;
				case eRotaryEncoder_moved_right:
					(selected_height_cm <= 75) ? selected_height_cm++ : 25;
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


//
// Interrupts
//¨

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
