/*------------------------------------------------------------------------------------------
	Project			: It Library
	Description		: 

	Writer			: $Author: zlkit $
	Revision		: $Rev: 1891 $
	Date			: 2019. 10.
	Copyright		: Piranti Corp.	( sw@piranti.co.kr )
	 
	Revision History 
	1. 2019. 10.	: Created
-------------------------------------------------------------------------------------------*/

//========================================================================
// Header
#include "rfm.h"

#include "RFMProtocol.h"            //  DevNone

//#include "Adafruit_GFX.h"         //  I2C LCD
#include "Adafruit_SSD1306.h"       //  I2C LCD

#include "version.h"			    //	Version Info

#include "main.h"

#include "audio.h"                  //  WriteI2CCodec()
//#include "rf_pa.h"                  //  WriteI2CCodec()

//========================================================================
// Define

//========================================================================
int g_nDevID = DevNone;            //  Device ID ( 1 : RF900M ���ű� / 2 : RF900T �۽ű� )
//========================================================================

//========================================================================
// Function

//========================================================================
int GetDevID    ( void )
//========================================================================
{
	return g_nDevID;
}


//========================================================================
void SetDevID    ( int nDevID )
//========================================================================
{
	g_nDevID = nDevID;
}


int	 g_nRFMMode = RFMModeNormal;	  //  eRFMMode

//========================================================================
int		GetRFMMode	( void )
//========================================================================
{
	return g_nRFMMode;
}

//========================================================================
void	SetRFMMode	( int nRFMMode )
//========================================================================
{
	g_nRFMMode = nRFMMode;
}



//========================================================================

#include "i2c.h"    //  EEPROM

//========================================================================
int		GetTrainSetIdx	( void )
//========================================================================
{
    uint8_t     idxTrainSet = 0;

    if ( HAL_OK != HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x50 << 1 ), 2, 2 ) )
    {
        printf( "%s(%d) - EEPROM Error\n", __func__, __LINE__ );

        return -1;
    }

    at24_HAL_ReadBytes( &hi2c1, 0xA0, 0x10, (uint8_t *)&idxTrainSet, 1 );
    printf( "%s(%d) - %d\n", __func__, __LINE__, idxTrainSet );

    return idxTrainSet;
}

//========================================================================
void	SetTrainSetIdx	( int idxTrainSet )
//========================================================================
{
    if ( HAL_OK != HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x50 << 1 ), 2, 2 ) )
    {
        printf( "%s(%d) - EEPROM Error\n", __func__, __LINE__ );

        return ;
    }

    printf( "%s(%d) - %d\n", __func__, __LINE__, idxTrainSet );
    at24_HAL_WriteBytes( &hi2c1, 0xA0, 0x10, (uint8_t *)&idxTrainSet, 1 );
}


//========================================================================
int		GetCarNo		( void )
//========================================================================
{
    uint8_t     nCarNo = 0;

    if ( HAL_OK != HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x50 << 1 ), 2, 2 ) )
    {
        printf( "%s(%d) - EEPROM Error\n", __func__, __LINE__ );

        return -1;
    }

    at24_HAL_ReadBytes( &hi2c1, 0xA0, 0x0E, (uint8_t *)&nCarNo, 1 );
    printf( "%s(%d) - %d\n", __func__, __LINE__, nCarNo );

    return nCarNo;
}

//========================================================================
void	SetCarNo		( int nCarNo )
//========================================================================
{
    if ( HAL_OK != HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x50 << 1 ), 2, 2 ) )
    {
        printf( "%s(%d) - EEPROM Error\n", __func__, __LINE__ );

        return ;
    }

    printf( "%s(%d) - %d\n", __func__, __LINE__, nCarNo );
    at24_HAL_WriteBytes( &hi2c1, 0xA0, 0x0E, (uint8_t *)&nCarNo, 1 );
}

//========================================================================
int		GetSpkVol	    ( void )
//========================================================================
{
    uint8_t     nSpkVol = 0;

    if ( HAL_OK != HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x50 << 1 ), 2, 2 ) )
    {
        printf( "%s(%d) - EEPROM Error\n", __func__, __LINE__ );

        return -1;
    }

    at24_HAL_ReadBytes( &hi2c1, 0xA0, 0x0F, (uint8_t *)&nSpkVol, 1 );
    printf( "%s(%d) - %d\n", __func__, __LINE__, nSpkVol );

    return nSpkVol;
}

//========================================================================
void	SetSpkVol	    ( int nSpkVol )
//========================================================================
{
    if ( HAL_OK != HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x50 << 1 ), 2, 2 ) )
    {
        printf( "%s(%d) - EEPROM Error\n", __func__, __LINE__ );

        return ;
    }

    printf( "%s(%d) - %d\n", __func__, __LINE__, nSpkVol );
    at24_HAL_WriteBytes( &hi2c1, 0xA0, 0x0F, (uint8_t *)&nSpkVol, 1 );

    //========================================================================
    //	Codec MAX9860ETG+
    if ( HAL_OK == HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x10 << 1 ), 2, 2 ) )
    {
    	AudioSpkVol( nSpkVol );
    }

}


//========================================================================


//========================================================================
void RF_Ping( void )
//========================================================================
{
    //*

    RF_Tx_Mode();

    SendStat();     //  Send Status Data

    RF_Rx_Mode();

    /*/
    //  Send RF Ping Packet.
    for ( int i = 0; i < pRadioConfiguration->Radio_PacketLength; i++ )
        customRadioPacket[i] = i;

    vRadio_StartTx_Variable_Packet ( pRadioConfiguration->Radio_ChannelNumber,
        &customRadioPacket[0],
        3 );// pRadioConfiguration->Radio_PacketLength );
    //  */
}

