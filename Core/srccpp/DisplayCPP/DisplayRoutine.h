/*
 * DisplayRoutine.h
 *
 *  Created on: 01-Feb-2023
 *      Author: MKS
 */

#ifndef SRCCPP_DISPLAYCPP_DISPLAYROUTINE_H_
#define SRCCPP_DISPLAYCPP_DISPLAYROUTINE_H_
#include "GLCD.h"


class DisplayRoutine {
public:
	DisplayRoutine();
	virtual ~DisplayRoutine();
	void run();
	void Init();

private:
	GLCD m_gLCDDrive;
	unsigned char Lpdc[15]="   POWERMETER";
	unsigned char production[21]="No of METER:16";
	unsigned char rejection[21] ="192.168.201.54";
	unsigned char boardID[13]="BoardId:3";
	void value2Digit(uint8_t value,uint8_t posLine,uint8_t posCol);
	void value4Digit(uint16_t value4dig,uint8_t posLine,uint8_t posCol);
	uint8_t GLCDNumericArray[2];
	uint8_t gLCDNumericArraySize_4[4];

};

#endif /* SRCCPP_DISPLAYCPP_DISPLAYROUTINE_H_ */
