/*
 * Modbusrtu.cpp
 *
 *  Created on: 07-Jan-2023
 *      Author: MKS
 */

#include "Modbusrtu.h"
#include "Modbus_types.h"
#include "aco_generic_enumeration.h"
#include <struct.h>
#include <string.h>


#define READINPUTREG 0x04
#define STARTADDRESS 0x03
#define NOOFDATA     0x02

constexpr uint8_t START_BYTE_1=0x5A;
constexpr uint8_t START_BYTE_2=0xA5;
constexpr uint8_t multipleWriteRequestH = 0x0B;
constexpr uint8_t multipleWriteRequestL = 0x82;
constexpr uint8_t multipleReadRequestH = 0x04;
constexpr uint8_t multipleReadRequestL = 0x83;

extern uint16_t temperatureSetOL,temperaturehighSetOL,temperatureLowSetOL;
extern uint8_t Total_No_Of_Meter;
extern uint8_t Rx_Dwin_Point;
extern struct ModbusRegValue MeterInfo[16];
extern uint8_t RxDatabuf[50];
extern uint8_t httpc_isConnected;

uint8_t test;
uint8_t u8ModbusReg[8];
uint8_t Seqcntrl=0;
uint8_t TxSeqComplete;
uint8_t CurrentMeter=1;
uint8_t CurrentFrame=1;
uint8_t noOfDataDwin;
uint16_t RxNoOfData;
uint16_t Valid_No_Of_Data;

Modbusrtu::Modbusrtu() {
	// TODO Auto-generated constructor stub

}

Modbusrtu::~Modbusrtu() {
	// TODO Auto-generated destructor stub
}

uint8_t Modbusrtu::getReadInputReg() const
{
	return READINPUTREG;
}

uint16_t Modbusrtu::getStartAddress() const
{
	return STARTADDRESS;
}

uint16_t Modbusrtu::getNoOfData() const
{
	return NOOFDATA;
}

void Modbusrtu::ModbusReadTransaction(void)
{
	switch(Seqcntrl){
	case 0:/*Only for Init*/
			CurrentMeter=1;
			CurrentFrame=1;
			Seqcntrl=1;
	break;
	case 1:
		++CurrentFrame;
		if(CurrentFrame > MeterInfo[CurrentMeter-1].NoOfFrame){
			CurrentMeter++;
			CurrentFrame=1;
		}
		if(CurrentMeter > Total_No_Of_Meter){
			CurrentMeter=1;
			CurrentFrame=1;
		}
		Seqcntrl=1;
	break;
	default:
	break;
	}

	u8ModbusReg[0] = CurrentMeter;
	u8ModbusReg[1] = 0x03;
	u8ModbusReg[2] = static_cast<uint8_t>((MeterInfo[CurrentMeter-1].ModbusFrameRegInfo[CurrentFrame-1].FrameStartAddress& 0xff00)>>8);
	u8ModbusReg[3] = static_cast<uint8_t>(MeterInfo[CurrentMeter-1].ModbusFrameRegInfo[CurrentFrame-1].FrameStartAddress & 0x00ff);
	u8ModbusReg[4] = static_cast<uint8_t>((MeterInfo[CurrentMeter-1].ModbusFrameRegInfo[CurrentFrame-1].FrameNoOfData & 0xff00)>>8);
	u8ModbusReg[5] = static_cast<uint8_t>(MeterInfo[CurrentMeter-1].ModbusFrameRegInfo[CurrentFrame-1].FrameNoOfData & 0x00ff);
	Valid_No_Of_Data = MeterInfo[CurrentMeter-1].ModbusFrameRegInfo[CurrentFrame-1].FrameNoOfData;
	u16CRC 		   = ASCChecksum(u8ModbusReg,6);
	u8ModbusReg[6] = static_cast<uint8_t>(u16CRC & 0x00ff);
	u8ModbusReg[7] = static_cast<uint8_t>((u16CRC & 0xff00)>>8);
	RxNoOfData = 5+(MeterInfo[CurrentMeter-1].ModbusFrameRegInfo[CurrentFrame-1].FrameNoOfData*2);
	//memset(u8rxbuf,0,sizeof(u8rxbuf));
	//memset(RxDatabuf,0,sizeof(RxDatabuf));

	HAL_UART_Transmit_IT(&huart1,u8ModbusReg,sizeof(u8ModbusReg));

}
//Hardware callback
/*
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_IT(&huart1,u8rxbuf,RxNoOfData);
}*/

uint16_t Modbusrtu::ASCChecksum(uint8_t *ASCSrc, uint8_t NoOfBytes)
{
	uint8_t i, CheckSumBytes;
	uint8_t CRCRegLow = 0xff;
	uint8_t CRCRegHigh = 0xff;
	uint8_t CRCIndex;

	CheckSumBytes = NoOfBytes;
   	for(i=0;i < CheckSumBytes;i++)
   	{
	    CRCIndex = CRCRegLow ^ *ASCSrc++; 				//TransmittingData[i];
		CRCRegLow = CRCRegHigh ^ CRCArrayHigh[CRCIndex];
		CRCRegHigh = CRCArrayLow[CRCIndex];

	}
	return (CRCRegHigh << 8 | CRCRegLow );
}

void Modbusrtu::dwinFrame(void)
{
	test=test+1;
	switch(cntIdDwin)
	{
	case 0:
		u8DwinRegister[0] = START_BYTE_1;
		u8DwinRegister[1] = START_BYTE_2;
		u8DwinRegister[2] = multipleWriteRequestH;
		u8DwinRegister[3] = multipleWriteRequestL;
		u8DwinRegister[4] = 0x20;
		u8DwinRegister[5] = 0x00;
		u8DwinRegister[6] = 0x00;
		u8DwinRegister[7] = test;
		u8DwinRegister[8] = 0;
		u8DwinRegister[9] = httpc_isConnected;
		u8DwinRegister[10] = 0;
		u8DwinRegister[11] = 1;
		noOfDataDwin=12;
		cntIdDwin=1;
	break;
	case 1:
		u8DwinRegister[0] = START_BYTE_1;
		u8DwinRegister[1] = START_BYTE_2;
		u8DwinRegister[2] = multipleWriteRequestH;
		u8DwinRegister[3] = multipleWriteRequestL;
		u8DwinRegister[4] = 0x20;
		u8DwinRegister[5] = 0x00;
		u8DwinRegister[6] = 0x00;
		u8DwinRegister[7] = test;
		u8DwinRegister[8] = 0;
		u8DwinRegister[9] = httpc_isConnected;
		u8DwinRegister[10] = 0;
		u8DwinRegister[11] = 1;
		noOfDataDwin=12;
		cntIdDwin=2;
	break;
	case 2:
		u8DwinRegister[0] = START_BYTE_1;
		u8DwinRegister[1] = START_BYTE_2;
		u8DwinRegister[2] = multipleReadRequestH;
		u8DwinRegister[3] = multipleReadRequestL;
		u8DwinRegister[4] = 0x30;
		u8DwinRegister[5] = 0x00;
		u8DwinRegister[6] = 0x16;
		Rx_Dwin_Point=0;
		noOfDataDwin=7;
		cntIdDwin=0;
	break;
	default:
		cntIdDwin=0;
	break;
	}
	//out_read_rxint_set.Noofbytesrx = (_u16ReadQty*2)+5;
	HAL_UART_Transmit_IT(&hlpuart1,u8DwinRegister,noOfDataDwin);
}
