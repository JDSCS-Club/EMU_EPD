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

#if defined(_WIN32)
#else
#include "radio.h"					//  bRadio_Check_Tx_RX()
#include "si446x_api_lib.h"			//	Si446xCmd
#endif

//========================================================================
// Define

//========================================================================

int		g_nDevID		=	DevNone;			//  Device ID ( 1 : RF900M / 2 : RF900T )

int		g_idxTrainSet	=	0;	  				//  Train Set Index

int	 	g_nSpkLevel		=	DefaultSpkVol;	//  Default (1) - 0(Mute) / 1 / 2(Normal) / 3

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

    if ( nCh < 0 || MaxTrainSet <= nCh )  nCh = 0;

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
    printf( " - Train Set : %d\n", 100 + GetTrainSetIdx() );
    printf( " - Car No : %d\n", GetCarNo() );
}


//========================================================================
//		RFM I2S Callback
//========================================================================


static int bRxBuffering = 1;	//  Rx Buffering. ( Packet 4 ~ Packet 0)

//========================================================================
void RFM_I2SEx_TxRxCpltCallback( I2S_HandleTypeDef *hi2s )
//========================================================================
{
	static int idx = 0;
	idx++;

	if ( GetDevID() == DevRF900T )
	{
		//========================================================================
		//  송신기.

		//  r_audio_buff -> RF-Tx

		if ( qBufCnt( &g_qBufAudioTx ) < ( ( I2S_DMA_LOOP_SIZE * 2 ) * ( I2S_DMA_LOOP_QCNT - 1 ) ) )
		{
			//  printf ( "P" );
			//  memset( r_audio_buff, idx, 64 );		//  Data Setting

			//	Queue Put
			qBufPut( &g_qBufAudioTx, (uint8_t *)r_audio_buff, ( I2S_DMA_LOOP_SIZE * 2 ) );
		}

		/*

		//========================================================================
		//  Audio In -> Audio Out Loop
		memcpy( t_audio_buff, r_audio_buff, 64 );

		/*/

		memset( t_audio_buff, 0, 64 );

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
				//			printf ( "G" );
							//  Queue Audio Data
				qBufGet( &g_qBufAudioRx, (uint8_t*)t_audio_buff, ( I2S_DMA_LOOP_SIZE * 2 ) );
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

		memset( t_audio_buff, 0, 64 );
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
				//			printf ( "G" );
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

	if ( GetDevID() == DevRF900M && bRxBuffering == 1 )
	{
		//========================================================================
		//  수신기 & 버퍼링중. -> Skip
	}
	else
	{
		HAL_I2SEx_TransmitReceive_DMA ( &hi2s3, t_audio_buff, r_audio_buff, I2S_DMA_LOOP_SIZE ); // 32byte
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
		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );
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

#if 0

	//	RF 송/수신 설정.

	if ( GetDevID() == DevRF900T )
	{
		for ( int i = 0; i < pRadioConfiguration->Radio_PacketLength; i++ )
			customRadioPacket[i] = i;

		vRadio_StartTx_Variable_Packet ( pRadioConfiguration->Radio_ChannelNumber,
										 &customRadioPacket[0],
										 pRadioConfiguration->Radio_PacketLength );

		//  RF 수신 Start
		vRadio_StartRX(
			pRadioConfiguration->Radio_ChannelNumber,
			pRadioConfiguration->Radio_PacketLength );
	}
	else
	{
		vRadio_StartRX(
			pRadioConfiguration->Radio_ChannelNumber,
			pRadioConfiguration->Radio_PacketLength );
	}
#endif

}



//========================================================================
int RFM_main( void )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	InitRFM();

	int nTick;

	/* Infinite main loop */
	while ( 1 )
	{
		nTick = HAL_GetTick();

		if ( GetDevID() == DevRF900T )
		{
			//	송신기 - KeyPad 동작.
			LoopProcKey( nTick );
		}

		//	Loop RFM
		LoopProcRFM( nTick );

		//	Loop Process ( CLI )
		LoopProcCLI();

		//	Loop Proc Packet : RF Packet
		LoopProcPkt();
	}
}


