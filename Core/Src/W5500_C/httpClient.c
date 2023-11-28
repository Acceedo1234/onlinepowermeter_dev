/*
 * httpClient.c
 *
 *  Created on: Feb 21, 2020
 *      Author: becky
 */


#include <stdlib.h>
#include <string.h>
#include <struct.h>

#include "wizchip_conf.h"
#include "socket.h"
#include "httpClient.h"

struct ModbusRegValue MeterInfo[20];

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint8_t * httpc_send_buf;
uint8_t * httpc_recv_buf;
//HttpRequest request = HttpRequest_multipart_post_initializer;
HttpRequest request = HttpRequest_get_initializer;

static int8_t httpsock = 0;
static uint8_t dest_ip[4] = {192,168,201,51};
static uint16_t dest_port =9010;
static uint16_t httpc_any_port = 0;

uint8_t httpc_isSockOpen = HTTPC_FALSE;
uint8_t httpc_isConnected = HTTPC_FALSE;
uint16_t httpc_isReceived = HTTPC_FALSE;

uint16_t valhttp;
uint8_t rxBufhttp[2048];
char *startPosition;
uint8_t *endPosition;
char filtereddata[200];
uint8_t *pch;
uint8_t Meter_Id_Rx;
uint8_t token_1[20];
uint8_t taildata[10];
uint8_t Total_No_Of_Meter;
uint8_t UpdateNewEntry;
uint8_t LockUpdate;


uint32_t heatnumber;
uint16_t rxType,furnace;
uint16_t rxReqCarbon,rxReqSilica,rxReqMn,rxReqCu,rxReqSn,rxReqZn;
extern uint8_t Meter_Id;
/* Private functions prototypes ----------------------------------------------*/
uint16_t get_httpc_any_port(void);
/*
255.255.255.0
192.168.0.200
192.168.0.206
*/
/* Public & Private functions ------------------------------------------------*/

uint8_t httpc_init(uint8_t sock, uint8_t * ip, uint16_t port, uint8_t * sbuf, uint8_t * rbuf)
{
	uint8_t ret = HTTPC_FALSE;

	if(sock <= _WIZCHIP_SOCK_NUM_)
	{
		// Hardware socket number for HTTP client (0 ~ 7)
		httpsock = sock;

		// Shared buffers: HTTP Send / Recevice
		httpc_send_buf = sbuf;
		httpc_recv_buf = rbuf;

		// Destination IP address and Port number
		// (Destination = HTTP server)
		dest_ip[0] = ip[0];
		dest_ip[1] = ip[1];
		dest_ip[2] = ip[2];
		dest_ip[3] = ip[3];
		dest_port = port;

		ret = HTTPC_TRUE;
	}

	return ret;
}


// return: true / false
uint8_t httpc_connection_handler()
{
	uint8_t ret = HTTPC_FALSE;

	uint16_t source_port;

#ifdef _HTTPCLIENT_DEBUG_
	uint8_t destip[4] = {0, };
	uint16_t destport = 80;
#endif

	uint8_t state = getSn_SR(httpsock);
	switch(state)
	{
		case SOCK_INIT:
			// state: connect
			ret = HTTPC_TRUE;
			break;

		case SOCK_ESTABLISHED:
			if(getSn_IR(httpsock) & Sn_IR_CON)
			{
#ifdef _HTTPCLIENT_DEBUG_
				// Serial debug message printout
				getsockopt(httpsock, SO_DESTIP, &destip);
				getsockopt(httpsock, SO_DESTPORT, &destport);
				printf(" > HTTP CLIENT: CONNECTED TO - %d.%d.%d.%d : %d\r\n",destip[0], destip[1], destip[2], destip[3], destport);
#endif
				httpc_isConnected = HTTPC_TRUE;
				//receivehttpcheck();
				setSn_IR(httpsock, Sn_IR_CON);
			}

			//httpc_isReceived = getSn_RX_RSR(httpsock);
			ret = HTTPC_CONNECTED;
			break;

		case SOCK_CLOSE_WAIT:
			disconnect(httpsock);
			break;

		case SOCK_FIN_WAIT:
		case SOCK_CLOSED:
			httpc_isSockOpen = HTTPC_FALSE;
			httpc_isConnected = HTTPC_FALSE;

			source_port = get_httpc_any_port();
#ifdef _HTTPCLIENT_DEBUG_
			printf(" > HTTP CLIENT: source_port = %d\r\n", source_port);
#endif

			if(socket(httpsock, Sn_MR_TCP, source_port, Sn_MR_ND) == httpsock)
			{
				if(httpc_isSockOpen == HTTPC_FALSE)
				{
#ifdef _HTTPCLIENT_DEBUG_
					printf(" > HTTP CLIENT: SOCKOPEN\r\n");
#endif
					httpc_isSockOpen = HTTPC_TRUE;
				}
			}


			break;

		default:
			break;
	}

	return ret;
}


