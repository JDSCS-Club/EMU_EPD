/*! @file radio.c
 * @brief This file contains functions to interface with the radio chip.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#include "..\bsp.h"

/*****************************************************************************
 *  Local Macros & Definitions
 *****************************************************************************/

/*****************************************************************************
 *  Global Variables
 *****************************************************************************/
const SEGMENT_VARIABLE(Radio_Configuration_Data_Array[], U8, SEG_CODE) = \
              RADIO_CONFIGURATION_DATA_ARRAY;

const SEGMENT_VARIABLE(RadioConfiguration, tRadioConfiguration, SEG_CODE) = \
                        RADIO_CONFIGURATION_DATA;

const SEGMENT_VARIABLE_SEGMENT_POINTER(pRadioConfiguration, tRadioConfiguration, SEG_CODE, SEG_CODE) = \
                        &RadioConfiguration;


SEGMENT_VARIABLE(customRadioPacket[RADIO_MAX_PACKET_LENGTH], U8, SEG_XDATA);

/*****************************************************************************
 *  Local Function Declarations
 *****************************************************************************/
void vRadio_PowerUp(void);

/*!
 *  Power up the Radio.
 *
 *  @note
 *
 */
void vRadio_PowerUp(void)
{
  SEGMENT_VARIABLE(wDelay,  U16, SEG_XDATA) = 0u;

  /* Hardware reset the chip */
  si446x_reset();

  /* Wait until reset timeout or Reset IT signal */
  for (; wDelay < pRadioConfiguration->Radio_Delay_Cnt_After_Reset; wDelay++);
}

/*!
 *  Radio Initialization.
 *
 *  @author Sz. Papp
 *
 *  @note
 *
 */
void vRadio_Init(void)
{
  U16 wDelay;

  /* Power Up the radio chip */
  vRadio_PowerUp();

  /* Load radio configuration */
  while (SI446X_SUCCESS != si446x_configuration_init(pRadioConfiguration->Radio_ConfigurationArray))
  {
    /* Error hook */
#if !(defined SILABS_PLATFORM_WMB912)
    LED4 = !LED4;
#else
    vCio_ToggleLed(eHmi_Led4_c);
#endif
    for (wDelay = 0x7FFF; wDelay--; ) ;
    /* Power Up the radio chip */
    vRadio_PowerUp();
  }

  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);
}

/*!
 *  Check if Packet sent IT flag or Packet Received IT is pending.
 *
 *  @return   SI4455_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT / SI4455_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT
 *
 *  @note
 *
 */
U8 bRadio_Check_Tx_RX(void)
{
  if (RF_NIRQ == FALSE)
  {
    /* Read ITs, clear pending ones */
    si446x_get_int_status_fast_clear_read();

	  if (Si446xCmd.GET_INT_STATUS.CHIP_PEND & SI446X_CMD_GET_CHIP_STATUS_REP_CHIP_PEND_CMD_ERROR_PEND_BIT)
    {
    	/* State change to */
     	si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEXT_STATE1_NEW_STATE_ENUM_SLEEP);
	
	  	/* Reset FIFO */
     	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_RX_BIT);
      
	  	/* State change to */
        si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEXT_STATE1_NEW_STATE_ENUM_RX);
    }

    if(Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT)
    {
      return SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT;
    }

    if(Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT)
    {
      /* Packet RX */

      /* Get payload length */
			// TX FIFO may need to be written for ACK. Do it now to save time for the TX-RX turnaround
      si446x_fifo_info(0x00 | SI446X_CMD_FIFO_INFO_ARG_FIFO_TX_BIT);

      si446x_read_rx_fifo(Si446xCmd.FIFO_INFO.RX_FIFO_COUNT, &customRadioPacket[0]);

      return SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT;
    }
      
	  if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_STATUS_CRC_ERROR_BIT)
    {
    	/* Reset FIFO */
    	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_RX_BIT);
    }
	  
  }

  return 0;
}

/*!
 *  Set Radio to RX mode. .
 *
 *  @param channel Freq. Channel,  packetLength : 0 Packet handler fields are used , nonzero: only Field1 is used
 *
 *  @note
 *
 */
