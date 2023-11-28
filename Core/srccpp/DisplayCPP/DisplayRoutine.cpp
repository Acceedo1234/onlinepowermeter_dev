/*
 * DisplayRoutine.cpp
 *
 *  Created on: 01-Feb-2023
 *      Author: MKS
 */

#include "DisplayRoutine.h"
//#include <sstream.h>
uint8_t GLCDRefreshFlag;

extern uint8_t wifi_command;
extern uint8_t hour_t,min_t,sec_t;
extern uint8_t Wdate_Rtc,date_Rtc,month_Rtc,year_Rtc;
extern uint16_t MAC_Gen_Prod_Input1_Production,Production_Zeit;
extern uint16_t MAC_Gen_Rej_Input_Production,Rejection_Zeit;
extern uint16_t Manual_RejectionCount;
extern uint16_t SectorPos;

DisplayRoutine::DisplayRoutine() {
	// TODO Auto-generated constructor stub

}

DisplayRoutine::~DisplayRoutine() {
	// TODO Auto-generated destructor stub
}

void DisplayRoutine::run()
{
	//m_gLCDDrive.m_clrlcd();/*  Try in local if not need comment out*/
	m_gLCDDrive.m_lcdputs1(0,1,Lpdc);
	m_gLCDDrive.m_lcdputs1(0,3,production);
	m_gLCDDrive.m_lcdputs1(0,4,rejection);
	m_gLCDDrive.m_lcdputs1(0,7,boardID);

}

void DisplayRoutine::value2Digit(uint8_t value,uint8_t posLine,uint8_t posCol)
{
	GLCDNumericArray[0] =(unsigned char)(((value%100)/10)+0x30);
    GLCDNumericArray[1] =(unsigned char)((value%10)+0x30);
	m_gLCDDrive.m_lcdputs1(posCol,posLine,GLCDNumericArray);
}
void DisplayRoutine::value4Digit(uint16_t value4dig,uint8_t posLine,uint8_t posCol)
{
	gLCDNumericArraySize_4[0] =(unsigned char)(((value4dig%10000)/1000)+0x30);
	gLCDNumericArraySize_4[1] =(unsigned char)((value4dig%1000)/100+0x30);
	gLCDNumericArraySize_4[2] =(unsigned char)(((value4dig%100)/10)+0x30);
	gLCDNumericArraySize_4[3] =(unsigned char)((value4dig%10)+0x30);
	m_gLCDDrive.m_lcdputs1(posCol,posLine,gLCDNumericArraySize_4);
}

void DisplayRoutine::Init()
{
	m_gLCDDrive.m_displayon();
	m_gLCDDrive.m_setstartline(0);
	m_gLCDDrive.m_clrlcd();


}

