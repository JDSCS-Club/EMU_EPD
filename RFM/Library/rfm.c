/*------------------------------------------------------------------------------------------
	Project			: RFM
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

#include "Adafruit_SSD1306.h"       //  I2C LCD

#include "version.h"			    //	Version Info

#include "main.h"

#include "audio.h"                  //  WriteI2CCodec()

#include "serial.h"					//  SerialInit()

#include "keypad.h"					//	GetKey()

#if defined(_WIN32)
#else
#include "radio.h"					//  bRadio_Check_Tx_RX()
#include "si446x_api_lib.h"			//	Si446xCmd
#endif

#include "ProcPkt.h"				//	nTxPkt

//========================================================================
// Define

//========================================================================

int		g_nDevID		=	DevNone;			//  Device ID ( 1 : RF900M / 2 : RF900T )

int		g_idxTrainSet	=	0;	  				//  Train Set Index

int	 	g_nSpkLevel		=	DefaultSpkVol;		//  Default (1) - 0(Mute) / 1 / 2(Normal) / 3

int	 	g_nRFMMode 		=	RFMModeNormal;		//  eRFMMode

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
	printf( "%s : %s(%d)\n", __func__,
							( nDevID == DevRF900M )? "RFM":"RFT",
							nDevID );
	g_nDevID = nDevID;
}

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
	if( GetDbgLevel() > 0 )
	{
		printf( "%s : ", __func__ );
		switch( nRFMMode )
		{
		case RFMModeNormal:		printf("Normal");	break;
		case RFMModeTx:			printf("Tx");		break;
		case RFMModeRx:			printf("Tx");		break;
		default:				printf("N/A");		break;
		}
		printf("(%d)\n", nRFMMode);
	}

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

    if ( GetDbgLevel() > 0 )
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

    if ( GetDbgLevel() > 0 )
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

    if ( GetDbgLevel() > 0 )
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

	if ( GetDbgLevel() > 0 )
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
	return ;

    HAL_GPIO_WritePin( RF_TX_GPIO_Port, RF_TX_Pin, GPIO_PIN_SET );
    HAL_GPIO_WritePin( RF_RX_GPIO_Port, RF_RX_Pin, GPIO_PIN_RESET );
}

//========================================================================
void RF_Rx_Mode()
//========================================================================
{
    //  수신 모드
	return ;

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
int cmd_ts      ( int argc, char * argv[] )
//========================================================================
{
    //	ts [train set] ( 0 ~ 9 )
    int 		nTrainSet = 0;

    switch ( argc )
    {
    case 2:		sscanf( argv[1], "%d", &nTrainSet );	        //	cmd [channel]
        break;
    }

    if ( nTrainSet < 0 || MaxTrainSet <= nTrainSet )  nTrainSet = 0;

   	printf( "%s(%d) - Train Set : %d\n", __func__, __LINE__, nTrainSet + 100 );

    SetTrainSetIdx( nTrainSet );
}

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

    if ( nCh < 0 || MaxTrainSet <= nCh )  nCh = 0;

    if ( GetDbgLevel() > 0 )
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

    if ( nCar < 0 || MaxCarNo < nCar )  nCar = 0;

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
    printf( " - Train Set : %d\n", 100 + GetTrainSetIdx() );
    printf( " - Car No : %d\n", GetCarNo() );
}


//========================================================================
int cmd_rfstat    ( int argc, char * argv[] )
//========================================================================
{
    //	rfstat - RF Tx / Rx Status
    printf( "[RF Info] Tx : %d / Rx : %d\n", nTxPkt, nRxPkt );
}


//========================================================================
int cmd_rspid     ( int argc, char * argv[] )
//========================================================================
{
    //	rspid [Car No] [0/1]
    int 		nID 	= 0;
    int 		nSet 	= 0;

    switch ( argc )
    {
    case 3:		sscanf( argv[2], "%d", &nSet );	        //	Set
    case 2:		sscanf( argv[1], "%d", &nID );	        //	ID
//	case 2:		sText = argv[1];						//	sscanf( argv[1], "%s", sText );		//	cmd [Text]
        break;
    }

    if ( nID < 0 || MaxCarNo < nID )
    {
    	printf("%s(%d) - return\n", __func__, __LINE__);
    	return ;
    }

    //	ID Flag Manual 설정.
    g_bSetRspIDManual = 1;

    if ( nSet )
    {
    	//	Set ID Flag
    	g_flagRspID |= ( 0x1 << nID );
    }
    else
    {
    	//	Clear ID Flag
    	g_flagRspID &= ~( 0x1 << nID );
    }

    printf( "%s(%d) - ID Flag : 0x%04X\n", __func__, __LINE__, g_flagRspID );
}



//========================================================================
//		RFM I2S Callback
//========================================================================


//========================================================================
//	interpolation compress ( 보간압축 )
#if defined( USE_AUDIO_INTERPOL_COMPRESS )	//	보간압축사용.

//#define	AUDIO_COMPR_RATE	8	//	Audio 압축율.
//#define	AUDIO_COMPR_RATE	6	//	Audio 압축율.
#define	AUDIO_COMPR_RATE	4	//	Audio 압축율.
//#define	AUDIO_COMPR_RATE	2	//	Audio 압축율.
//#define	AUDIO_COMPR_RATE	1	//	Audio 압축율.

#define	FRAME_ENC_SIZE		(I2S_DMA_LOOP_SIZE * AUDIO_COMPR_RATE)

static int16_t	bufAudioEnc[FRAME_ENC_SIZE * 2];	//	Rx
static int16_t	bufAudioDec[FRAME_ENC_SIZE * 2];	//	Tx

#endif	//	defined( USE_AUDIO_INTERPOL_COMPRESS )	//	보간압축사용.
//========================================================================

static int bRxBuffering = 1;	//  Rx Buffering. ( Packet 4 ~ Packet 0)

//========================================================================
void RFM_I2SEx_TxRxCpltCallback( I2S_HandleTypeDef *hi2s )
//========================================================================
{
	static int idx = 0;
	int16_t		*pAudioTx;
	int16_t		*pAudioRx;

	if ( GetDevID() == DevRF900M && bRxBuffering == 1 )
	{
		//========================================================================
		//  수신기 & 버퍼링중. -> Skip
	}
	else
	{
#if defined( USE_AUDIO_INTERPOL_COMPRESS )	//	보간압축사용.

		pAudioTx = &bufAudioDec[FRAME_ENC_SIZE * idx];
		pAudioRx = &bufAudioEnc[FRAME_ENC_SIZE * (( idx + 1 ) % 2)];

		HAL_I2SEx_TransmitReceive_DMA( &hi2s3,
										pAudioTx,	//	Tx
										pAudioRx,	//	Rx
										FRAME_ENC_SIZE ); // 32byte

		pAudioRx = &bufAudioEnc[FRAME_ENC_SIZE * idx];
		idx = ( idx + 1 ) % 2;
		pAudioTx = &bufAudioDec[FRAME_ENC_SIZE * idx];

#else	//	defined( USE_AUDIO_INTERPOL_COMPRESS )	//	보간압축사용.

		HAL_I2SEx_TransmitReceive_DMA( &hi2s3, t_audio_buff, r_audio_buff, I2S_DMA_LOOP_SIZE ); // 32byte

#endif
	}

	if ( GetDevID() == DevRF900T )
	{
		//========================================================================
		//  송신기.

		//  r_audio_buff -> RF-Tx

		if ( qBufCnt( &g_qBufAudioTx ) < ( ( I2S_DMA_LOOP_SIZE * 2 ) * ( I2S_DMA_LOOP_QCNT - 1 ) ) )
		{
			//  printf ( "P" );
			//  memset( r_audio_buff, idx, 64 );		//  Data Setting

			//========================================================================
#if defined( USE_AUDIO_INTERPOL_COMPRESS )	//	보간압축사용.

			//	Encoding : 8 KHz -> 2 KHz
			int i;
			for( i = 0; i < I2S_DMA_LOOP_SIZE; i++ )
			{
				r_audio_buff[ i ] = pAudioRx[ i * AUDIO_COMPR_RATE ];
			}

#endif	//	defined( USE_AUDIO_INTERPOL_COMPRESS )	//	보간압축사용.
			//========================================================================

			//	Queue Put
			qBufPut( &g_qBufAudioTx, (uint8_t *)r_audio_buff, ( I2S_DMA_LOOP_SIZE * 2 ) );
		}

		/*

		//========================================================================
		//  Audio In -> Audio Out Loop
		memcpy( t_audio_buff, r_audio_buff, 64 );

		/*/

