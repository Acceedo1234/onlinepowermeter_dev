/*
 * w5500_spi.c
 *
 *  Created on: Jan 10, 2023
 *      Author: MKS
 */

#include "main.h"
#include "wizchip_conf.h"
#include "stdio.h"
#include "socket.h"
#include "dhcp.h"
#include "dns.h"
#include <stdio.h>
#include "httpClient.h"
#include "struct.h"


#define SOCK_DHCP               3
#define SOCK_DNS                4
#define DATA_BUF_SIZE	2048

wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x03, 0x09,0x05, 0x00, 0x03},
                            .ip = {192, 168, 0, 195},
                            .sn = {255,255,255,0},
                            .gw = {0, 0, 0, 0},
                            .dns = {0,0,0,0},
                            .dhcp = NETINFO_STATIC };

wiz_NetInfo checkgWIZNETINFO;
extern PowerMeterdata PowerMeterdatabase[16];

#define DHCP_SOCKET 0
#define DNS_SOCKET 1
#define HTTP_SOCKET 2

uint8_t flag_sent_http_request = DISABLE;
uint8_t connectionCheck= HTTPC_FALSE;

uint8_t g_send_buf[DATA_BUF_SIZE];
uint8_t g_recv_buf[DATA_BUF_SIZE];

uint8_t data_buf [DATA_BUF_SIZE];
// 1K should be enough, see https://forum.wiznet.io/t/topic/1612/2
uint8_t dhcp_buffer [ 1024 ] ;
// 1K seems to be enough for this buffer as well
uint8_t dns_buffer [ 1024 ] ;

//uint8_t Domain_IP[4]  = {20,187,110,12};
//uint8_t Domain_IP[4]  = {192,168,0,121};
uint8_t Domain_IP[4]  = {122,165,215,149};

uint8_t Domain_name[] = "isc2.power-meter.acceedo.in";//
//uint8_t URI[] = "http://usm3-ht.acceedo.in:9005/set_temp?u=5&p=00000001&tm=0030&tl=0320&th=0030&h=030&l=080&ht=070&lt=080&rv=0130&bv=0650&yv=1230&rc=2705&bc=2909&yc=0500&k=203040&x=1";
//uint8_t URI[] = "/search?ei=BkGsXL63AZiB-QaJ8KqoAQ&q=W6100&oq=W6100&gs_l=psy-ab.3...0.0..6208...0.0..0.0.0.......0......gws-wiz.eWEWFN8TORw";
uint8_t URI[500];
extern SPI_HandleTypeDef hspi2;
#define _W5500_SPI       hspi2
uint16_t wiz5500_var;
uint8_t statusChipInit,statusPhysLink;
uint8_t retDHCPstatus;

/*Client to server exchange*/
uint16_t act_temperature_c1,act_temperature_c2,act_temperature_c3,act_temperature_c4;
extern uint16_t Seq1temperature;
extern uint16_t Seq2temperature;
extern uint8_t Seq1durationHr,Seq1durationMin,Seq2durationHr,Seq2durationMin;
extern uint8_t seq1_remaining_time_Hr,seq1_remaining_time_min,seq2_remaining_time_Hr,seq2_remaining_time_min;
extern uint8_t status_to_server;
extern uint16_t ProcessId_Value;
uint8_t dataURI[800];

uint16_t set_duration_seq1,set_duration_seq2;
uint16_t remaining_duration_seq1,remaining_duration_seq2;
uint8_t Meter_Id;
uint16_t SizeOfDataURI,length1,length2,length3,length4,length5;
uint16_t x,k,l;

void processDHCP(void);

void ethernetHTTPRoutine(void);

void initializeHttp(void);

void wizchip_select(void)
{
	HAL_GPIO_WritePin(GPIOB, W5500_CS_Pin, GPIO_PIN_RESET);
}

void wizchip_deselect(void)
{
	HAL_GPIO_WritePin(GPIOB, W5500_CS_Pin, GPIO_PIN_SET);
}

void W5500_ReadBuff(uint8_t* buff,uint16_t len​​)
{
    HAL_SPI_Receive(&_W5500_SPI,buff,len​​,HAL_MAX_DELAY) ;
}

void W5500_WriteBuff(uint8_t* buff,uint16_t len)
{
    HAL_SPI_Transmit(&_W5500_SPI,buff,len,HAL_MAX_DELAY) ;
}

