/*
 * UARTTxRxHandle.c
 *
 *  Created on: Jan 21, 2023
 *      Author: MKS
 */
#include"main.h"
#include "aco_generic_enumeration.h"
#include "struct.h"
#include"dwin_receive_decode.h"
#include <string.h>

extern UART_HandleTypeDef hlpuart1;
extern UART_HandleTypeDef huart1;

PowerMeterdata PowerMeterdatabase[16];
uint8_t rxTempBuff[5];
uint8_t checkbuff[200];
uint8_t refinc;
uint8_t rxMeterId;
uint8_t DataTrack=0;
uint8_t TotalNoOfData;
uint32_t carbonActWght,SilicaActWght,ManganeaseActWght,copperActWght,tinActWght,zincActWeight;
uint8_t CountTrack;

extern uint8_t RxDatabuf[50];
extern uint8_t modbustemp[3];
extern uint8_t u8rxbuf[255];
extern uint8_t Rxseqdecoder;
extern uint8_t Total_No_Of_Meter;
extern uint8_t CurrentFrame;
extern uint16_t Valid_No_Of_Data;
extern void ESPRxDecoder(unsigned char Rxwifi_data,unsigned char Rxseqdecoder);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &hlpuart1)
	{
		Dwin_Receive_Decode(rxTempBuff);
		HAL_UART_Receive_IT(&hlpuart1,rxTempBuff,1);
	}

	if(huart == &huart1)
	{
		HAL_UART_Receive_IT(&huart1,modbustemp,1);
		switch(DataTrack)
		{
			case 0:
				if(modbustemp[0] <= Total_No_Of_Meter){
					DataTrack=1;
					rxMeterId = modbustemp[0];
				}
			break;
			case 1:
				if(modbustemp[0]==3){
					DataTrack=2;
				}
				else
				{
					DataTrack=0;
				}
			break;
			case 2:
				if(modbustemp[0] == Valid_No_Of_Data*2)
				{
					TotalNoOfData = modbustemp[0];
					PowerMeterdatabase[rxMeterId-1].NoOfData[CurrentFrame-1]=TotalNoOfData;
					CountTrack=0;
					DataTrack=3;
				}
				else
				{
					CountTrack=0;
					DataTrack=0;
				}

			break;
			case 3:

				u8rxbuf[CountTrack] = modbustemp[0];
				CountTrack++;
				TotalNoOfData--;
				if(TotalNoOfData==0){
					DataTrack=4;
				}
			break;
			case 4:
				memcpy(&PowerMeterdatabase[rxMeterId-1].loc_datastore[CurrentFrame-1][0],u8rxbuf,100);
				memset(u8rxbuf,0,sizeof(u8rxbuf));
				DataTrack=0;
			break;
			default:
				DataTrack=0;
			break;

		}

	}
}
/*
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	HAL_UARTEx_ReceiveToIdle_IT(&huart1, RxDatabuf, 16);
}*/

