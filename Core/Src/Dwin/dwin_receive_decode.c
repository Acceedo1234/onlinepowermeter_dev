/*
 * dwin_receive_decode.c
 *
 *  Created on: Jan 4, 2024
 *      Author: MKS
 */
#include"main.h"
#include"dwin_receive_decode.h"
uint8_t Rx_Dwin_Data_Buff[50];
uint8_t Rx_Dwin_Point;
uint8_t No_Of_Dwin_Bytes;
uint8_t Rx_Dwin_Data_Buff_Point;
uint8_t Rx_Dwin_Complete;

void Dwin_Receive_Decode(uint8_t *rx_data)
{
	if((rx_data ==0x83)&&(Rx_Dwin_Point==0))
			{
				Rx_Dwin_Point=1;
			}
			else if(Rx_Dwin_Point==1)
			{
				if(rx_data == 0x30){
					Rx_Dwin_Point=2;
				}
				else{
					Rx_Dwin_Point=0;
				}
			}
			else if((rx_data == 0x00)&&(Rx_Dwin_Point==2))
			{
				Rx_Dwin_Point=3;
			}
			else if(Rx_Dwin_Point==3)
			{
				Rx_Dwin_Point=4;
				No_Of_Dwin_Bytes = (*rx_data)*2;
				Rx_Dwin_Data_Buff_Point = 0;
			}
			else if(Rx_Dwin_Point==4)
			{
				//Rx_Dwin_Point=4;
				Rx_Dwin_Data_Buff[Rx_Dwin_Data_Buff_Point]= rx_data;
				No_Of_Dwin_Bytes = No_Of_Dwin_Bytes-1;
				Rx_Dwin_Data_Buff_Point = Rx_Dwin_Data_Buff_Point+1;
				if(No_Of_Dwin_Bytes==0){
					Rx_Dwin_Complete = 1;
					Rx_Dwin_Point=0;
				}
			}
}