uint8_t W5500_Spi(uint8_t Data)
{
	uint8_t ret;
	HAL_SPI_TransmitReceive(&_W5500_SPI, &Data, &ret, 1, 100);
	return ret;
}
/*
void spiWriteByte(uint8_t tx)
{
	uint8_t rx;
	HAL_SPI_TransmitReceive(&_W5500_SPI, &tx, &rx, 1, 10);
}

uint8_t spiReadByte(void)
{
	uint8_t rx = 0, tx = 0xFF;
	HAL_SPI_TransmitReceive(&_W5500_SPI, &tx, &rx, 1, 10);
	return rx;
}*/
/*
uint8_t W5500_ReadByte ( void ) {
    uint8_t byte ;
    W5500_ReadBuff (& byte , sizeof ( byte ) ) ;
    return byte ;
}

void W5500_WriteByte ( uint8_t byte ) {
    W5500_WriteBuff ( & byte , sizeof ( byte ) ) ;
}
*/
volatile uint8_t ip_assigned = 0;

void Callback_IPAssigned(void) {
    ip_assigned = 1;
}

void Callback_IPConflict(void) {

}
/*UDP
void wiz5500Init(void)
{

	//hardware reset the module
	HAL_GPIO_WritePin(W5500RST_GPIO_Port, W5500RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(15000);
	HAL_GPIO_WritePin(W5500RST_GPIO_Port, W5500RST_Pin, GPIO_PIN_SET);

	reg_wizchip_cs_cbfunc(wizchip_select,wizchip_deselect);
	reg_wizchip_spi_cbfunc(W5500_Spi,W5500_Spi);
	reg_wizchip_spiburst_cbfunc(W5500_ReadBuff,W5500_WriteBuff);
	wiz5500_var = 0;
	uint8_t rx_tx_buff_sizes [ ] = { 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 } ;
	wizchip_init ( rx_tx_buff_sizes , rx_tx_buff_sizes ) ;


	wiz_NetInfo net_info = {
	    . mac  = { 0xEA , 0x11 , 0x22 , 0x33 , 0x44 , 0xEA } ,
	    . dhcp = NETINFO_DHCP
	} ;
	// set MAC address before using DHCP
	setSHAR ( net_info. mac ) ;
	DHCP_init ( DHCP_SOCKET , dhcp_buffer ) ;

	reg_dhcp_cbfunc (
	    Callback_IPAssigned ,
	    Callback_IPAssigned ,
	    Callback_IPConflict
	) ;

	uint32_t ctr = 10000 ;
	while ( ( ! ip_assigned ) && ( ctr > 0 ) ) {
	    DHCP_run ( ) ;
	    ctr --;
	}
	if ( ! ip_assigned ) {
	    return ;
	}

	getIPfromDHCP(net_info.ip);
	getGWfromDHCP(net_info.gw);
	getSNfromDHCP(net_info.sn);
	uint8_t dns[4];
	getDNSfromDHCP(dns);

	wizchip_setnetinfo ( & net_info ) ;
}*/