#if defined( USE_AUDIO_INTERPOL_COMPRESS )	//	보간압축사용.
		memset( pAudioTx, 0, FRAME_ENC_SIZE * 2 );	//	Tx
#else
		memset( t_audio_buff, 0, I2S_DMA_LOOP_SIZE * 2 );
#endif

		//========================================================================
		//  Rx Buffering ( Packet Count : 0 ~ 4 )
		//  RF-Rx -> t_audio_buff
		if ( bRxBuffering )
		{
			//  Buffering
			if ( qBufCnt( &g_qBufAudioRx ) > ( ( I2S_DMA_LOOP_SIZE * 2 ) * 3 ) )
			{
				//  패킷이 4개 이상인경우 버퍼링 종료.
				bRxBuffering = 0;

				printf ( "E" );	 //  버퍼링종료
			}
		}

		if ( bRxBuffering == 0 )
		{
			//  Rx Audio Out
			if ( qBufCnt( &g_qBufAudioRx ) >= ( I2S_DMA_LOOP_SIZE * 2 ) )
			{
				//	printf ( "G" );
				//  Queue Audio Data

				qBufGet( &g_qBufAudioRx, (uint8_t*)t_audio_buff, ( I2S_DMA_LOOP_SIZE * 2 ) );

#if defined( USE_AUDIO_INTERPOL_COMPRESS )	//	보간압축사용.

				int dtVal;	//	sample 보간.
				static int16_t nlastSample = 0;

				//	Decoding : 2 KHz -> 8 KHz
				int i;
				for( i = 0; i < FRAME_ENC_SIZE; i++ )
				{
					if ( i % AUDIO_COMPR_RATE == 0 )
					{
						if ( i == 0 )
						{
							pAudioTx[ i ] = nlastSample;	//	t_audio_buff[i / AUDIO_COMPR_RATE];
						}
						else
						{
							pAudioTx[ i ] = t_audio_buff[i / AUDIO_COMPR_RATE - 1];
						}
						dtVal = (t_audio_buff[(i / AUDIO_COMPR_RATE)] - pAudioTx[ i ]) / AUDIO_COMPR_RATE;

						if( (i + AUDIO_COMPR_RATE) == FRAME_ENC_SIZE ) nlastSample = t_audio_buff[(i / AUDIO_COMPR_RATE)];		//	Last Sample
					}
					else
					{
						//	sample간 보간.
						pAudioTx[ i ] = pAudioTx[ i - 1 ] + dtVal;
					}
			//		outBuf[ i ] = bufAudioDec[i / AUDIO_COMPR_RATE];
				}

#endif	//	defined( USE_AUDIO_INTERPOL_COMPRESS )	//	보간압축사용.
			}
			else
			{
				printf ( "B" );	 //  버퍼링시작
				//  Data
				bRxBuffering = 1;
			}
		}
		//  */
	}
	else
	{
		//========================================================================
		//  수신기.

		memset( t_audio_buff, 0, I2S_DMA_LOOP_SIZE * 2 );
		//  Rx Buffering ( Packet Count : 0 ~ 4 )
		//  RF-Rx -> t_audio_buff
		if ( bRxBuffering )
		{
			//  Buffering
			if ( qBufCnt( &g_qBufAudioRx ) > ( ( I2S_DMA_LOOP_SIZE * 2 ) * 3 ) )
			{
				//  패킷이 4개 이상인경우 버퍼링 종료.
				bRxBuffering = 0;

				printf ( "E" );	 //  버퍼링종료
			}
		}

		if ( bRxBuffering == 0 )
		{
			//  Rx Audio Out
			if ( qBufCnt( &g_qBufAudioRx ) >= ( I2S_DMA_LOOP_SIZE * 2 ) )
			{
				//	printf ( "G" );
				//  Queue Audio Data
				qBufGet( &g_qBufAudioRx, (uint8_t *)t_audio_buff, ( I2S_DMA_LOOP_SIZE * 2 ) );
			}
			else
			{
				printf ( "B" );	 //  버퍼링시작

				//  Data
				bRxBuffering = 1;
			}
		}
	}

}