void vRadio_StartRX(U8 channel, U8 packetLenght )
{
  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);

   // Reset the Rx Fifo
   si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_RX_BIT);

  /* Start Receiving packet, channel 0, START immediately, Packet length used or not according to packetLength */
  si446x_start_rx(channel, 0u, packetLenght,
                  SI446X_CMD_START_RX_ARG_NEXT_STATE1_RXTIMEOUT_STATE_ENUM_NOCHANGE,
                  SI446X_CMD_START_RX_ARG_NEXT_STATE2_RXVALID_STATE_ENUM_READY,
                  SI446X_CMD_START_RX_ARG_NEXT_STATE3_RXINVALID_STATE_ENUM_RX );
}

/*!
 *  Set Radio to TX mode, variable packet length.
 *
 *  @param channel Freq. Channel, Packet to be sent length of of the packet sent to TXFIFO
 *
 *  @note
 *
 */
void vRadio_StartTx_Variable_Packet(U8 channel, U8 *pioRadioPacket, U8 length)
{
  /* Leave RX state */
  si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEXT_STATE1_NEW_STATE_ENUM_READY);

  /* Read ITs, clear pending ones */
  si446x_get_int_status(0u, 0u, 0u);

  /* Reset the Tx Fifo */
  si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_TX_BIT);

  /* Fill the TX fifo with datas */
  si446x_write_tx_fifo(length, pioRadioPacket);

  /* Start sending packet, channel 0, START immediately */
   si446x_start_tx(channel, 0x80, length);

}

/*!
 *  Set Radio to TX mode, variable packet length. 
 *  Use it when multiple fields are in use (START_TX is called with 0 length).
 *
 *  @param channel Freq. Channel, Packet to be sent length of of the packet sent to TXFIFO
 *
 *  @note
 *
 */
void vRadio_StartTx_Variable_Packet_MultiField(U8 channel, U8 *pioRadioPacket, U8 length)
{
  /* Read ITs, clear pending ones */
	si446x_get_int_status_fast_clear();

  /* Reset the Tx Fifo */
  si446x_fifo_info_fast_reset(SI446X_CMD_FIFO_INFO_ARG_FIFO_TX_BIT);

  /* Fill the TX fifo with datas */
  si446x_write_tx_fifo(length, pioRadioPacket);

  /* Start sending packet, channel 0, START immediately */
	 si446x_start_tx(channel, 0x80, 0);
}


/**
 *  Find a property value in the dynamic Radio Configuration Array
 *  (I.e. get property value that is in radio_config.h, but unknown where it is within the array)
 *
 *  @note This code is capable of finding a property's configuration value in a standard configuration byte array.
 *  Byte array pattern is: Length, [Command, GroupId, NumOfProps, StartPropId, Val1, ..., ValN], 0x00   
 *  Used when some property value is needed out of a generated configuration header or hex file.
 *  If more then one write is available the last one will be returned. 
*/
U8 bRadio_FindProperty(U8* pbArray, U8 bGroup, U8 bAddress, U8* pbValue)
{   
  U16 wInd = 0;
  U8 bPropertyValue = 0x00;
  BIT gPropertyFound = FALSE;

  // Input validation            
  if (pbArray == NULL)
  {
    return FALSE;
  }

  // Search until reaching the terminating 0
  while (pbArray[wInd] != 0)
  {
    // Looking for SET_PROPERTY = 0x11 command
    if (pbArray[wInd+1] != 0x11)
    {
      wInd += pbArray[wInd] + 1; 
      continue;
    }

    // It is a SET_PROPERTY command, check if the corresponding row makes sense (i.e. the array is not broken)
    if ( pbArray[wInd] < 0x05 || pbArray[wInd] > 0x10 || pbArray[wInd] != (pbArray[wInd + 3] + 4) ) // Command length in line with API length
    {                                                                      
      return FALSE;
    }

    // Look for property value
    if (pbArray[wInd + 2] == bGroup)
    {
      if ( (pbArray[wInd + 4] <= bAddress ) && ( bAddress < pbArray[wInd + 3] + pbArray[wInd + 4] ) )
      {
        bPropertyValue = pbArray[wInd + 5 + (bAddress - pbArray[wInd + 4])];
        gPropertyFound = TRUE;
        // Don't break the loop here, check the rest of the array
      }
    }
    wInd += pbArray[wInd] + 1;
  }

  if (gPropertyFound)
  {
    *pbValue = bPropertyValue;
  }
  return gPropertyFound;
}