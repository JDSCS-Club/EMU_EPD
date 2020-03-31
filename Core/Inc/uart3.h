/*
 * uart3.h
 *
 *  Created on: 2019. 12. 19.
 *      Author: RobertKim
 */

#ifndef INC_UART3_H_
#define INC_UART3_H_

#include "main.h"
#include "stdbool.h"

//--------------------------------------------------------------------------------------------//
//  UART3 packet value
//--------------------------------------------------------------------------------------------//
#define UART3_VALUE_HEADER1                0xA5
#define UART3_VALUE_HEADER2                0xAA

//--------------------------------------------------------------------------------------------//
//  UART3 packet length
//--------------------------------------------------------------------------------------------//
#define UART3_LEN_HEADER                        2
#define UART3_LEN_PACKET_LEN                	2

//--------------------------------------------------------------------------------------------//
//  UART3 packet position
//--------------------------------------------------------------------------------------------//
#define UART3_POS_HEADER                        0
#define UART3_POS_PACKET_LEN                    2
#define UART3_POS_DATA                    		4

//--------------------------------------------------------------------------------------------//
//  UART3 variable
//--------------------------------------------------------------------------------------------//
#define MAX_UART3_CMD_LENGTH    1000

typedef struct _UartCommadType{
	uint8_t Buffer[MAX_UART3_CMD_LENGTH];
	uint16_t Index;
}UartCommadType;

extern uint16_t uart3TotalLength;
extern bool g_bUart3Detected;
extern bool g_bUart3FindPacket;
extern uint16_t g_Uart3PosHeader;
extern uint16_t g_Uart3CheckTick;
extern UartCommadType g_Uart3Command;
extern uint8_t g_Uart3Packet[MAX_UART3_CMD_LENGTH];
extern uint8_t recvBuffer;

extern void UART3_RecvHandler(int c);
extern void UART3_DecodeCommand(void);
extern void Uart3_Process(void);
extern void Uart3_Send_Packet(uint8_t *packet, uint16_t length);
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//

#endif /* INC_UART3_H_ */