//========================================================================
//		RFM Procedure
//========================================================================

//========================================================================
int InitRFM( void )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	//========================================================================
	I2C_BusScan( &hi2c1 );
	I2C_BusScan( &hi2c3 );

	if ( HAL_OK == HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x50 << 1 ), 2, 2 ) )
	{
		printf( "%s(%d) - EEPROM OK\n", __func__, __LINE__ );
		TestEEPROM( &hi2c1 ); //  Test EEPROM

		//========================================================================
		//  Read TrainSet
		uint8_t	 idxTrainSet;
		idxTrainSet = GetTrainSetIdx();

		if ( idxTrainSet < 0 || idxTrainSet >= MaxTrainSet )
		{
			idxTrainSet = 0;

			SetTrainSetIdx( idxTrainSet );
		}

		g_idxTrainSet = idxTrainSet;

		//========================================================================
		//  Radio Channel 설정.
		pRadioConfiguration->Radio_ChannelNumber = g_idxTrainSet;
	}

	//========================================================================
	//	Car Number 설정.
	g_flagRspID |= (0x1 << GetCarNo());		//	자신의 ID Flag 설정.

	//========================================================================
	//	Codec MAX9860ETG+
	if ( HAL_OK == HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x10 << 1 ), 2, 2 ) )
	{
		//  Read Rev.
		char buf[10];
		int cntRetry;

		cntRetry = 0;

		memset( buf, 0, sizeof( buf ) );
		while ( HAL_I2C_Mem_Read( &hi2c1, (uint16_t)( 0x10 << 1 ), (uint16_t)0xFF, I2C_MEMADD_SIZE_8BIT, buf, (uint16_t)1, 1000 ) != HAL_OK && cntRetry < 10 ) cntRetry++;

		printf( "%s(%d) - Codec ( MAX9860ETG+ ) / Rev. 0x%02X\n", __func__, __LINE__, buf[0] );

		//========================================================================
		//  Codec 초기화.
		InitCodecMAX9860();

		//========================================================================
		//	Init Audio ( I2S PCM )
		AudioInit();

		//========================================================================
		//	I2S Callback 등록.
		SetCallbackI2STxRxCplt( RFM_I2SEx_TxRxCpltCallback );

		//========================================================================
	}

	//========================================================================
	//	OLED
	if ( HAL_OK == HAL_I2C_IsDeviceReady( &hi2c3, (uint16_t)( SSD1306_I2C_ADDRESS ), 2, 2 ) )
	{
		//========================================================================
		//	OLED가 연결되어있음.
		SetDevID( DevRF900T );	 //  송신기.

		//	LCD Init
		LCDInit();

		LCDClear();
		LCDDrawRect( 0, 0, 128, 32, 1 );

		HAL_Delay( 500 );

		//  LCD : 메뉴 화면.
		LCDMenu();

		//  LCD :   편성 : 100
		//========================================================================
		//  편성 : XXX
		UpdateLCDMain();

		//  LCD : Light Icon
		LCDLight( 0 );

		//========================================================================
		//  Spk Vol Set
		//  Default (1) - 0(Mute) / 1 / 2(Normal) / 3

		int	 nSpkVol;
		nSpkVol = GetSpkVol();

		if ( nSpkVol < 0 || nSpkVol > MaxSpkVol )
		{
			nSpkVol = DefaultSpkVol;
			SetSpkVol( nSpkVol );
		}

		g_nSpkLevel = nSpkVol;

		//  LCD : Spk Icon
		LCDSpeaker( g_nSpkLevel );		//  Spk Icon : Spk On
		//========================================================================
	}
	else
	{
		//========================================================================
		//	OLED가 없으면 -> 수신기
		SetDevID( DevRF900M );	 //  수신기.

		//  수신기 스피커 레벨 변경.
		WriteI2CCodec( 0x09, 0x12 );	//  0x12 ( -6 )
//		WriteI2CCodec( 0x09, 0x1A );	//  0x1A ( -10 )
	}
	//========================================================================

	//========================================================================
	//  Default 수신모드.
	RF_Rx_Mode();

	//========================================================================