//========================================================================
void LoopProcRFM ( int nTick )
//========================================================================
{
	static int stampRx;

	static int idx = 0;

	int i;

	RFMPkt	bufRFTx;

	U8 bMain_IT_Status;

	return ;

#if defined(USE_ENV_TEST)

	if ( s_bEnLoopRFTx )
	{
		//	RF 전송모드

		vRadio_StartTx_Variable_Packet (
			pRadioConfiguration->Radio_ChannelNumber,
			&bufRFTx,
			pRadioConfiguration->Radio_PacketLength );

	}

#endif	//	defined(USE_ENV_TEST)

	//========================================================================
	//  Transmit - 송신기
	if ( GetDevID() == DevRF900T )
	{
		if( GetRFMMode() != RFMModeRx )
		{
			//  수신중이 아닌경우 PTT / SOS키 누를시 음성송출.
			static int bPTTOnOff = 0;	   //  PTT On/Off상태
			static int bOldPTT_Key = 1;
			int bPTT_Key;

			bPTT_Key = HAL_GPIO_ReadPin( PTT_KEY_GPIO_Port, PTT_KEY_Pin );

			if( bOldPTT_Key != bPTT_Key )
			{

#if defined(USE_PTT_KEY_TOGGLE)
				if ( bPTT_Key == 1 )
				{
					bPTTOnOff = !( bPTTOnOff );	 //  Toggle Key
				}
#else
				bPTTOnOff = !(bPTT_Key);			//  Press(On) / Release(Off)
#endif // USE_PTT_KEY_TOGGLE

				//  PTT_KEY ON Check
				if( bPTTOnOff )
				{				//	송신중
					LCDSetCursor( 20, 13 );
					LCDPrintf( "방송중..." );

					SetRFMMode( RFMModeTx );

					//  Green LED On
					HAL_GPIO_WritePin ( LED_ON_A_GPIO_Port, LED_ON_A_Pin, GPIO_PIN_SET ); //  Green LED
				}
				else
				{
					//========================================================================
					//  편성 : XXX
					UpdateLCDMain();

					//========================================================================
					RF_Rx_Mode();
					//========================================================================

					SetRFMMode( RFMModeNormal );

					//  Green LED Off
					HAL_GPIO_WritePin ( LED_ON_A_GPIO_Port, LED_ON_A_Pin, GPIO_PIN_RESET ); //  Green LED

					// Start RX with radio packet length
					vRadio_StartRX (
						pRadioConfiguration->Radio_ChannelNumber,
						pRadioConfiguration->Radio_PacketLength );

				}

				bOldPTT_Key = bPTT_Key;
			}

			static int bSOSOnOff = 0;	   //  SOS On/Off상태
			static int bOldSOS_Key = 1;
			int bSOS_Key;

			bSOS_Key = HAL_GPIO_ReadPin( SOS_KEY_GPIO_Port, SOS_KEY_Pin );

			if( bOldSOS_Key != bSOS_Key )
			{
				bSOSOnOff = !( bSOS_Key );

				//  PTT_KEY ON Check
				if( bSOSOnOff )
				{				//	송신중
					LCDSetCursor( 20, 13 );
					LCDPrintf( "통화중..." );

					SetRFMMode( RFMModeTx );

					//  Green LED On
					HAL_GPIO_WritePin ( LED_ON_A_GPIO_Port, LED_ON_A_Pin, GPIO_PIN_SET ); //  Green LED
				}
				else
				{
					//========================================================================
					//  편성 : XXX
					UpdateLCDMain();

					//========================================================================
					RF_Rx_Mode();
					//========================================================================

					SetRFMMode( RFMModeNormal );

					//  Green LED Off
					HAL_GPIO_WritePin ( LED_ON_A_GPIO_Port, LED_ON_A_Pin, GPIO_PIN_RESET ); //  Green LED

					// Start RX with radio packet length
					vRadio_StartRX (
						pRadioConfiguration->Radio_ChannelNumber,
						pRadioConfiguration->Radio_PacketLength );

				}

				bOldSOS_Key = bSOS_Key;
			}

			if( bPTTOnOff || bSOSOnOff )
			{
				//========================================================================
				RF_Tx_Mode();
				//========================================================================

				// PTT Key가 눌려있을시 전송.
				if( qBufCnt( &g_qBufAudioTx ) >= ( I2S_DMA_LOOP_SIZE * 2 ) )
				{
					//		printf ( "G" );
					qBufGet( &g_qBufAudioTx, (uint8_t*)bufRFTx.dat.data, ( I2S_DMA_LOOP_SIZE * 2 ) );

					//  송신기 -> 수신기
					bufRFTx.hdr.addrSrc = GetDevID();		//  Src : 송신기 DevRF900T

					if( bPTTOnOff )
					{
						//  송신기 -> 수신기
						bufRFTx.hdr.addrDest	=	DevRF900M;		 //  Dest : 수신기
						bufRFTx.hdr.nPktID		=	PktPA;
					}
					else
					{
						//  송신기 -> 송신기
						bufRFTx.hdr.addrDest	=	DevRF900T;		 //  Dest : 수신기
						bufRFTx.hdr.nPktID		=	PktCall;
					}

					vRadio_StartTx_Variable_Packet (
						pRadioConfiguration->Radio_ChannelNumber,
						(uint8_t *)&bufRFTx,
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

#if 0
		//========================================================================
		//  On/Off

		static int bOldOnOff;
		int bOnOff;

		bOnOff = HAL_GPIO_ReadPin( ON_OFF_KEY_GPIO_Port, ON_OFF_KEY_Pin );

		if( bOldOnOff != bOnOff )
		{
			if( bOnOff == 0 )
			{
				// On/Off 버튼 눌렀다 뗄경우 Off

				//	Power Off
				HAL_GPIO_WritePin( ON_OFF_EN_GPIO_Port, ON_OFF_EN_Pin, GPIO_PIN_RESET );
			}

			bOldOnOff = bOnOff;
		}

		//========================================================================
		//  Flash Light
		static int bOldKeyLight = 1;
		static int bLightOnOff; //  toggle
		int bKeyLight;

		bKeyLight = HAL_GPIO_ReadPin( DOME4_GPIO_Port, DOME4_Pin );

		if( bOldKeyLight != bKeyLight )
		{
			if( bKeyLight == 1 )
			{
				bLightOnOff = !(bLightOnOff); //  toggle
			}

			if( bLightOnOff )
			{
				//	Flash Light On
				HAL_GPIO_WritePin( FLASH_ON_GPIO_Port, FLASH_ON_Pin, GPIO_PIN_SET );

				LCDLight( 1 );
			}
			else
			{
				//	Flash Light Off
				HAL_GPIO_WritePin( FLASH_ON_GPIO_Port, FLASH_ON_Pin, GPIO_PIN_RESET );

				LCDLight( 0 );
			}


			bOldKeyLight = bKeyLight;
		}

		//========================================================================
		//  Spk
		static int bOldKeySpk = 1;
//		static int bSpkOnOff = 1; //  toggle
		int bKeySpk;

		bKeySpk = HAL_GPIO_ReadPin( DOME6_GPIO_Port, DOME6_Pin );

		if ( bOldKeySpk != bKeySpk )
		{
			if ( bKeySpk == 1 )
			{
//				bSpkOnOff = !(bSpkOnOff); //  toggle
				g_nSpkLevel = ( g_nSpkLevel + 1 ) % 4;  //  0, 1, 2, 3

				//========================================================================
				SetSpkVol( g_nSpkLevel );
				//========================================================================
			}

			if ( g_nSpkLevel )
			{
				//	RFM SPK On
//				HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );
				//	RFM SPK Off
				HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );

				LCDSpeaker( g_nSpkLevel );
			}
			else
			{
				//	RFM SPK Off
				HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );

				LCDSpeaker( 0 );
			}

			bOldKeySpk = bKeySpk;
		}


		//========================================================================
		//  Menu Key
		static int bOldKeyMenu = 1;
		int bKeyMenu;

		bKeyMenu = HAL_GPIO_ReadPin( DOME1_GPIO_Port, DOME1_Pin );

		if ( bOldKeyMenu != bKeyMenu )
		{
			if ( bKeyMenu )
			{
			}
			else
			{
				//	Menu
				ProcBtnMenu();
			}

			bOldKeyMenu = bKeyMenu;
		}

		//========================================================================
		//  OK Key
		static int bOldKeyOK = 1;
		int bKeyOK;

		bKeyOK = HAL_GPIO_ReadPin( DOME3_GPIO_Port, DOME3_Pin );

		if ( bOldKeyOK != bKeyOK )
		{
			if ( bKeyOK )
			{
			}
			else
			{
				ProcBtnOK();
			}

			bOldKeyOK = bKeyOK;
		}

		//========================================================================
		//  Up Key
		static int bOldKeyUp = 1;
		int bKeyUp;

		bKeyUp = HAL_GPIO_ReadPin( DOME2_GPIO_Port, DOME2_Pin );

		if ( bOldKeyUp != bKeyUp )
		{
			if ( bKeyUp )
			{
			}
			else
			{
				ProcBtnUp();
			}

			bOldKeyUp = bKeyUp;
		}

		//========================================================================
		//  Down Key
		static int bOldKeyDown = 1;
		int bKeyDown;

		bKeyDown = HAL_GPIO_ReadPin( DOME5_GPIO_Port, DOME5_Pin );

		if ( bOldKeyDown != bKeyDown )
		{
			if ( bKeyDown )
			{
			}
			else
			{
				ProcBtnDown();
			}

			bOldKeyDown = bKeyDown;
		}
#endif

		//========================================================================
		//  ADC_Power
		//  Normal Mode 일때 Battery 체크.
		//	RSSI 수신감도 체크.
		static int oldTick = 0;

		if ( nTick - oldTick > 1000 )
		{
			//  Period : 1 sec
			if ( GetRFMMode() == RFMModeNormal )
			{
				Adc_Power();

#if defined(USE_RSSI)
				//	RSSI Ping
				RF_RSSI();	//	주기적으로 상태정보 전송.
#endif	//	defined(USE_RSSI)
			}

			oldTick = nTick;
		}

		//========================================================================
	}
	else
	{
		//========================================================================
		//  수신기.
		//  Buffering
		if ( bRxBuffering == 1 &&
			qBufCnt( &g_qBufAudioRx ) > ( ( I2S_DMA_LOOP_SIZE * 2 ) * 3 ) )
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

	bMain_IT_Status = bRadio_Check_Tx_RX();

	switch ( bMain_IT_Status )
	{
	//========================================================================
	//  Transmit
	case SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT:

		HAL_GPIO_TogglePin ( LED_ST_GPIO_Port, LED_ST_Pin );
//		HAL_GPIO_TogglePin ( LED_ON_B_GPIO_Port, LED_ON_B_Pin );

		idx++;

		if ( idx % 250 == 0 )
		{
			printf ( "T" );

			//for ( i = 0; i < 64; i++ )	printf( "%02X ", bufRFTx[i] );
			//printf( "\n" );
		}
		break;

	//========================================================================
	//  Receive
	case SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT:

		HAL_GPIO_TogglePin ( LED_ST_GPIO_Port, LED_ST_Pin );

		//  Queue Buffer Put
//		printf ( "P" );

		RFMPkt	*pRFPkt = (RFMPkt *)&customRadioPacket[0];

		if( GetDevID() == DevRF900T && pRFPkt->hdr.nPktID == PktCall )
		{
			//  송신기
			uint16_t	 *pAudioBuf = (uint16_t*)pRFPkt->dat.data;

			stampRx = nTick;
			SetRFMMode( RFMModeRx );

#if defined(USE_RFT_ONLY_RX_SPK_ON)
			//  송신기 : 수신중인 경우 SPK ON
			HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );
#endif

			//  Red LED On
			HAL_GPIO_WritePin ( LED_ON_B_GPIO_Port, LED_ON_B_Pin, GPIO_PIN_SET ); //  RED LED

			//  통화 : 송신기 -> 송신기
			qBufPut( &g_qBufAudioRx, (uint8_t*)pAudioBuf, ( I2S_DMA_LOOP_SIZE * 2 ) );
		}
		else if (	pRFPkt->hdr.addrSrc == DevRF900T
					&& pRFPkt->hdr.addrDest == DevRF900M
					&& pRFPkt->hdr.nPktID == PktPA
				)
		{
			if ( GetDevID() == DevRF900M )
			{
				//  수신기
				uint16_t	 *pAudioBuf = (uint16_t*)pRFPkt->dat.data;

				//  방송 : 송신기 -> 수신기
				qBufPut( &g_qBufAudioRx, (uint8_t*)pAudioBuf, ( I2S_DMA_LOOP_SIZE * 2 ) );

				// 조명 On
				HAL_GPIO_WritePin ( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_SET );

				stampRx = nTick;
				SetRFMMode( RFMModeRx );

				//  수신기 Spk Relay On
				HAL_GPIO_WritePin( AUDIO_ON_GPIO_Port, AUDIO_ON_Pin, GPIO_PIN_SET );
			}
			else
			{
				//========================================================================
				uint16_t	 *pAudioBuf = (uint16_t*)pRFPkt->dat.data;

#if defined(USE_RFT_ONLY_RX_SPK_ON)
				//  송신기 : 수신중인 경우 SPK ON
				HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );
#endif

				//  방송 : 송신기 -> 수신기
				qBufPut( &g_qBufAudioRx, (uint8_t*)pAudioBuf, ( I2S_DMA_LOOP_SIZE * 2 ) );
				//========================================================================

				//  송신기
				stampRx = nTick;
				SetRFMMode( RFMModeRx );

				//  Red LED On
				HAL_GPIO_WritePin ( LED_ON_B_GPIO_Port, LED_ON_B_Pin, GPIO_PIN_SET ); //  RED LED
			}
		}

		if ( GetDbgLevel() > 0 )
		{
			for ( i = 0; i < 64; i++ )	printf( "%02X ", customRadioPacket[i] );
			printf( "\n" );
		}

		//========================================================================
		//  Status Data
		if ( pRFPkt->hdr.nPktID == PktStat )
		{
			//	상태정보 수신.
			printf ( "[Stat] Car:%d\n", pRFPkt->dat.stat.nCarNo );
		}

		if ( GetDevID() == DevRF900M )
		{
			//  수신기 조명제어.

			if ( pRFPkt->hdr.nPktID == PktLightOff )
			{
				// 조명 Off 명령 수신시.
				HAL_GPIO_WritePin ( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_RESET );
			}
			else if ( pRFPkt->hdr.nPktID == PktLightOn )
			{
				// 조명 Off 명령 수신시.
				HAL_GPIO_WritePin ( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_SET );
			}
		}

		idx++;

		if ( idx % 250 == 0 )
		{
			//  1초간격
			printf ( "R" );
			//for ( i = 0; i < 64; i++ )	printf( "%02X ", customRadioPacket[i] );
			//printf( "\n" );
		}

		if ( GetRFMMode() != RFMModeTx )
		{
			//  송신모드가 아닌경우 수신 Start
			// Start RX with radio packet length
			vRadio_StartRX (
				pRadioConfiguration->Radio_ChannelNumber,
				pRadioConfiguration->Radio_PacketLength );
		}

		break;

	default:
		break;
	}

	//========================================================================
	//	수신중 해제
	if( nTick - stampRx > 500 && GetRFMMode() == RFMModeRx )
	{
		// Rx 패킷이 500 ms 없을 경우 수신모드 해제
		SetRFMMode( RFMModeNormal );

		//  수신기 Spk Relay Off
		HAL_GPIO_WritePin( AUDIO_ON_GPIO_Port, AUDIO_ON_Pin, GPIO_PIN_RESET );

#if defined(USE_RFT_ONLY_RX_SPK_ON)
		if ( GetDevID() == DevRF900T )
		{
			//  송신기 : 수신중이 아닌경우 SPK OFF
			HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );
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



