/*
 * struct.h
 *
 *  Created on: May 31, 2023
 *      Author: DINESH
 */

#ifndef INC_STRUCT_H_
#define INC_STRUCT_H_
#include "main.h"
typedef struct {
	uint16_t FrameStartAddress;
	uint16_t FrameNoOfData;
}ModbusRegInfo;

struct ModbusRegValue{
	uint8_t NoOfFrame;
	ModbusRegInfo ModbusFrameRegInfo[5];

};

typedef struct {
	uint8_t loc_datastore[4][200];
	uint8_t NoOfData[4];
}PowerMeterdata;






#endif /* INC_STRUCT_H_ */