#if defined(USE_RFT_ONLY_RX_SPK_ON)

	if ( GetDevID() == DevRF900T )
	{
		//  송신기 : 수신중이 아닌경우 SPK OFF
//		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );
		RFM_Spk(0);
	}

#endif


#if defined(USE_RADIO_SI4463)

	//========================================================================
	//	Drivers/si4463
	RF_Init();
	RF_Info();

#else

	//========================================================================
	//	Drivers/radio
	vRadio_Init ();

	//========================================================================
	vRadio_Init ();		//	FIXME : 일부 송신기가 2회 초기화를 해야 초기화되는 경우가 있음.
	//========================================================================

	si446x_part_info ();

	printf ( "=========================\n" );
	printf ( "%08x\n", Si446xCmd.PART_INFO.CHIPREV );
	printf ( "%08x\n", Si446xCmd.PART_INFO.PART );
	printf ( "%08x\n", Si446xCmd.PART_INFO.PBUILD );
	printf ( "%08x\n", Si446xCmd.PART_INFO.ID );
	printf ( "%08x\n", Si446xCmd.PART_INFO.CUSTOMER );
	printf ( "%08x\n", Si446xCmd.PART_INFO.ROMID );
	printf ( "-------------------------\n" );

#endif

}

//========================================================================
int RFM_main( void )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	InitRFM();

	//========================================================================
	//	Radio Spi
	RF_RxTx_Mode();