// return: socket status
uint8_t httpc_connect()
{
	uint8_t ret = HTTPC_FALSE;

	if(httpsock >= 0)
	{
		if(httpc_isSockOpen == HTTPC_TRUE)
		{
			// TCP connect
			ret = connect(httpsock, dest_ip, dest_port);
			if(ret == SOCK_OK) ret = HTTPC_TRUE;
		}
	}

	return ret;
}

uint16_t httpc_add_customHeader_field(uint8_t * customHeader_buf, const char * name, const char * value)
{
	uint16_t len = 0;

	if((strlen((char *)name) + strlen((char *)value)) > DATA_BUF_SIZE) return 0;

	if(strlen((char *)customHeader_buf) == 0)
	{
		len = sprintf((char *)customHeader_buf, "%s: %s\r\n", name, value);
	}
	else
	{
		len = sprintf((char *)customHeader_buf, "%s%s: %s\r\n", customHeader_buf, name, value);
	}

	return len;
}

// return: sent header length
uint16_t httpc_send_header(HttpRequest * req, uint8_t * buf, uint8_t * customHeader_buf, uint16_t content_len)
{
	uint16_t len;

	if(httpc_isConnected == HTTPC_TRUE)
	{
		memset(buf, 0x00, DATA_BUF_SIZE);

		len = sprintf((char *)buf, "%s %s HTTP/1.1\r\n", req->method, req->uri);
		len += sprintf((char *)buf+len, "Host: %s\r\n", req->host);
		len += sprintf((char *)buf+len, "Connection: %s\r\n", req->connection);

		// HTTP content type: POST / PUT
		if(content_len > 0)
		{
			len += sprintf((char *)buf+len, "Content-Length: %d\r\n", content_len);

			if(strcmp((char *)req->content_type, HTTP_CTYPE_MULTIPART_FORM) == 0)
			{
				// HTTP content type: multipart/form-data
				len += sprintf((char *)buf+len, "Content-Type: %s; boundary=%s\r\n", req->content_type, formDataBoundary);
			}
			else
			{
				// HTTP content type: others
				len += sprintf((char *)buf+len, "Content-Type: %s\r\n", req->content_type); // HTTP content type: others
			}
		}

		// for adding custom header fields
		if(customHeader_buf != NULL)
		{
			if((strlen((char *)customHeader_buf) + len + 2) <= DATA_BUF_SIZE)
			{
				len += sprintf((char *)buf+len, "%s", customHeader_buf);
			}
		}

		len += sprintf((char *)buf+len, "\r\n");

#ifdef _HTTPCLIENT_DEBUG_
		printf(" >> HTTP Request header - Method: %s, URI: %s, Content-Length: %d\r\n", req->method, req->uri, content_len);
		printf("%s", buf);
#endif
		send(httpsock, buf, len);
	}
	else
	{
		len = HTTPC_FAILED;
	}

	return len;
}


// return: sent body length
uint16_t httpc_send_body(uint8_t * buf, uint16_t len)
{
	uint16_t sentlen = 0;

#ifdef _HTTPCLIENT_DEBUG_
	uint16_t i;
#endif

	if(httpc_isConnected == HTTPC_TRUE)
	{
		do{
			sentlen += send(httpsock, buf, len);
		} while(sentlen < len);

#ifdef _HTTPCLIENT_DEBUG_
		if(sentlen > 0)
		{
			printf(" >> HTTP Request Body\r\n");
			for(i = 0; i < sentlen; i++) printf("%c", buf[i]);
			printf("\r\n");
		}
#endif
	}
	else
	{
		sentlen = HTTPC_FAILED;
	}

	return sentlen;
}


