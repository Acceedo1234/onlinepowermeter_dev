/*
 * esp8266_rx.c
 *
 *  Created on: Jan 21, 2023
 *      Author: MKS
 */
#include "esp8266_rx.h"
extern uint8_t checkbuff[200];
extern uint8_t refinc;
extern uint8_t RefreshBlockInfo;//update at rx end
uint8_t alarmOnOff;
uint8_t SW_Hour,SW_Minute;
uint8_t SW_Date,SW_Month,SW_Year;
uint16_t temperatureSetOL,temperaturehighSetOL,temperatureLowSetOL,productiontimeSetOL;
uint8_t productionhysPosSetOL,productionhysNegSetOL,SWCurrentShift,productChangeOL;

extern void W25qxx_ReadSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_SectorSize);
extern void W25qxx_WriteSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_SectorSize);
extern void W25qxx_EraseSector(uint32_t SectorAddr);

void ReadOnlineData(void);

void ESPRXDataSeg(void)
{
	uint16_t ReceivePLCData,ReceivePLCData2,ReceivePLCData3,ReceivePLCData4;
	uint16_t ReceivePLCData7;
	uint8_t ReceivePLCData11,ReceivePLCData12,ReceivePLCData13,ReceivePLCData14;
	uint8_t ReceivePLCData5,ReceivePLCData6,ReceivePLCData8,ReceivePLCData9,ReceivePLCData10;
	uint8_t UpdateOLdataMem  =0;
	uint8_t FlashWriteOLdata[10];
	if(!RxCompleteU2C1WIFI){return;}
	RxCompleteU2C1WIFI=0;

	ReceivePLCData  = Uart_rx_buffer[3]+(Uart_rx_buffer[2]*10)+(Uart_rx_buffer[1]*100)+(Uart_rx_buffer[0]*1000);//Metal Temperature set
	ReceivePLCData2 = Uart_rx_buffer[7]+(Uart_rx_buffer[6]*10)+(Uart_rx_buffer[5]*100)+(Uart_rx_buffer[4]*1000);//Metal Temperature high set
	ReceivePLCData3 = Uart_rx_buffer[11]+(Uart_rx_buffer[10]*10)+(Uart_rx_buffer[9]*100)+(Uart_rx_buffer[8]*1000); //Metal Temperature Low set
	ReceivePLCData4 = Uart_rx_buffer[15]+(Uart_rx_buffer[14]*10)+(Uart_rx_buffer[13]*100)+(Uart_rx_buffer[12]*1000);//Process Time
	//Date,Month,Year
	ReceivePLCData5 = Uart_rx_buffer[17]+(Uart_rx_buffer[16]*10);//SW_Date
	ReceivePLCData6 = Uart_rx_buffer[19]+(Uart_rx_buffer[18]*10);//SW_Month
	ReceivePLCData7 = Uart_rx_buffer[23]+(Uart_rx_buffer[22]*10)+(Uart_rx_buffer[21]*100)+(Uart_rx_buffer[20]*1000);//SW_Year
	ReceivePLCData8 = Uart_rx_buffer[25]+(Uart_rx_buffer[24]*10);//SW_Hour
	ReceivePLCData9 = Uart_rx_buffer[27]+(Uart_rx_buffer[26]*10);//SW_Minute
	//Currentshift_SW
	ReceivePLCData10 = Uart_rx_buffer[28];
	ReceivePLCData11 = Uart_rx_buffer[30]+(Uart_rx_buffer[29]*10);//Hys +
	ReceivePLCData12 = Uart_rx_buffer[32]+(Uart_rx_buffer[31]*10);//Hys -
	ReceivePLCData13 = Uart_rx_buffer[34]+(Uart_rx_buffer[33]*10);//Production reset status
	ReceivePLCData14 = Uart_rx_buffer[36]+(Uart_rx_buffer[35]*10);//Entry Status

	SW_Hour = ReceivePLCData8;
	SW_Minute = ReceivePLCData9;
	SW_Date = ReceivePLCData5;
	SW_Month = ReceivePLCData6;
	SW_Year = 23;

	if((ReceivePLCData != 0)&&(temperatureSetOL != ReceivePLCData)&&(ReceivePLCData<5000))
	{
		temperatureSetOL = ReceivePLCData;
		UpdateOLdataMem  =1;
	}
	if((ReceivePLCData2 != 0)&&(temperaturehighSetOL != ReceivePLCData2)&&(ReceivePLCData2<5000))
	{
		temperaturehighSetOL = ReceivePLCData2;
		UpdateOLdataMem  =1;
	}
	if((ReceivePLCData3 != 0)&&(temperatureLowSetOL != ReceivePLCData3)&&(ReceivePLCData3<5000))
	{
		temperatureLowSetOL  = ReceivePLCData3;
		UpdateOLdataMem  =1;
	}
	if((ReceivePLCData4 != 0)&&(productiontimeSetOL != ReceivePLCData4)&&(ReceivePLCData4<5000))
	{
		productiontimeSetOL  = ReceivePLCData4;
		UpdateOLdataMem  =1;
	}
	if((ReceivePLCData11 != 0) && (ReceivePLCData11 != productionhysPosSetOL)&&(ReceivePLCData11<=10))
	{
		productionhysPosSetOL= ReceivePLCData11;
		UpdateOLdataMem  =1;
	}
	if((ReceivePLCData12 != 0) && (ReceivePLCData12 != productionhysNegSetOL)&&(ReceivePLCData12<=10))
	{
		productionhysNegSetOL= ReceivePLCData12;
		UpdateOLdataMem  =1;
	}
	SWCurrentShift		 = ReceivePLCData10;
	productChangeOL		 = ReceivePLCData13;
	alarmOnOff			 = ReceivePLCData14;

	if(UpdateOLdataMem  == 1)
	{
		UpdateOLdataMem  =0;
		FlashWriteOLdata[0] = (uint8_t)temperatureSetOL&0x00ff;
		FlashWriteOLdata[1] = (uint8_t)(temperatureSetOL>>8)&0x00ff;
		FlashWriteOLdata[2] = (uint8_t)temperaturehighSetOL&0x00ff;
		FlashWriteOLdata[3] = (uint8_t)(temperaturehighSetOL>>8)&0x00ff;
		FlashWriteOLdata[4] = (uint8_t)temperatureLowSetOL&0x00ff;
		FlashWriteOLdata[5] = (uint8_t)(temperatureLowSetOL>>8)&0x00ff;
		FlashWriteOLdata[6] = (uint8_t)productiontimeSetOL&0x00ff;
		FlashWriteOLdata[7] = (uint8_t)(productiontimeSetOL>>8)&0x00ff;
		FlashWriteOLdata[8] = productionhysPosSetOL;
		FlashWriteOLdata[9] = productionhysNegSetOL;
		W25qxx_EraseSector(602);
		W25qxx_WriteSector(FlashWriteOLdata,602,0,10);
	}

}