#if 0

	TestProcPkt();		//	RFM Main

#endif

	InitProcPkt();

	int nTick;

	/* Infinite main loop */
	while ( 1 )
	{
		nTick = HAL_GetTick();

		if ( GetDevID() == DevRF900T )
		{
			//	송신기 - KeyPad 동작.
			LoopProcKey( nTick );

			//	ADC Power : Battery
			LoopProcAdc( nTick );
		}

		//	Loop RFM
		LoopProcRFM( nTick );

		//	Loop Process ( CLI )
		LoopProcCLI();

		//	Loop Proc Packet : RF Packet
		LoopProcPkt( nTick );
	}
}


//========================================================================
void LoopProcRFM ( int nTick )
//========================================================================
{
	RFMPkt	bufRFTx;

	//========================================================================
	//  Transmit - 송신기
	if ( GetDevID() == DevRF900T )
	{
		if( GetRFMMode() != RFMModeRx )
		{
			if( GetKey(eKeyPtt) || GetKey(eKeySos) )
			{
				//========================================================================
				RF_Tx_Mode();
				//========================================================================

				// PTT Key가 눌려있을시 전송.
				if( qBufCnt( &g_qBufAudioTx ) >= ( I2S_DMA_LOOP_SIZE * 2 ) )
				{
					//		printf ( "G" );
					qBufGet( &g_qBufAudioTx, (uint8_t*)bufRFTx.dat.data, ( I2S_DMA_LOOP_SIZE * 2 ) );

#if defined(USE_HOPPING)
					//========================================================================
					//	Packet Header
					if( GetKey(eKeyPtt) )
					{
						_MakePktHdr( &bufRFTx, GetDevID(), 0xFF, sizeof( RFMPktPACall ), PktPA );
					}
					else
					{
						_MakePktHdr( &bufRFTx, GetDevID(), 0xFF, sizeof( RFMPktPACall ), PktCall );
					}

#else
					//  송신기 -> 수신기
					bufRFTx.hdr.addrSrc = GetDevID();		//  Src : 송신기 DevRF900T

					if( GetKey(eKeyPtt) )	// bPTTOnOff )
					{
						//  송신기 -> 수신기
						bufRFTx.hdr.addrDest	=	DevRF900M;		 //  Dest : 수신기
					}
					else
					{
						//  송신기 -> 송신기
						bufRFTx.hdr.addrDest	=	DevRF900T;		 //  Dest : 수신기
					}

					if( GetKey(eKeyPtt) )	bufRFTx.hdr.nPktCmd = PktPA;	//  송신기 -> 수신기
					else					bufRFTx.hdr.nPktCmd = PktCall;	//  송신기 -> 송신기
#endif


					SendPacket( (uint8_t *)&bufRFTx,
						pRadioConfiguration->Radio_PacketLength );
				}
			}
		}

		//========================================================================
		static int nOldRFMMode = 0;

		int nRFMMode = GetRFMMode();
		if( nRFMMode != nOldRFMMode )
		{
			switch ( nRFMMode )
			{
			case RFMModeRx:
				LCDSetCursor( 20, 13 );
				LCDPrintf( "수신중..." );
				break;

			case RFMModeNormal:
				//========================================================================
				//  편성 : XXX
				UpdateLCDMain();
				break;
			}

			nOldRFMMode = nRFMMode;
		}
	}
	else
	{
		//========================================================================
		//  수신기.
		//  Buffering
		if ( bRxBuffering == 1 &&
			qBufCnt( &g_qBufAudioRx ) > ( ( I2S_DMA_LOOP_SIZE * 2 ) * 2 ) )
		{
			//  버퍼링 완료시점.
			//	  I2S 구동 재계.
			HAL_I2SEx_TxRxCpltCallback( &hi2s3 );
		}

		//========================================================================
		//	RSSI 수신감도 체크.
		static int oldTick = 0;

		if ( nTick - oldTick > 1000 )
		{
			//  Period : 1 sec
			if ( GetRFMMode() == RFMModeNormal )
			{
#if defined(USE_RSSI)
				//	RSSI Ping
				RF_RSSI();	//	주기적으로 상태정보 전송.
#endif	//	defined(USE_RSSI)
			}

			oldTick = nTick;
		}
	}
	//========================================================================

	//========================================================================
	//	수신중 해제
	if( nTick - nRxStamp > 500 && GetRFMMode() == RFMModeRx )
	{
		// Rx 패킷이 500 ms 없을 경우 수신모드 해제
		SetRFMMode( RFMModeNormal );

		//  수신기 Spk Relay Off
		HAL_GPIO_WritePin( AUDIO_ON_GPIO_Port, AUDIO_ON_Pin, GPIO_PIN_RESET );

#if defined(USE_RFT_ONLY_RX_SPK_ON)
		if ( GetDevID() == DevRF900T )
		{
			//  송신기 : 수신중이 아닌경우 SPK OFF
//			HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );
			RFM_Spk(0);
		}
#endif

		//  Red LED Off
		HAL_GPIO_WritePin ( LED_ON_B_GPIO_Port, LED_ON_B_Pin, GPIO_PIN_RESET ); //  RED LED
	}

	//  수신기 Standby GPIO 제어 : 모듈동작시 1초간격 Blink
	static int s_nTickStandby;

	if ( nTick - s_nTickStandby >= 1000 )
	{
		//	Standby GPIO Toggle
		if ( GetRFMMode() == RFMModeRx )
		{
			//  수신기:수신중 LED On ( Standby LED On )
			HAL_GPIO_WritePin( STANDBY_GPIO_Port, STANDBY_Pin, GPIO_PIN_SET );
		}
		else
		{
			//  Standby LED Toggle
			HAL_GPIO_TogglePin( STANDBY_GPIO_Port, STANDBY_Pin );
		}

#if defined(USE_SEND_STATUS)	//	상태 정보 전송.
		//========================================================================
		RF_Ping();	//	상태정보전송.
		//========================================================================
#endif

		s_nTickStandby = nTick;
	}
}