// return: sent data length
uint16_t httpc_send(HttpRequest * req, uint8_t * buf, uint8_t * body, uint16_t content_len)
{
	uint16_t i;
	uint16_t len;
	uint8_t http_header_generated = HTTPC_FAILED;

	if(httpc_isConnected == HTTPC_TRUE)
	{
		do
		{
			memset(buf, 0x00, DATA_BUF_SIZE);

			/* HTTP request header */
			len = sprintf((char *)buf, "%s\r\n", req->uri);
//			len += sprintf((char *)buf+len, "Host: %s\r\n", req->host);
			len += sprintf((char *)buf+len, "Connection: %s\r\n", req->connection);

			// HTTP content type: POST / PUT only
			if(content_len > 0)
			{
				len += sprintf((char *)buf+len, "Content-Length: %d\r\n", content_len);

				if(strcmp((char *)req->content_type, HTTP_CTYPE_MULTIPART_FORM) == 0)
				{
					// HTTP content type: multipart/form-data
					len += sprintf((char *)buf+len, "Content-Type: %s; boundary=%s\r\n", req->content_type, formDataBoundary);
				}
				else
				{
					// HTTP content type: others
					len += sprintf((char *)buf+len, "Content-Type: %s\r\n", req->content_type); // HTTP content type: others
				}
			}
			len += sprintf((char *)buf+len, "\r\n");

			// Avoiding buffer overflow
			if((len + content_len) > DATA_BUF_SIZE) {
				content_len = DATA_BUF_SIZE - len; // HTTP request header re-generation (do-while)
				//http_header_generated = FAILED;
			} else {
				http_header_generated = HTTPC_SUCCESS;
			}
		} while(http_header_generated != HTTPC_SUCCESS);

		/* HTTP request body */
		for(i = 0; i < content_len; i++)
		{
			buf[len++] = body[i];
		}

#ifdef _HTTPCLIENT_DEBUG_
		printf(" >> HTTP Request - Method: %s, URI: %s, Content-Length: %d\r\n", req->method, req->uri, content_len);
		for(i = 0; i < len; i++) printf("%c", buf[i]);
		printf("\r\n");
#endif
		send(httpsock, buf, len);
	}
	else
	{
		len = HTTPC_FAILED;
	}

	return len;
}


// return: received data length
uint16_t httpc_recv(uint8_t * buf, uint16_t len)
{
	uint16_t recvlen;

	if(httpc_isConnected == HTTPC_TRUE)
	{
		if(len > DATA_BUF_SIZE) len = DATA_BUF_SIZE;
		recvlen = recv(httpsock, buf, len);
	}
	else
	{
		recvlen = HTTPC_FAILED;
	}

	return recvlen;
}


// return: true / false
uint8_t httpc_disconnect(void)
{
	uint8_t ret = HTTPC_FALSE;

	if(httpc_isConnected == HTTPC_TRUE)
	{
#ifdef _HTTPCLIENT_DEBUG_
		printf(" > HTTP CLIENT: Try to disconnect\r\n");
#endif
		ret = disconnect(httpsock);
		if(ret == SOCK_OK)
		{
			ret = HTTPC_TRUE;
#ifdef _HTTPCLIENT_DEBUG_
			printf(" > HTTP CLIENT: Disconnected\r\n");
#endif
		}
	}

	return ret;
}


// return: source port number for tcp client
uint16_t get_httpc_any_port(void)
{
	if(httpc_any_port)
	{
		if((httpc_any_port >= HTTP_CLIENT_PORT_MIN) && (httpc_any_port < HTTP_CLIENT_PORT_MAX)) {
			httpc_any_port++;
		} else {
			httpc_any_port = 0;
		}
	}

	if(httpc_any_port < HTTP_CLIENT_PORT_MIN)
	{
		// todo: get random seed value
		httpc_any_port = (rand() % 10000) + 46000; // 46000 ~ 55999
	}

	return httpc_any_port;
}

