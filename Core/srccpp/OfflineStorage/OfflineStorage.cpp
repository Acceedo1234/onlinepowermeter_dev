/*
 * OfflineStorage.cpp
 *
 *  Created on: 09-Feb-2023
 *      Author: MKS
 */
#include "main.h"
#include "OfflineStorage.h"
#include <stdio.h>
#include <string.h>
using namespace std;

#define MAXNOOFBLOCK 32

extern "C" {
void W25qxx_WriteSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_SectorSize);
void W25qxx_ReadSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_SectorSize);
void W25qxx_EraseSector(uint32_t SectorAddr);
bool W25qxx_IsEmptyBlock(uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_BlockSize);
void W25qxx_EraseBlock(uint32_t BlockAddr);
void W25qxx_EraseChip(void);
}

uint16_t SectorPos;

/*Data need to saved in flash*/
extern uint8_t Ip_config_Ip[4],Ip_Config_Subnet[4],Ip_config_gateway[4],Ip_config_DNS[4],Ip_config_Server[4];
extern uint16_t Ip_config_Server_Port;
/*check for change in data*/
extern uint8_t Update_Dwin_Set_Data;
extern uint8_t No_Of_Meter;
extern uint16_t Ip_config_Server_Port_K1;
extern uint8_t No_Of_Meter_K1;


OfflineStorage::OfflineStorage() {
	// TODO Auto-generated constructor stub

}

OfflineStorage::~OfflineStorage() {
	// TODO Auto-generated destructor stub
}

void OfflineStorage::run()
{
	m_writesetdata();

}

void OfflineStorage::m_writesetdata(){
		if(Update_Dwin_Set_Data)
		{
			Update_Dwin_Set_Data=0;
			W25qxx_EraseSector(100);
			m_writeDwinBuf[0] = Ip_config_Ip[0];
			m_writeDwinBuf[1] = Ip_config_Ip[1];
			m_writeDwinBuf[2] = Ip_config_Ip[2];
			m_writeDwinBuf[3] = Ip_config_Ip[3];

			m_writeDwinBuf[4] = Ip_Config_Subnet[0];
			m_writeDwinBuf[5] = Ip_Config_Subnet[1];
			m_writeDwinBuf[6] = Ip_Config_Subnet[2];
			m_writeDwinBuf[7] = Ip_Config_Subnet[3];

			m_writeDwinBuf[8] = Ip_config_gateway[0];
			m_writeDwinBuf[9] = Ip_config_gateway[1];
			m_writeDwinBuf[10] = Ip_config_gateway[2];
			m_writeDwinBuf[11] = Ip_config_gateway[3];

			m_writeDwinBuf[12] = Ip_config_DNS[0];
			m_writeDwinBuf[13] = Ip_config_DNS[1];
			m_writeDwinBuf[14] = Ip_config_DNS[2];
			m_writeDwinBuf[15] = Ip_config_DNS[3];

			m_writeDwinBuf[16] = Ip_config_Server[0];
			m_writeDwinBuf[17] = Ip_config_Server[1];
			m_writeDwinBuf[18] = Ip_config_Server[2];
			m_writeDwinBuf[19] = Ip_config_Server[3];

			m_writeDwinBuf[20] = (unsigned char)(Ip_config_Server_Port>>8)&0x00ff;
			m_writeDwinBuf[21] = (unsigned char)(Ip_config_Server_Port)&0x00ff;

			m_writeDwinBuf[22] = No_Of_Meter;
			W25qxx_WriteSector(m_writeDwinBuf,100,0,23);
		}
}


void OfflineStorage::m_readsetdata(){
	W25qxx_ReadSector(m_readFlashBuf,100,0,23);

		 Ip_config_Ip[0] = m_readFlashBuf[0];
		 Ip_config_Ip[1] = m_readFlashBuf[1];
		 Ip_config_Ip[2] = m_readFlashBuf[2];
		 Ip_config_Ip[3] = m_readFlashBuf[3];

		 Ip_Config_Subnet[0]= m_readFlashBuf[4];
		 Ip_Config_Subnet[1]= m_readFlashBuf[5];
		 Ip_Config_Subnet[2]= m_readFlashBuf[6];
		 Ip_Config_Subnet[3]= m_readFlashBuf[7];

		 Ip_config_gateway[0] = m_readFlashBuf[8];
		 Ip_config_gateway[1] = m_readFlashBuf[9];
		 Ip_config_gateway[2] = m_readFlashBuf[10];
		 Ip_config_gateway[3] = m_readFlashBuf[11];

		 Ip_config_DNS[0] = m_readFlashBuf[12];
		 Ip_config_DNS[1] = m_readFlashBuf[13];
		 Ip_config_DNS[2] = m_readFlashBuf[14];
		 Ip_config_DNS[3] = m_readFlashBuf[15];

		 Ip_config_Server[0] = m_readFlashBuf[16];
		 Ip_config_Server[1] = m_readFlashBuf[17];
		 Ip_config_Server[2] = m_readFlashBuf[18];
		 Ip_config_Server[3] = m_readFlashBuf[19];

		 Ip_config_Server_Port 		= ((m_readFlashBuf[20]<<8)|(m_readFlashBuf[21]));
		 Ip_config_Server_Port_K1 	= Ip_config_Server_Port;
		 No_Of_Meter 				= m_readFlashBuf[22];
		 No_Of_Meter_K1 			= No_Of_Meter;
}

void OfflineStorage::ReadOfflinedataInit()
{
	m_readsetdata();
}

void OfflineStorage::ECUProductionInit(void)
{

}