#if defined(USE_ENV_TEST)

static int	s_bEnLoopRFTx = 0;

//========================================================================
void	SetLoopRFTx		( int bEnable )	//	RF Data 전송 유지..
//========================================================================
{
	printf( "%s(%d) - Loop RF Tx : %d\n", __func__, __LINE__, bEnable );

	if ( bEnable )
	{
		RF_Tx_Mode();
	}
	else
	{
		RF_Rx_Mode();
	}

	s_bEnLoopRFTx = bEnable;
}


//========================================================================
int cmd_rftx      ( int argc, char * argv[] )
//========================================================================
{
	//	bEnable ( 1 / 0 )
	int bEnable = 0;
	int nCh = 0;	//	Channel
	uint8_t nPA = 0x7F;	//	Power Amplifier
	char *sRFType;

	switch ( argc )
	{
	case 5:		sscanf( argv[4], "%d", &nPA );		//	Power Amplifier
	case 4:		sscanf( argv[3], "%d", &nCh );		//	Channel
	case 3:		sRFType = argv[2];					//	RF Type [ u (unmodulated) / m(modulated) ]
	case 2:		sscanf( argv[1], "%d", &bEnable );	//	cmd [Enable]
//	case 2:		sText = argv[1];					//	sscanf( argv[1], "%s", sText );		//	cmd [Text]
		break;
	}

	printf( "%s(%d) - En(%d) / Type(%s) / Ch(%d) / PA(%d)\n", __func__, __LINE__,
			bEnable, sRFType, nCh, nPA );

	//========================================================================
	//	채널 설정.
	pRadioConfiguration->Radio_ChannelNumber = nCh;

	//========================================================================
	//	RF Type
	if ( strcmp( sRFType, "u" ) == 0 )
	{
		SetLoopRFTx( 0 );	//	RF Loop 종료.
		//	Continuous Wave ( 무변조방식 )
		vRadio_Init_cw();	//	무변조방식 설정.

		vRadio_RF_Pwr( nPA );

//		vRadio_StartTx( nCh, NULL );
		vRadio_StartTx( pRadioConfiguration->Radio_ChannelNumber, NULL );

		return 0;
	}
	else if ( strcmp( sRFType, "m" ) == 0 )
	{
		vRadio_Init();	//	변조방식 설정.
		//========================================================================
		//	RF출력 설정.
		vRadio_RF_Pwr( nPA );
	}

	if ( argc >= 2 )
	{
		if ( strcmp( argv[1], "1" ) == 0 )			bEnable = 1;
		else if ( strcmp( argv[1], "0" ) == 0 )		bEnable = 0;
	}

	printf( "%s(%d) - Loop RF Tx : %d\n", __func__, __LINE__, bEnable );

	SetLoopRFTx( bEnable );
}

#endif	//	defined(USE_ENV_TEST)



