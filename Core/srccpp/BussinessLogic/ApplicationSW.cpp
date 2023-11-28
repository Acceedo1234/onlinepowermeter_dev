
/*
 * LpdcLogic.cpp
 *
 *  Created on: 04-Feb-2023
 *      Author: MKS
 */
extern "C"{
#include "Struct_Input_output.h"
}
#include <BussinessLogic/ApplicationSW.h>
#include "main.h"
GPIO_PinState GPIO_PinStateMac;
GPIO_PinState GPIO_PinStateRej;

uint8_t MAC_A_Prod_Input1_StartTimer;
uint16_t MAC_A_Prod_Input1_CountBase;
uint16_t MAC_Gen_Prod_Input1_Production,Production_Zeit;
uint16_t MAC_Gen_Rej_Input_Production,Rejection_Zeit;
uint16_t Manual_RejectionCount;
uint8_t CurrentShift_K1;
uint8_t IsCurrentShiftUpdated;
uint8_t updateSetData;

/*Data shared btw server & client*/
//Client -server
uint8_t status_to_server;
uint16_t ProcessId_Value;
//server - client
extern uint8_t skipIdTCP;
extern uint8_t statusTCP;
extern uint16_t seq1_count_inc,seq2_count_inc;


uint16_t Seq1temperature;
uint16_t Seq2temperature;
uint8_t Seq1durationHr,Seq1durationMin,Seq2durationHr,Seq2durationMin;
uint16_t temperature_reference;
uint8_t start_process_control_timer;
uint8_t stop_timer_count;
uint8_t process_complete;
uint16_t seq1_duration_total,seq2_duration_total;

uint8_t debouncestart,debounceend;
uint8_t calculateDeltaWeight;
uint8_t negValue[10];
uint32_t carbonTargetWghtMem,silicaTargetWghtMem,manganeaseTargetWghtMem,copperTargetWghtMem,tinTargetWghtMem,zincTargetWghtMem;
uint32_t Carbon_calculated_Weight,Silica_calculated_Weight,manganease_calculated_Weight,Copper_calculated_Weight,Tin_calculated_Weight,Zinc_calculated_Weight;

Powermeter::Powermeter() {
	// TODO Auto-generated constructor stub

}

Powermeter::~Powermeter() {
	// TODO Auto-generated destructor stub
}

void Powermeter::run()
{
	m_selectframedata();
}

void Powermeter::m_selectframedata()
{



}