//========================================================================
void RF_RSSI( void )
//========================================================================
{
    //  Send RF Ping Packet.
    RF_Ping();
//
//    return;
//
//    //  Get Modem Status.
//    si446x_get_modem_status_fast_clear_read();
//
//    if ( GetDbgLevel() > 0 )
//    {
//        printf( "%s(%d) - ant1:%d / ant2:%d / curr:%d / latch:%d\n", __func__, __LINE__,
//                Si446xCmd.GET_MODEM_STATUS.ANT1_RSSI,
//                Si446xCmd.GET_MODEM_STATUS.ANT2_RSSI,
//                Si446xCmd.GET_MODEM_STATUS.CURR_RSSI,
//                Si446xCmd.GET_MODEM_STATUS.LATCH_RSSI
//        );
//    }
//
//    uint16_t rssi;
//
//    rssi = Si446xCmd.GET_MODEM_STATUS.LATCH_RSSI;
//
//    if ( rssi > 200 )                        LCDRSSI( 5 );  //  RSSI 5
//    else if ( 160 < rssi && rssi <= 200 )    LCDRSSI( 4 );  //  RSSI 4
//    else if ( 130 < rssi && rssi <= 160 )    LCDRSSI( 3 );  //  RSSI 3
//    else if ( 100 < rssi && rssi <= 130 )    LCDRSSI( 2 );  //  RSSI 2
//    else if ( 85 < rssi && rssi <= 100 )     LCDRSSI( 1 );  //  RSSI 1
//    else if ( rssi <= 85 )                   LCDRSSI( 0 );  //  RSSI 0
}



//========================================================================
void RF_Tx_Mode()
//========================================================================
{
    //  송신 모드

    HAL_GPIO_WritePin( RF_TX_GPIO_Port, RF_TX_Pin, GPIO_PIN_SET );
    HAL_GPIO_WritePin( RF_RX_GPIO_Port, RF_RX_Pin, GPIO_PIN_RESET );

}

//========================================================================
void RF_Rx_Mode()
//========================================================================
{
    //  수신 모드

    HAL_GPIO_WritePin( RF_TX_GPIO_Port, RF_TX_Pin, GPIO_PIN_RESET );
    HAL_GPIO_WritePin( RF_RX_GPIO_Port, RF_RX_Pin, GPIO_PIN_SET );

}

//========================================================================
void RF_RxTx_Mode()
//========================================================================
{
    //  송/수신 모드

    HAL_GPIO_WritePin( RF_TX_GPIO_Port, RF_TX_Pin, GPIO_PIN_SET );
    HAL_GPIO_WritePin( RF_RX_GPIO_Port, RF_RX_Pin, GPIO_PIN_SET );
}

//========================================================================
void	RFM_Spk			( int bOnOff )		//	1(On) / 0(Off)
//========================================================================
{
    if ( bOnOff )
    {
        //  Spk Relay On
        HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );
    }
    else
    {
        //  Spk Relay Off
        HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );
    }
}

//========================================================================
void PrintVerInfo( void )
//========================================================================
{
    //  Print Version Info

    const char* logo_string = "RFM";

    printf( "\n\n" );
    printf( "%s\n", logo_string );
    printf( "Version = %s(%s - %s)\n", APP_VER, __DATE__, __TIME__ );

    //  LCD Display
    if( GetDevID() == DevRF900T )
    {
        char sBuf[2][32];

        sprintf( sBuf[0], "RFM v%s", APP_VER );
        //        sprintf( sBuf[1], "%s", __DATE__, __TIME__ );

        LCDClear();
        LCDSetCursor( 10, 10 );
        LCDPrintf( sBuf[0] );
        //        LCDSetCursor( 0, 20 );
        //        LCDPrintf( sBuf[1] );
    }
}

//========================================================================
//
//	Command
//
//========================================================================

#include "menu.h"      //  g_cntTrainSet

//========================================================================
int cmd_ch      ( int argc, char * argv[] )
//========================================================================
{
    //	ch [channel] ( 0 ~ 9 )
    int 		nCh = 0;

    switch ( argc )
    {
    case 2:		sscanf( argv[1], "%d", &nCh );	        //	cmd [channel]
//	case 2:		sText = argv[1];						//	sscanf( argv[1], "%s", sText );		//	cmd [Text] 
        break;
    }

    if ( nCh < 0 || g_cntTrainSet <= nCh )  nCh = 0;

    printf( "%s(%d) - Channel : %d\n", __func__, __LINE__, nCh );

    SetTrainSetIdx( nCh );
}


//========================================================================
int cmd_car     ( int argc, char * argv[] )
//========================================================================
{
    //	car [Car No] ( 0 ~ 9 )
    int 		nCar = 0;

    switch ( argc )
    {
    case 2:		sscanf( argv[1], "%d", &nCar );	        //	cmd [Car No]
//	case 2:		sText = argv[1];						//	sscanf( argv[1], "%s", sText );		//	cmd [Text] 
        break;
    }

    if ( nCar < 0 || 10 <= nCar )  nCar = 0;

    printf( "%s(%d) - Car No : %d\n", __func__, __LINE__, nCar );

    SetCarNo( nCar );
}


//========================================================================
int cmd_info    ( int argc, char * argv[] )
//========================================================================
{
    //	info - Train Setting Config
    uint8_t		nBuf[10];
    int 		nCh = 0;
    uint16_t	nData;

    printf( "[Setting]\n" );
    printf( " - Train Set : %s\n", g_sTrainSet[GetTrainSetIdx()] );
    printf( " - Car No : %d\n", GetCarNo() );
}


//========================================================================
