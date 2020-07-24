/*! @file radio.c
 * @brief This file contains functions to interface with the radio chip.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

//#include "..\bsp.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#if defined(SI4463_USE_FIFO_MODE)
#include "radio_config_dr500k.h"
//#include "radio_config.h"
#elif defined(SI4463_USE_DIRECT_MODE)
#include "radio_config_directRX.h"
#else
#include "radio_config.h"
#endif
#include "radio.h"
#include "si446x_api_lib.h"
#include "si446x_cmd.h"
#include "radio_hal.h"
#include <stdio.h>

//#include "user_ctrl.h"

//extern rf_rx_audio_pkg_cb rx_audio_pkg_callback;



/*****************************************************************************
 *  Local Macros & Definitions
 *****************************************************************************/

/*****************************************************************************
 *  Global Variables
 *****************************************************************************/
#ifdef STM_USE_SI4663_RF_TRANCEVER
const U8 Radio_Configuration_Data_Array[] = RADIO_CONFIGURATION_DATA_ARRAY;
const tRadioConfiguration RadioConfiguration = RADIO_CONFIGURATION_DATA;
const tRadioConfiguration *pRadioConfiguration = &RadioConfiguration;

U8 customRadioPacket[RADIO_MAX_PACKET_LENGTH];
U8 radio_init_status = SI446X_COMMAND_ERROR;
#else
const SEGMENT_VARIABLE(Radio_Configuration_Data_Array[], U8, SEG_CODE) = \
              RADIO_CONFIGURATION_DATA_ARRAY;

const SEGMENT_VARIABLE(RadioConfiguration, tRadioConfiguration, SEG_CODE) = \
                        RADIO_CONFIGURATION_DATA;

const SEGMENT_VARIABLE_SEGMENT_POINTER(pRadioConfiguration, tRadioConfiguration, SEG_CODE, SEG_CODE) = \
                        &RadioConfiguration;

SEGMENT_VARIABLE(customRadioPacket[RADIO_MAX_PACKET_LENGTH], U8, SEG_XDATA);
#endif
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
  //SEGMENT_VARIABLE(wDelay,  U16, SEG_XDATA) = 0u;

  /* Hardware reset the chip */
  si446x_reset();

  /* Wait until reset timeout or Reset IT signal */
  //HAL_Delay(100);
  si446x_get_int_status(0u, 0u, 0u);
 // for (; wDelay < pRadioConfiguration->Radio_Delay_Cnt_After_Reset; wDelay++);
}

/*!
 *  Radio Initialization.
 *
 *  @author Sz. Papp
 *
 *  @note
 *
 */
U8 vRadio_Init(void)
{
    //U16 wDelay;
    //U8 ST_Rtn;
  U8 ret=10;
  U8 radio_init_status;

  /* Power Up the radio chip */
  vRadio_PowerUp();
  /* Load radio configuration */
  while (ret != 0)
  {
    radio_init_status = si446x_configuration_init(pRadioConfiguration->Radio_ConfigurationArray);
    if (radio_init_status == SI446X_SUCCESS)
	{
      break;
    }
    /* Error hook */
    //HAL_GPIO_TogglePin(GREEN_LED_GPIO_Port,GREEN_LED_Pin);
    //HAL_Delay(100);

    /* Power Up the radio chip */
    vRadio_PowerUp();
    ret--;
  }
  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);

  return radio_init_status;
}

/*!
 *  Check if Packet received IT flag is pending.
 *
 *  @return   TRUE - Packet successfully received / FALSE - No packet pending.
 *
 *  @note
 *
 */
#if defined(SI4463_USE_FIFO_MODE)
U8 bRadio_Check_Tx_RX(void)
{
  if (radio_hal_NirqLevel() == GPIO_PIN_RESET)
  {
      /* Read ITs, clear pending ones */
    si446x_get_int_status(0u, 0u, 0u);
    if(radio_init_status == SI446X_SUCCESS)
    {
      //printf("\nDetect interrupt !! %0x\n",Si446xCmd.GET_INT_STATUS.PH_PEND);
    }
    
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
      si446x_fifo_info(0x00);
      
      si446x_read_rx_fifo(Si446xCmd.FIFO_INFO.RX_FIFO_COUNT, &customRadioPacket[0]);
      
      rx_audio_pkg_callback(&customRadioPacket[0],Si446xCmd.FIFO_INFO.RX_FIFO_COUNT);
      
      return SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT;
    }
      
    if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_STATUS_CRC_ERROR_BIT)
    {
      /* Reset FIFO */
      si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_RX_BIT);
    }
  }
  else
  {
    //printf("IRQ HIGH... \n");
  }
  return FALSE;
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
#elif defined(SI4463_USE_DIRECT_MODE)

/* !
 *  Set Radio to RX mode, fixed packet length.
 *
 *  @param channel Freq. Channel
 *
 *  @note
 *
 */
void vRadio_StartRX(U8 channel)
{
  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);

  /* Start Receiving packet, channel 0, START immediately, Packet off  */
  si446x_start_rx(channel, 0u, 0u,
                  SI446X_CMD_START_RX_ARG_NEXT_STATE1_RXTIMEOUT_STATE_ENUM_NOCHANGE,
                  SI446X_CMD_START_RX_ARG_NEXT_STATE2_RXVALID_STATE_ENUM_RX,
                  SI446X_CMD_START_RX_ARG_NEXT_STATE3_RXINVALID_STATE_ENUM_RX );
}

/*!
 *  Set Radio to TX mode, fixed packet length.
 *
 *  @param channel Freq. Channel, Packet to be sent
 *
 *  @note
 *
 */

void vRadio_StartTx(U8 channel, U8 *pioFixRadioPacket)
{
  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);

  /* Start sending packet on channel, START immediately, Packet according to PH */
  si446x_start_tx(channel, 0u, 0u);
}

void vRadio_Change_GPIO_Cfg(U8 mode)
{
  U8 rx_tx;
  //0x00, 0x10, 0x14, 0x13, 0x27, 0x0B, 0x00 /* directTX GPIO1:TX_CLK, GPIO2:RX_DATA, GPIO3:TX_DATA,NIRQ,SDO :Pullup*/
  //0x00, 0x11, 0x14, 0x13, 0x27, 0x0B, 0x00 /* directRX GPIO1:RX_CLK, GPIO2:RX_DATA, GPIO3:TX_DATA,NIRQ,SDO :Pullup*/
  rx_tx = (0x10 | mode);
  
  si446x_gpio_pin_cfg(0x00, rx_tx, 0x14, 0x13, 0x27, 0x0B, 0x00);
}

#endif
