/*! @file radio.h
 * @brief This file is contains the public radio interface functions.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */
#if defined(SI4463_USE_FIFO_MODE)
#include "radio_config_dr500k.h"
//#include "radio_config.h"
#elif defined(SI4463_USE_DIRECT_MODE)
#include "radio_config_directRX.h"
#endif
#ifndef RADIO_H_
#define RADIO_H_

/*****************************************************************************
 *  Global Macros & Definitions
 *****************************************************************************/
/*! Maximal packet length definition (FIFO size) */
#define RADIO_MAX_PACKET_LENGTH     64u

/*****************************************************************************
 *  Global Typedefs & Enums
 *****************************************************************************/
typedef struct
{
    const U8   *Radio_ConfigurationArray;

    U8   Radio_ChannelNumber;
    U8   Radio_PacketLength;
    U8   Radio_State_After_Power_Up;

    U16  Radio_Delay_Cnt_After_Reset;

    U8   Radio_CustomPayload[RADIO_MAX_PACKET_LENGTH];
} tRadioConfiguration;

/*****************************************************************************
 *  Global Variable Declarations
 *****************************************************************************/
 #ifdef STM_USE_SI4663_RF_TRANCEVER
 extern U8 customRadioPacket[RADIO_MAX_PACKET_LENGTH];
 extern const tRadioConfiguration *pRadioConfiguration;
 extern const U8 Radio_Configuration_Data_Array[970];
 #else
extern const SEGMENT_VARIABLE_SEGMENT_POINTER(pRadioConfiguration, tRadioConfiguration, SEG_CODE, SEG_CODE);
extern SEGMENT_VARIABLE(customRadioPacket[RADIO_MAX_PACKET_LENGTH], U8, SEG_XDATA);

/*! Si446x configuration array */
extern const SEGMENT_VARIABLE(Radio_Configuration_Data_Array[], U8, SEG_CODE);
#endif

/*****************************************************************************
 *  Global Function Declarations
 *****************************************************************************/
U8  vRadio_Init(void);
#if defined(SI4463_USE_FIFO_MODE)
U8    bRadio_Check_Tx_RX(void);
void  vRadio_StartRX(U8 channel, U8 packetLenght );
//U8    bRadio_Check_Ezconfig(U16);
void  vRadio_StartTx_Variable_Packet(U8 channel, U8 *pioRadioPacket, U8 length);
#elif defined(SI4463_USE_DIRECT_MODE)
void  vRadio_StartRX(U8);
void  vRadio_StartTx(U8, U8 *);
void vRadio_Change_GPIO_Cfg(U8);
#endif

#endif /* RADIO_H_ */
