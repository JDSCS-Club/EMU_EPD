/*
 * uart3.c
 *
 *  Created on: 2019. 12. 19.
 *      Author: RobertKim
 */
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
#include <uart3.h>
#include "main.h"
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
uint16_t uart3TotalLength;
bool g_bUart3Detected;
bool g_bUart3FindPacket;
uint16_t g_Uart3PosHeader;
uint16_t g_Uart3CheckTick;
UartCommadType g_Uart3Command;
uint8_t g_Uart3Packet[MAX_UART3_CMD_LENGTH];
uint8_t recvBuffer = 0;
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  receive Uart3 packet
  */
void UART3_RecvHandler(int c)
{
	g_Uart3CheckTick = 50;// time-out control ms

	if (g_Uart3Command.Index >= MAX_UART3_CMD_LENGTH)
	{
		g_Uart3Command.Index = 0;
	}

	g_Uart3Command.Buffer[g_Uart3Command.Index] = c;

	g_Uart3Command.Index++;

	uint8_t lenDefaultHeader = UART3_LEN_HEADER + UART3_LEN_PACKET_LEN;
	if(g_Uart3Command.Index >= lenDefaultHeader)
	{
		if(g_bUart3FindPacket == false)
		{
			g_Uart3PosHeader =  g_Uart3Command.Index - lenDefaultHeader;
			if(g_Uart3Command.Buffer[g_Uart3PosHeader] == UART3_VALUE_HEADER1
				&& g_Uart3Command.Buffer[g_Uart3PosHeader + 1] == UART3_VALUE_HEADER2)
			{
				g_bUart3FindPacket = true;
			}
		}
		else
		{
			uint16_t lenTotalDataOnly = (uint16_t)(g_Uart3Command.Buffer[g_Uart3PosHeader + UART3_POS_PACKET_LEN] << 8);
			lenTotalDataOnly |= (uint8_t)(g_Uart3Command.Buffer[g_Uart3PosHeader + UART3_POS_PACKET_LEN + 1]);
			if((g_Uart3Command.Index - g_Uart3PosHeader) == UART3_LEN_HEADER + UART3_LEN_PACKET_LEN + lenTotalDataOnly)    // header + data field
			{
				uart3TotalLength = g_Uart3Command.Index - g_Uart3PosHeader;
				uint16_t idx = 0;
				for(idx = 0; idx < uart3TotalLength; idx++)
					g_Uart3Packet[idx] = g_Uart3Command.Buffer[g_Uart3PosHeader + idx];

				g_bUart3Detected = true; // command  buffer recieve ok
				g_Uart3Command.Index = 0; // reset index of command buffer
				g_Uart3CheckTick = 0;
				g_Uart3PosHeader = 0;
				g_bUart3FindPacket = false;
			}
		}
	}
}
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  decode Uart3 packet
  */
void UART3_DecodeCommand(void)
{
	if(g_bUart3Detected)
	{
		g_bUart3Detected = false;

		Uart3_Send_Packet((uint8_t*)g_Uart3Packet, uart3TotalLength);
	}
}

//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  Uart3 process.
  */
void Uart3_Process(void)
{
	if(HAL_UART_Receive(&huart3, &recvBuffer, 1, 1000) == HAL_OK)
		UART3_RecvHandler(recvBuffer);
	UART3_DecodeCommand();
}
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief send uart3 packet
  */
void Uart3_Send_Packet(uint8_t *packet, uint16_t length)
{
	HAL_UART_Transmit(&huart3, packet, length, 1000);
}
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