void wiz5500Init(void)
{

	uint8_t memsize[2][8] = { {2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
	uint8_t tmp;
	//hardware reset the module
	HAL_GPIO_WritePin(W5500RST_GPIO_Port, W5500RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(15000);
	HAL_GPIO_WritePin(W5500RST_GPIO_Port, W5500RST_Pin, GPIO_PIN_SET);

	reg_wizchip_cs_cbfunc(wizchip_select,wizchip_deselect);
	reg_wizchip_spi_cbfunc(W5500_Spi,W5500_Spi);
//	reg_wizchip_spiburst_cbfunc(W5500_ReadBuff,W5500_WriteBuff);

	if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
	{
		statusChipInit=1;
		return;
	}
	/* PHY link status check */
	do
	{
		if(ctlwizchip(CW_GET_PHYSTATUS, (void*)&tmp) == -1)
		{
			statusPhysLink=1;
			return;
		}
		statusPhysLink=0;
	} while (tmp == PHY_LINK_OFF);
	HAL_Delay(3000);
	//getVERSIONR();

	wizchip_setnetinfo(&gWIZNETINFO);
	HAL_Delay(1000);


//	processDHCP();

	wizchip_getnetinfo(&checkgWIZNETINFO);

	initializeHttp();

}

void processDHCP(void)
{
	uint8_t dhcp_retry;
	//DHCP Client running
	DHCP_init(SOCK_DHCP, data_buf);
	while(1)
	{
		retDHCPstatus = DHCP_run();
		if(retDHCPstatus == DHCP_IP_LEASED)
		{
			break;
		}
		else if(retDHCPstatus == DHCP_FAILED)
		{
			dhcp_retry++;
			if(dhcp_retry <= 3) {

			}
		}

		if(dhcp_retry > 3)
		{
			DHCP_stop();
			break;
		}
	}
}

void initializeHttp(void)
{
	httpc_init(0, Domain_name, 9004, g_send_buf, g_recv_buf);
}

void ethernetHTTPRoutine(void)
{

	receivehttpcheck();
	httpc_connection_handler();

	if(httpc_isSockOpen)
	{
		connectionCheck = httpc_connect();
	}
	if(httpc_isConnected)
	{
	//	if(!flag_sent_http_request)
		{
// Send: HTTP request
//http://power-meter.acceedo.in:9010/powermeter?mid=1&bid=1&data=5,3,4,158,200,66,72,0,0,00,00,25,39
//uint8_t PowerData[255]="["20,183,72,48,10,100,72,48,28,101,78,34"]"
#if 0
			memcpy(dataURI,0,sizeof(dataURI));
		for(k=0,x=0;k<=3;k++)
		{
			if(PowerMeterdatabase[Meter_Id-1].NoOfData[k-1]!=0){
				for(l=0;l <= (PowerMeterdatabase[Meter_Id-1].NoOfData[k-1]-1);l++){
				dataURI[x] = PowerMeterdatabase[Meter_Id-1].loc_datastore[k][l];
				x++;
			}
			}
			else
			{
				dataURI[x] = '0';
				x++;
			}

		if(k==3){
			dataURI[x]='"';
			x++;
			dataURI[x]=']';
		}
		else{
			dataURI[x]=',';
			//x++;
		}
		}
#endif

		length1 = sprintf(URI,"POST /powermeter?mid=%d&bid=1 HTTP/1.0\r\n"
						"Host: isc2.power-meter.acceedo.in:9004\r\n"
						"Accept: text/html\r\n"
						"Content-Type: application/json \r\n"
						"Content-Length: %d\r\n\r\n"
						"[%c",Meter_Id,265,'"');
		length2 =  sprintf(URI+length1,"%03d,%03d,%03d,%03d,%03d,%03d,%03d%,%03d,%03d,%03d,%03d,%03d,%03d,%03d,%03d,%03d%c",
						  PowerMeterdatabase[Meter_Id-1].loc_datastore[0][0],PowerMeterdatabase[Meter_Id-1].loc_datastore[0][1],PowerMeterdatabase[Meter_Id-1].loc_datastore[0][2],PowerMeterdatabase[Meter_Id-1].loc_datastore[0][3],
						  PowerMeterdatabase[Meter_Id-1].loc_datastore[0][4],PowerMeterdatabase[Meter_Id-1].loc_datastore[0][5],PowerMeterdatabase[Meter_Id-1].loc_datastore[0][6],PowerMeterdatabase[Meter_Id-1].loc_datastore[0][7],
						  PowerMeterdatabase[Meter_Id-1].loc_datastore[0][8],PowerMeterdatabase[Meter_Id-1].loc_datastore[0][9],PowerMeterdatabase[Meter_Id-1].loc_datastore[0][10],PowerMeterdatabase[Meter_Id-1].loc_datastore[0][11],
						  PowerMeterdatabase[Meter_Id-1].loc_datastore[0][12],PowerMeterdatabase[Meter_Id-1].loc_datastore[0][13],PowerMeterdatabase[Meter_Id-1].loc_datastore[0][14],PowerMeterdatabase[Meter_Id-1].loc_datastore[0][15],
						  '"');//48+2+15
		length3 =  sprintf(URI+length2+length1,"%c%c%03d,%03d,%03d,%03d,%03d,%03d,%03d%,%03d,%03d,%03d,%03d,%03d,%03d,%03d,%03d,%03d%c",
								 ',','"',PowerMeterdatabase[Meter_Id-1].loc_datastore[1][0],PowerMeterdatabase[Meter_Id-1].loc_datastore[1][1],PowerMeterdatabase[Meter_Id-1].loc_datastore[1][2],PowerMeterdatabase[Meter_Id-1].loc_datastore[1][3],
								  PowerMeterdatabase[Meter_Id-1].loc_datastore[1][4],PowerMeterdatabase[Meter_Id-1].loc_datastore[1][5],PowerMeterdatabase[Meter_Id-1].loc_datastore[1][6],PowerMeterdatabase[Meter_Id-1].loc_datastore[1][7],
								  PowerMeterdatabase[Meter_Id-1].loc_datastore[1][8],PowerMeterdatabase[Meter_Id-1].loc_datastore[1][9],PowerMeterdatabase[Meter_Id-1].loc_datastore[1][10],PowerMeterdatabase[Meter_Id-1].loc_datastore[1][11],
								  PowerMeterdatabase[Meter_Id-1].loc_datastore[1][12],PowerMeterdatabase[Meter_Id-1].loc_datastore[1][13],PowerMeterdatabase[Meter_Id-1].loc_datastore[1][14],PowerMeterdatabase[Meter_Id-1].loc_datastore[1][15],
								  '"');
		length4 =  sprintf(URI+length3+length2+length1,"%c%c%03d,%03d,%03d,%03d,%03d,%03d,%03d%,%03d,%03d,%03d,%03d,%03d,%03d,%03d,%03d,%03d%c",
								 ',','"',PowerMeterdatabase[Meter_Id-1].loc_datastore[2][0],PowerMeterdatabase[Meter_Id-1].loc_datastore[2][1],PowerMeterdatabase[Meter_Id-1].loc_datastore[2][2],PowerMeterdatabase[Meter_Id-1].loc_datastore[2][3],
								  PowerMeterdatabase[Meter_Id-1].loc_datastore[2][4],PowerMeterdatabase[Meter_Id-1].loc_datastore[2][5],PowerMeterdatabase[Meter_Id-1].loc_datastore[2][6],PowerMeterdatabase[Meter_Id-1].loc_datastore[2][7],
								  PowerMeterdatabase[Meter_Id-1].loc_datastore[2][8],PowerMeterdatabase[Meter_Id-1].loc_datastore[2][9],PowerMeterdatabase[Meter_Id-1].loc_datastore[2][10],PowerMeterdatabase[Meter_Id-1].loc_datastore[2][11],
								  PowerMeterdatabase[Meter_Id-1].loc_datastore[2][12],PowerMeterdatabase[Meter_Id-1].loc_datastore[2][13],PowerMeterdatabase[Meter_Id-1].loc_datastore[2][14],PowerMeterdatabase[Meter_Id-1].loc_datastore[2][15],
								  '"');
		length5 = sprintf(URI+length4+length3+length2+length1,"%c%c%03d,%03d,%03d,%03d,%03d,%03d,%03d%,%03d,%03d,%03d,%03d,%03d,%03d,%03d,%03d,%03d%c%c",
										 ',','"',PowerMeterdatabase[Meter_Id-1].loc_datastore[3][0],PowerMeterdatabase[Meter_Id-1].loc_datastore[3][1],PowerMeterdatabase[Meter_Id-1].loc_datastore[3][2],PowerMeterdatabase[Meter_Id-1].loc_datastore[3][3],
										  PowerMeterdatabase[Meter_Id-1].loc_datastore[3][4],PowerMeterdatabase[Meter_Id-1].loc_datastore[3][5],PowerMeterdatabase[Meter_Id-1].loc_datastore[3][6],PowerMeterdatabase[Meter_Id-1].loc_datastore[3][7],
										  PowerMeterdatabase[Meter_Id-1].loc_datastore[3][8],PowerMeterdatabase[Meter_Id-1].loc_datastore[3][9],PowerMeterdatabase[Meter_Id-1].loc_datastore[3][10],PowerMeterdatabase[Meter_Id-1].loc_datastore[3][11],
										  PowerMeterdatabase[Meter_Id-1].loc_datastore[3][12],PowerMeterdatabase[Meter_Id-1].loc_datastore[3][13],PowerMeterdatabase[Meter_Id-1].loc_datastore[3][14],PowerMeterdatabase[Meter_Id-1].loc_datastore[3][15],
										  '"',']');
#if 0
		sprintf(URI,"POST /powermeter?mid=%d&bid=1 HTTP/1.0\r\n"
				"Host: power-meter.acceedo.in:9010\r\n"
				"Accept: text/html\r\n"
				"Content-Type: application/json \r\n"
				"Content-Length: %d\r\n\r\n"
				"[%c20,183,72,48,10,100,72,48,28,101,78,34%c,"
				"%c30,183,72,48,10,100,72,48,28,101,78,34%c,"
				"%c40,183,72,48,10,100,72,48,28,101,78,34%c,"
				"%c20,183,72,48,10,100,72,48,28,101,78,34%c]",Meter_Id,165,'"','"','"','"','"','"','"','"');
#endif
		//request.method = (uint8_t *)HTTP_POST;
		request.uri = (uint8_t *)URI;
		//request.host = (uint8_t *)Domain_name;
	    request.connection=(uint8_t *)HTTP_CONNECTION_CLOSE;
	   // request.content_type = (uint8_t *)HTTP_CTYPE_TEXT_HTML;

		// HTTP client example #1: Function for send HTTP request (header and body fields are integrated)
			{
			httpc_send(&request, g_recv_buf, g_send_buf, 0);
			}
			flag_sent_http_request = ENABLE;
		}
		// Recv: HTTP response

	}

}