void receivehttpcheck(void)
{
	uint16_t Frame_1_StartAdd,Frame_1_NoOfData;
	uint16_t Frame_2_StartAdd,Frame_2_NoOfData;
	uint16_t Frame_3_StartAdd,Frame_3_NoOfData;
	uint16_t Frame_4_StartAdd,Frame_4_NoOfData;
	uint16_t Frame_5_StartAdd,Frame_5_NoOfData;
	//uint8_t filtereddata[200];
	valhttp = getSn_RX_RSR(httpsock);
//	filtereddata
	if(valhttp>0){
		//httpc_recv(rxBufhttp,valhttp);
		memset(rxBufhttp,0,sizeof(rxBufhttp));
		recv(httpsock, rxBufhttp, valhttp);
		startPosition = strchr(rxBufhttp,'$');
		strcpy(filtereddata,startPosition);
		pch = strtok (filtereddata,",");
		if(pch!= NULL){
			strcpy(token_1,pch);
			if(token_1[0] == '$'){
				Meter_Id_Rx = token_1[2]-48 +((token_1[1]-48)*10);
				UpdateNewEntry =0;
				LockUpdate=0;
			}
			else{
				//exit;
			}
		}
		pch = strtok (0,",");
		while(pch!= NULL){
			strcpy(token_1,pch);
			if((token_1[0] == 'F')&&(token_1[1] == '1')){
/*Caluclation consume runtime*/Frame_1_StartAdd = token_1[7]-48 +((token_1[6]-48)*10)+((token_1[5]-48)*100)+((token_1[4]-48)*1000)+((token_1[3]-48)*10000);
				Frame_1_NoOfData = token_1[13]-48 +((token_1[12]-48)*10)+((token_1[11]-48)*100)+((token_1[10]-48)*1000)+((token_1[9]-48)*10000);
				MeterInfo[Meter_Id_Rx-1].ModbusFrameRegInfo[0].FrameStartAddress = Frame_1_StartAdd;
				MeterInfo[Meter_Id_Rx-1].ModbusFrameRegInfo[0].FrameNoOfData = Frame_1_NoOfData;
				UpdateNewEntry=UpdateNewEntry+1;
				if(UpdateNewEntry > 1){LockUpdate=1;}
				(LockUpdate == 0)? (MeterInfo[Meter_Id_Rx-1].NoOfFrame = 1):(LockUpdate=1);
			}
			else if((token_1[0] == 'F')&&(token_1[1] == '2')){
				Frame_2_StartAdd = token_1[7]-48 +((token_1[6]-48)*10)+((token_1[5]-48)*100)+((token_1[4]-48)*1000)+((token_1[3]-48)*10000);
				Frame_2_NoOfData = token_1[13]-48 +((token_1[12]-48)*10)+((token_1[11]-48)*100)+((token_1[10]-48)*1000)+((token_1[9]-48)*10000);
				MeterInfo[Meter_Id_Rx-1].ModbusFrameRegInfo[1].FrameStartAddress = Frame_2_StartAdd;
				MeterInfo[Meter_Id_Rx-1].ModbusFrameRegInfo[1].FrameNoOfData = Frame_2_NoOfData;
				UpdateNewEntry=UpdateNewEntry+1;
				(LockUpdate == 0)? (MeterInfo[Meter_Id_Rx-1].NoOfFrame = 2):(LockUpdate=1);

			}
			else if((token_1[0] == 'F')&&(token_1[1] == '3')){
				Frame_3_StartAdd = token_1[7]-48 +((token_1[6]-48)*10)+((token_1[5]-48)*100)+((token_1[4]-48)*1000)+((token_1[3]-48)*10000);
				Frame_3_NoOfData = token_1[13]-48 +((token_1[12]-48)*10)+((token_1[11]-48)*100)+((token_1[10]-48)*1000)+((token_1[9]-48)*10000);
				MeterInfo[Meter_Id_Rx-1].ModbusFrameRegInfo[2].FrameStartAddress = Frame_3_StartAdd;
				MeterInfo[Meter_Id_Rx-1].ModbusFrameRegInfo[2].FrameNoOfData = Frame_3_NoOfData;
				UpdateNewEntry=UpdateNewEntry+1;
				(LockUpdate == 0)? (MeterInfo[Meter_Id_Rx-1].NoOfFrame = 3):(LockUpdate=1);
			}
			else if((token_1[0] == 'F')&&(token_1[1] == '4')){
				Frame_4_StartAdd = token_1[7]-48 +((token_1[6]-48)*10)+((token_1[5]-48)*100)+((token_1[4]-48)*1000)+((token_1[3]-48)*10000);
				Frame_4_NoOfData = token_1[13]-48 +((token_1[12]-48)*10)+((token_1[11]-48)*100)+((token_1[10]-48)*1000)+((token_1[9]-48)*10000);
				MeterInfo[Meter_Id_Rx-1].ModbusFrameRegInfo[3].FrameStartAddress = Frame_4_StartAdd;
				MeterInfo[Meter_Id_Rx-1].ModbusFrameRegInfo[3].FrameNoOfData = Frame_4_NoOfData;
				(LockUpdate == 0)? (MeterInfo[Meter_Id_Rx-1].NoOfFrame = 4):(LockUpdate=1);
				UpdateNewEntry=UpdateNewEntry+1;
			}
			else if((token_1[0] == 'F')&&(token_1[1] == '5')){
				Frame_5_StartAdd = token_1[7]-48 +((token_1[6]-48)*10)+((token_1[5]-48)*100)+((token_1[4]-48)*1000)+((token_1[3]-48)*10000);
				Frame_5_NoOfData = token_1[13]-48 +((token_1[12]-48)*10)+((token_1[11]-48)*100)+((token_1[10]-48)*1000)+((token_1[9]-48)*10000);
				MeterInfo[Meter_Id_Rx-1].ModbusFrameRegInfo[4].FrameStartAddress = Frame_5_StartAdd;
				MeterInfo[Meter_Id_Rx-1].ModbusFrameRegInfo[4].FrameNoOfData = Frame_5_NoOfData;
				(LockUpdate == 0)? (MeterInfo[Meter_Id_Rx-1].NoOfFrame = 5):(LockUpdate=1);
				UpdateNewEntry=UpdateNewEntry+1;
			}
			else{
				/*Wrong data*/
			}
			pch = strtok (0,",");
		}

		Total_No_Of_Meter = 16;/*modify manual*/
		if(Meter_Id_Rx == Total_No_Of_Meter)
		{
			Meter_Id =1;
		}
		else
		{
			Meter_Id =Meter_Id_Rx+1;
		}

	//   	}
	}
}