void ReadOnlineData(void)
{
	uint8_t FlashReadOLdata[10];
	W25qxx_ReadSector(FlashReadOLdata,602,0,10);
	temperatureSetOL 		= (FlashReadOLdata[1]<<8|FlashReadOLdata[0]);
	temperaturehighSetOL 	= (FlashReadOLdata[3]<<8|FlashReadOLdata[2]);
	temperatureLowSetOL 	= (FlashReadOLdata[5]<<8|FlashReadOLdata[4]);
	productiontimeSetOL 	= (FlashReadOLdata[7]<<8|FlashReadOLdata[6]);
	productionhysPosSetOL 	= FlashReadOLdata[8];
	productionhysNegSetOL   = FlashReadOLdata[9];
}


void ESPRxDecoder(unsigned char Rxwifi_data,unsigned char Rxseqdecoder)
{

   static unsigned char Error_Retry;
	 switch(Rxseqdecoder)
	   {
	   	case 1:
			 if((Rxwifi_data=='O')&&(bufferptr==0))
			 {
			 	bufferptr=1;
			 }
			 else if((Rxwifi_data=='K')&&(bufferptr==1))
			 {
			 	bufferptr=0;
				Rxseqdecoder=0;
				wifi_command=30;
//				WifiStatusVar = 1;
				Check_CWMODE_For_Hang=0;
			 }
			 else{;}

			 if((Rxwifi_data=='n')&&(bufferptr==0))
			 {
			 	bufferptr=1;
			 }
			 else if((Rxwifi_data=='o')&&(bufferptr==1))
			 {
			 	bufferptr=2;
			 }
			 else if(bufferptr==2)
			 {
			 	bufferptr=3;
			 }
			 else if((Rxwifi_data=='c')&&(bufferptr==3))
			 {
			 	bufferptr=4;
			 }
			 else if((Rxwifi_data=='h')&&(bufferptr==4))
			 {
			 	bufferptr=5;
			 }
			 else if((Rxwifi_data=='a')&&(bufferptr==5))
			 {
			 	bufferptr=6;
			 }
			 else if((Rxwifi_data=='n')&&(bufferptr==6))
			 {
			 	bufferptr=7;
			 }
			 else if((Rxwifi_data=='g')&&(bufferptr==7))
			 {
			 	bufferptr=8;
			 }
			 else if((Rxwifi_data=='e')&&(bufferptr==8))
			 {
			 	bufferptr=0;
				Rxseqdecoder=0;
				wifi_command=30;
				Check_CWMODE_For_Hang=0;
			 }
			 else{;}
		break;
		case 2:
			if((Rxwifi_data=='O')&&(bufferptr==0))
			 {
			 	bufferptr=1;
			 }
			 else if((Rxwifi_data=='K')&&(bufferptr==1))
			 {
			 	bufferptr=0;
				Rxseqdecoder=0;
				wifi_command=49;
			 }
		break;
		case 3:
			if((Rxwifi_data=='T')&&(bufferptr==0))
			 {
			 	bufferptr=1;
			 }
			 else if((Rxwifi_data=='P')&&(bufferptr==1))
			 {
			 	bufferptr=2;
			 }
			 else if((Rxwifi_data=='-')&&(bufferptr==2))
			 {
			 	bufferptr=3;
			 }

			 else if((Rxwifi_data=='L')&&(bufferptr==3))
			 {
			 	bufferptr=4;
			 }
			 else if((Rxwifi_data=='I')&&(bufferptr==4))
			 {
			 	bufferptr=5;
			 }
			 else if((Rxwifi_data=='N')&&(bufferptr==5))
			 {
			 	bufferptr=6;
			 }
			 else if((Rxwifi_data=='K')&&(bufferptr==6))
			 {
			 	bufferptr=7;
			 }
			 else if((Rxwifi_data=='_')&&(bufferptr==7))
			 {
			 	bufferptr=8;
			 }
			 else if((Rxwifi_data=='B')&&(bufferptr==8))
			 {
			 	bufferptr=9;
			 }
			 else if((Rxwifi_data=='7')&&(bufferptr==9))
			 {
			 	bufferptr=10;
			 }
			 else if((Rxwifi_data=='A')&&(bufferptr==10))
			 {
			 	bufferptr=11;
			 }
			 else if((Rxwifi_data=='5')&&(bufferptr==11))
			 {
			 	bufferptr=12;
			 }
			 else if((Rxwifi_data=='0')&&(bufferptr==12))
			 {
			 	bufferptr=13;
			 }
			 else if((Rxwifi_data=='E')&&(bufferptr==13))
				 {
			 	bufferptr=0;
				Err_bufferptr=0;
				Rxseqdecoder=0;
				wifi_command=70;
				Error_Retry=0;
				WifiDisplay = 1;
//				WifiStatusVar = 2;
			 }


		/*	 if((Rxwifi_data=='t')&&(bufferptr==0))
			 {
			 	bufferptr=1;
			 }
			 else if((Rxwifi_data=='e')&&(bufferptr==1))
			 {
			 	bufferptr=2;
			 }
			 else if((Rxwifi_data=='s')&&(bufferptr==2))
			 {
			 	bufferptr=3;
			 }
			 else if((Rxwifi_data=='t')&&(bufferptr==3))
			 {
			 	bufferptr=0;
				Err_bufferptr=0;
				Rxseqdecoder=0;
				wifi_command=70;
				Error_Retry=0;
			 }	 */
			/* Rxwifi_data = UART_ubGetData8();
			 if((Rxwifi_data=='N')&&(bufferptr==0))
			 {
			 	bufferptr=1;
			 }
			 else if((Rxwifi_data=='O')&&(bufferptr==1))
			 {
			 	bufferptr=2;
			 }
			 else if((Rxwifi_data=='K')&&(bufferptr==2))
			 {
			 	bufferptr=3;
			 }
			 else if((Rxwifi_data=='I')&&(bufferptr==3))
			 {
			 	bufferptr=4;
			 }
			 else if((Rxwifi_data=='A')&&(bufferptr==4))
			 {
			 	bufferptr=0;
				Err_bufferptr=0;
				Rxseqdecoder=0;
				wifi_command=70;
				Error_Retry=0;
			 }	*/

			 if((Rxwifi_data=='N')&&(Err_bufferptr==0))
			 {
			 	Err_bufferptr=1;
			 }
			 else if((Rxwifi_data=='o')&&(Err_bufferptr==1))
			 {
			 	Err_bufferptr=2;
			 }
			 else if((Rxwifi_data=='A')&&(Err_bufferptr==2))
			 {
			 	Err_bufferptr=3;
			 }
			 else if((Rxwifi_data=='P')&&(Err_bufferptr==3))
			 {
			 	Err_bufferptr=0;
				bufferptr=0;
				Rxseqdecoder=0;
				WifiDisplay = 3;
				if(++Error_Retry<3)		//retry for error
				{
				   wifi_command=50;
				}
				else
				{
				   wifi_command=10;
				   Error_Retry=0;
				}
			 }
		break;
		case 4:	   //retry need to be added
			 if((Rxwifi_data=='O')&&(bufferptr==0))
			 {
			 	bufferptr=1;
			 }
			 else if((Rxwifi_data=='K')&&(bufferptr==1))
			 {
			 	bufferptr=0;
				Rxseqdecoder=0;
				wifi_command=80;
			 }
		break;
		case 5:

			if((Rxwifi_data=='C')&&(bufferptr==0))
			 {
			 	bufferptr=1;
			 }
			 else if((Rxwifi_data=='O')&&(bufferptr==1))
			 {
			 	bufferptr=2;
			 }
			 else if((Rxwifi_data=='N')&&(bufferptr==2))
			 {
			 	bufferptr=3;
			 }
			 else if((Rxwifi_data=='N')&&(bufferptr==3))
			 {
			 	bufferptr=4;
			 }
			 else if((Rxwifi_data=='E')&&(bufferptr==4))
			 {
			 	bufferptr=5;
			 }
			 else if((Rxwifi_data=='C')&&(bufferptr==5))
			 {
			 	bufferptr=6;
			 }
			 else if((Rxwifi_data=='T')&&(bufferptr==6))
			 {
			 	Rxseqdecoder=0;
				wifi_command=90;
				bufferptr=0;
				Error_Retry=0;
				Err_bufferptr=0; //wifi connected
			   WifiDisplay = 2;
			 }
			 	if((Rxwifi_data=='L')&&(bufferptr==0))	  //linked
			 {
			 	bufferptr=1;
			 }
			 else if((Rxwifi_data=='i')&&(bufferptr==1))
			 {
			 	bufferptr=2;
			 }
			 else if((Rxwifi_data=='n')&&(bufferptr==2))
			 {
			 	bufferptr=3;
			 }
			 else if((Rxwifi_data=='k')&&(bufferptr==3))
			 {
			 	bufferptr=4;
			 }
			 else if((Rxwifi_data=='e')&&(bufferptr==4))
			 {
			 	bufferptr=5;
			 }
			 else if((Rxwifi_data=='d')&&(bufferptr==5))
			 {
			 	Rxseqdecoder=0;
				wifi_command=90;
				bufferptr=0;
				Error_Retry=0;
				Err_bufferptr=0;
			 }
			 if((Rxwifi_data=='E')&&(Err_bufferptr==0))
			 {
			 	Err_bufferptr=1;
			 }
			 else if((Rxwifi_data=='R')&&(Err_bufferptr==1))
			 {
			 	Err_bufferptr=2;
			 }
			 else if((Rxwifi_data=='R')&&(Err_bufferptr==2))
			 {
			 	Err_bufferptr=3;
			 }
			 else if((Rxwifi_data=='O')&&(Err_bufferptr==3))
			 {
			 	Err_bufferptr=4;
			 }
			 else if((Rxwifi_data=='R')&&(Err_bufferptr==4))
			 {
			   	Err_bufferptr=0;
				bufferptr=0;
				Rxseqdecoder=0;
				WifiDisplay = 4;
				if(++Error_Retry<3)		//retry for error
				{
				   wifi_command=80;
				}
				else
				{
				   wifi_command=50;	 // modified by satheesh
				   Error_Retry=0;
				}
			 }
		break;
		case 6:
			 if((Rxwifi_data=='>')&&(bufferptr==0))
			 {
			 	bufferptr=0;
				Rxseqdecoder=0;
				wifi_command=100;
			 }
		break;
		case 7:
			checkbuff[refinc]= Rxwifi_data;
			refinc++;
			if((Rxwifi_data=='$')&&(Data_bufferptr==0))
			{
				 Data_bufferptr=1;
				 Valid_DataWifi1= 1;
			}
			else if(Data_bufferptr==1)
			 {
				Uart_rx_buffer[0] = DecToASCIIFun(Rxwifi_data);
				Data_bufferptr=2;
			}
			else if(Data_bufferptr==2)
			{
				 Uart_rx_buffer[1] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=3;
			}
			else if(Data_bufferptr==3)
			{
				 Uart_rx_buffer[2] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=4;
			}
			else if(Data_bufferptr==4)
			{
				 Uart_rx_buffer[3] = DecToASCIIFun(Rxwifi_data);		//Metal Temperature set
				 Data_bufferptr=5;
			}
			else if(Data_bufferptr==5)      //,(to seperate data)
			{
				 Data_bufferptr=6;
			}

			 else if(Data_bufferptr==6)       					  //Metal Temperature high set
			 {
				Uart_rx_buffer[4] = DecToASCIIFun(Rxwifi_data);
				Data_bufferptr=7;
			}
			else if(Data_bufferptr==7)
			{
				 Uart_rx_buffer[5] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=8;
			}
			else if(Data_bufferptr==8)
			{
				 Uart_rx_buffer[6] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=9;
			}
			else if(Data_bufferptr==9)
			{
				 Uart_rx_buffer[7] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=10;
			}
			else if(Data_bufferptr==10)      //,(to seperate data)
			{
				 Data_bufferptr=11;
			}

			else if(Data_bufferptr==11)       					  //Metal Temperature Low set
			 {
				Uart_rx_buffer[8] = DecToASCIIFun(Rxwifi_data);
				Data_bufferptr=12;
			}
			else if(Data_bufferptr==12)
			{
				 Uart_rx_buffer[9] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=13;
			}
			else if(Data_bufferptr==13)
			{
				 Uart_rx_buffer[10] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=14;
			}
			else if(Data_bufferptr==14)
			{
				 Uart_rx_buffer[11] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=15;
			}
			else if(Data_bufferptr==15)      //,(to seperate data)
			{
				 Data_bufferptr=16;
			}

			else if(Data_bufferptr==16)       				  //Process Time
			 {
				Uart_rx_buffer[12] = DecToASCIIFun(Rxwifi_data);
				Data_bufferptr=17;
			}
			else if(Data_bufferptr==17)
			{
				 Uart_rx_buffer[13] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=18;
			}
			else if(Data_bufferptr==18)
			{
				 Uart_rx_buffer[14] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=19;
			}
			else if(Data_bufferptr==19)
			{
				 Uart_rx_buffer[15] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=20;
			}
			else if(Data_bufferptr==20)      //,(to seperate data)
			{
				 Data_bufferptr=21;
			}

			else if(Data_bufferptr==21)       	//system date
			 {
				Uart_rx_buffer[16] = DecToASCIIFun(Rxwifi_data);
				Data_bufferptr=22;
			}
			else if(Data_bufferptr==22)
			{
				 Uart_rx_buffer[17] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=23;
			}
			else if(Data_bufferptr==23)      //,(to seperate data)
			{
				 Data_bufferptr=24;
			}
			else if(Data_bufferptr==24)		 //System Month
			{
				 Uart_rx_buffer[18] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=25;
			}
			else if(Data_bufferptr==25)
			{
				 Uart_rx_buffer[19] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=26;
			}
			else if(Data_bufferptr==26)      //,(to seperate data)
			{
				 Data_bufferptr=27;
			}
			else if(Data_bufferptr==27)      //System year
			 {
				Uart_rx_buffer[20] = DecToASCIIFun(Rxwifi_data);
				Data_bufferptr=28;
			}
			else if(Data_bufferptr==28)
			{
				 Uart_rx_buffer[21] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=29;
			}
			else if(Data_bufferptr==29)
			{
				 Uart_rx_buffer[22] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=30;
			}
			else if(Data_bufferptr==30)
			{
				 Uart_rx_buffer[23] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=31;
			}
			else if(Data_bufferptr==31)	   //,(to seperate data)
			{
				 Data_bufferptr=32;
			}
			else if(Data_bufferptr==32)	   //System Hour
			{
				 Uart_rx_buffer[24] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=33;
			}
			else if(Data_bufferptr==33)
			{
				 Uart_rx_buffer[25] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=34;
			}
			else if(Data_bufferptr==34)      //,(to seperate data)
			{
				 Data_bufferptr=35;
			}
			else if(Data_bufferptr==35)      //System Minute
			 {
				Uart_rx_buffer[26] = DecToASCIIFun(Rxwifi_data);
				Data_bufferptr=36;
			}
			else if(Data_bufferptr==36)
			{
				 Uart_rx_buffer[27] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=37;
			}
			else if(Data_bufferptr==37)		  //,(to seperate data)
			{
				 Data_bufferptr=38;
			}
			else if(Data_bufferptr==38)		  //SW_CurrentShift
			{
				 Uart_rx_buffer[28] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=39;
			}
			else if(Data_bufferptr==39)        //,
			{
				 Data_bufferptr=40;
			}
			else if(Data_bufferptr==40)      //Pos adj
			{
				Uart_rx_buffer[29] = DecToASCIIFun(Rxwifi_data);
				Data_bufferptr=41;
			}
			else if(Data_bufferptr==41)
			{
				 Uart_rx_buffer[30] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=42;
			}
			else if(Data_bufferptr==42)	   //,
			{
				 Data_bufferptr=43;
			}
			else if(Data_bufferptr==43)	   //Neg adj
			{
				 Uart_rx_buffer[31] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=44;
			}
			else if(Data_bufferptr==44)
			{
				 Uart_rx_buffer[32] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=45;
			}
			else if(Data_bufferptr==45)    //,
			 {
				Data_bufferptr=46;
			}
			else if(Data_bufferptr==46)	   //Clearindication
			{
				 Uart_rx_buffer[33] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=47;
			}
			else if(Data_bufferptr==47)
			{
				 Uart_rx_buffer[34] = DecToASCIIFun(Rxwifi_data);
				 Data_bufferptr=48;
			}
			else if(Data_bufferptr==48)	 //,
			{
				 Data_bufferptr=49;
			}
			else if(Data_bufferptr==49)      //Alarmstatus
			{
				 Data_bufferptr=50;
				 Uart_rx_buffer[35] = DecToASCIIFun(Rxwifi_data);
			}
			else if(Data_bufferptr==50)
			 {
				Uart_rx_buffer[36] = DecToASCIIFun(Rxwifi_data);
				Data_bufferptr=0;
				RefreshBlockInfo = 0;
				RxCompleteU2C1WIFI=1;
				Updatetimeinfo=1;
			}
			else if(Data_bufferptr==51)      //Dummy
			{
				 Data_bufferptr=0;
			}

		   if(Valid_DataWifi1)
		   {
			 if((Rxwifi_data=='U')&&(bufferptr==0))
			 {
			 	bufferptr=1;
			 }
			 else if((Rxwifi_data=='n')&&(bufferptr==1))
			 {
			 	bufferptr=2;
			 }
			 else if((Rxwifi_data=='l')&&(bufferptr==2))
			 {
			    bufferptr=3;
			 }
			 else if((Rxwifi_data=='i')&&(bufferptr==3))
			 {
			    bufferptr=4;
			 }
			 else if((Rxwifi_data=='n')&&(bufferptr==4))
			 {
			    bufferptr=5;
			 }
			 else if((Rxwifi_data=='k')&&(bufferptr==5))
			 {
			    bufferptr=0;
				Rxseqdecoder=0;
				wifi_command=102;
				//RefreshBlockInfo=0;//update at rx end
				Error_Close=0;
				Valid_DataWifi1=0;
				//RxCompleteU2C1WIFI=1;			  /*receive complete*/
			 }
			 if((Rxwifi_data=='C')&&(bufferptr==0))
			 {
			 	bufferptr=1;
			 }
			 else if((Rxwifi_data=='L')&&(bufferptr==1))
			 {
			 	bufferptr=2;
			 }
			 else if((Rxwifi_data=='O')&&(bufferptr==2))
			 {
			    bufferptr=3;
			 }
			 else if((Rxwifi_data=='S')&&(bufferptr==3))
			 {
			    bufferptr=4;
			 }
			 else if((Rxwifi_data=='E')&&(bufferptr==4))
			 {
			    bufferptr=5;
			 }
			 else if((Rxwifi_data=='D')&&(bufferptr==5))
			 {
			    bufferptr=0;
				Rxseqdecoder=0;
				wifi_command=102;
				//RefreshBlockInfo=0;//update at rx end
				Error_Close=0;
				Valid_DataWifi1=0;
				//RxCompleteU2C1WIFI=1;				  /*receive complete*/
			 }
			}
			else
			{
			  if((Rxwifi_data=='U')&&(bufferptr==0))
			 {
			 	bufferptr=1;
			 }
			 else if((Rxwifi_data=='n')&&(bufferptr==1))
			 {
			 	bufferptr=2;
			 }
			 else if((Rxwifi_data=='l')&&(bufferptr==2))
			 {
			    bufferptr=3;
			 }
			 else if((Rxwifi_data=='i')&&(bufferptr==3))
			 {
			    bufferptr=4;
			 }
			 else if((Rxwifi_data=='n')&&(bufferptr==4))
			 {
			    bufferptr=5;
			 }
			 else if((Rxwifi_data=='k')&&(bufferptr==5))
			 {
			    bufferptr=0;
				Rxseqdecoder=0;
				wifi_command=102;
				//RefreshBlockInfo=0;//update at rx end
				Error_Close=0;
				Valid_DataWifi1=0;
				//RxCompleteU2C1WIFI=1;				    /*receive complete*/
			 }

			   if((Rxwifi_data=='C')&&(bufferptr==0))
			 {
			 	bufferptr=1;
			 }
			 else if((Rxwifi_data=='L')&&(bufferptr==1))
			 {
			 	bufferptr=2;
			 }
			 else if((Rxwifi_data=='O')&&(bufferptr==2))
			 {
			    bufferptr=3;
			 }
			 else if((Rxwifi_data=='S')&&(bufferptr==3))
			 {
			    bufferptr=4;
			 }
			 else if((Rxwifi_data=='E')&&(bufferptr==4))
			 {
			    bufferptr=5;
			 }
			 else if((Rxwifi_data=='D')&&(bufferptr==5))
			 {
			    bufferptr=0;
				Rxseqdecoder=0;
				if(++Error_Close>3){wifi_command=10;Error_Close=0;}
				else{
					//RefreshBlockInfo=0;//update at rx end
					wifi_command=102;}
				Valid_DataWifi1=0;
				//RxCompleteU2C1WIFI=0;   /*Wrong receive*/
			 }
			}
		break;
		case 8:
			 if((Rxwifi_data=='O')&&(bufferptr==0))
			 {
			 	bufferptr=1;
			 }
			 else if((Rxwifi_data=='K')&&(bufferptr==1))
			 {
			 	bufferptr=0;
				Rxseqdecoder=0;
				wifi_command=20;
			 }
		break;
	   	default:
		break;
		}
}

unsigned char DecToASCIIFun(unsigned char TempVar)
{
	unsigned char  ASCIIHex;
	switch(TempVar)
	{
	case 48:
			ASCIIHex = 0x00;
			break;
	case 49:
			ASCIIHex = 0x01;
			break;
	case 50:
			ASCIIHex = 0x02;
			break;
	case 51:
			ASCIIHex = 0x03;
			break;
	case 52:
			ASCIIHex = 0x04;
			break;
	case 53:
			ASCIIHex = 0x05;
			break;
	case 54:
			ASCIIHex = 0x06;
			break;
	case 55:
			ASCIIHex = 0x07;
			break;
	case 56:
			ASCIIHex = 0x08;
			break;
	case 57:
			ASCIIHex = 0x09;
			break;
	case 65:
			ASCIIHex = 0x0A;
			break;
	case 66:
			ASCIIHex = 0x0B;
			break;
	case 67:
			ASCIIHex = 0x0C;
			break;
	case 68:
			ASCIIHex = 0x0D;
			break;
	case 69:
			ASCIIHex = 0x0E;
			break;
	case 70:
			ASCIIHex = 0x0F;
			break;
	}
	return (ASCIIHex);
}




