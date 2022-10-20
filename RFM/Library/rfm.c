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

#include "adpcm.h"					//	ADPCM Codec

//========================================================================
// Define

//========================================================================

int		g_nDevID		=	DevNone;			//  Device ID ( 1 : RF900M / 2 : RF900T )

int		g_nDevFlag		=	DevFlagNone;		//  Device Flag ( Light )

int		g_idxTrainSet	=	0;	  				//  Train Set Index
int		g_nCarNo		=	0;	  				//  Car Number

int		g_nChRx			=	_ChTS1_1;			//  RF Rx Channel

int	 	g_nSpkLevel		=	DefaultSpkVol;		//  Default (1) - 0(Mute) / 1 / 2(Normal) / 3

int	 	g_nRFMMode 		=	RFMModeNormal;		//  eRFMMode

int		g_offsetCA		=	0;					//	충돌회피 Offset ( msec ) ( 0 ~ 1000 )

int		g_nStampRxPkt	=	0;					//	방송/통화 Stamp

int		g_nRSSI			=	0;					//	RSSI Value

#if defined(USE_RF_COMM_MODE)
int		g_nRFMode		=	0;					//	RF Mode 무선 중계 모드. eRFMode ( RFMode1 / RFMode2 )
#endif

#if defined(USE_HOP_MANUAL)
int		g_nManHopping	=	0;					//	On(1) / Off(2) / Unused(0 : Other)
#endif	//	defined(USE_HOP_MANUAL)

//	Device Stat
RFMDevStat		g_devStat[ MaxCarNo ] = { 0, };



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
	if ( g_nRFMMode != nRFMMode && nRFMMode == RFMModeNormal )
	{
		//	타 모드에서 Normal 모드로 변경시 상태정보 ID Flag값 유지.
		ReloadStampStat();
	}

	if ( g_nRFMMode != nRFMMode )
	{
		//	타 모드에서 Normal 모드로 변경시 상태정보 ID Flag값 유지.
		printf( "%s", StrRFMMode( nRFMMode ) );
	}

	g_nRFMMode = nRFMMode;
}

//========================================================================
char	*StrRFMMode		( int nRFMMode )
//========================================================================
{
	switch( nRFMMode )
	{
	case RFMModeNormal:		return "[Normal]";
	case RFMModeTx:			return "[Tx]";
	case RFMModeRx:			return "[Rx]";
	case RFMModeOcc:		return "[Occ]";
	case RFMModeUpgr:		return "[Upgr]";
	default:				return "[N/A]";
	}
}

//========================================================================
int	GetRFTID( void )	//	송신기 ID
//========================================================================
{
	//	송신기 ID 얻기.
	//========================================================================
	//	송신기 #1 / #2
	//	ChTx_1			=	8,			//	* CH8 : 송신기#1 - (Car No : 11)
	//	ChTx_2			=	9,			//	* CH9 : 송신기#2 - (Car No : 12)
	if ( g_nCarNo == RFTCarNo1 ) return 1;			//	11
	else if ( g_nCarNo == RFTCarNo2 ) return 2;		//	12
	else return 0;
	//========================================================================
}

//========================================================================
int GetChRx( void )
//========================================================================
{
	//	Get Self Rx Channel

	//========================================================================
#if defined(USE_CH_ISO_DEV)

	if ( GetDevID() == DevRF900T )
	{
		//========================================================================
		//	송신기 #1 / #2
		//	ChTx_1			=	8,				//	* CH8 : 송신기#1 - (Car No : 11)
		//	ChTx_2			=	ChTx_1+ChGap,	//	* CH9 : 송신기#2 - (Car No : 12)
		return ChTx_1 + ( ( g_nCarNo + 1) % 2 )*ChGap;	// 현재 호차 채널
		//========================================================================
	}
	else if ( GetDevID() == DevRF900M )
	{
		if( g_nRFMode == RFMode2 )//#if defined(USE_COMM_MODE_CH_GRP)
		{
			//	그룹주파수 모드. - [ 1, 2 ] [ 3, 4 ] ...
			return ChTS1_1 + (( g_nCarNo - 1 )/2)*ChGap;	// 현재 호차 채널
		}
		else	//	#else
		{
			//========================================================================
			//	수신기.
			//	ChTS1_1			=	11,			//	* CH11 : 1편성 ( 1호차 )
			//	ChTS1_2			=	12,			//	* CH12 : 1편성 ( 2호차 )
			//
			//		...
			//
			//	ChTS1_10		=	20,			//	* CH20 : 1편성 ( 10호차 )
			return ChTS1_1 + ( g_nCarNo - 1 )*ChGap;	// 현재 호차 채널
			//========================================================================
		}	//	#endif
	}

#else
	//========================================================================
	//	CH1 : 1, 3, 5
	//	CH2 :  2, 4, 6
	return ChTS1_1 + g_idxTrainSet * 2 + ( ( g_nCarNo + 1 ) % 2 );	// 현재 호차 채널
#endif
	//========================================================================
}

//========================================================================
int		GetChOtherRFT	( void )			//	타 송신기 채널.
//========================================================================
{
	//========================================================================
	//	송신기 #1 / #2
	//	ChTx_1			=	8,				//	* CH8 : 송신기#1 - (Car No : 11)
	//	ChTx_2			=	9,				//	* CH9 : 송신기#2 - (Car No : 12)
//	return ChTx_1 + ( g_nCarNo % 2 );		// Self 송신기 채널
	return ChTx_1 + ( ( g_nCarNo ) % 2 )*ChGap;	// Other 송신기 채널
	//========================================================================
}


//========================================================================
int		IsNearRFT	( void )			//	가까운 송신기 존재 유무 확인.
//========================================================================
{
	int nCh = ChTS1_1;		//	Default

	//	가장가까운 호차 검색.
	int nMaxRSSI = 0;
	for ( int idx = 1; idx <= 10; idx++ )
	{
		//	Car #1 ~ #10
		if ( g_devStat[idx].nRSSI > nMaxRSSI )
		{
			nCh = g_devStat[idx].stat.nChRx;		//	채널 설정.
			nMaxRSSI = g_devStat[idx].nRSSI;
		}
	}

	return nCh;
}


//========================================================================
int		GetChNearRFT	( int nMinRSSI )			//	가장 가까운 송신기 채널.
//========================================================================
{
	int nCh = 0;		//	Default

	//	가장가까운 송신기 검색.
	int nMaxRSSI = 0;
	for ( int idx = 11; idx <= 12; idx++ )			//	송신기 채널 검색.
	{
		//	Car #1 ~ #10
		if ( g_devStat[idx].nRSSI > nMaxRSSI )
		{
			nCh = g_devStat[idx].stat.nChRx;		//	채널 설정.
			nMaxRSSI = g_devStat[idx].nRSSI;
		}
	}

	if ( nMinRSSI > nMaxRSSI )
	{
		//	RSSI값이 한계 이상 낮은경우 선택 X
		nCh = 0;
	}

	return nCh;
}

//========================================================================
int		GetChNearRFM	( void )			//	가장 가까운 수신기 채널.
//========================================================================
{
	int nCh = ChTS1_1;		//	Default

	//	가장가까운 호차 검색.
	int nMaxRSSI = 0;
	for ( int idx = 1; idx <= 10; idx++ )
	{
		//	Car #1 ~ #10
		if ( g_devStat[idx].nRSSI > nMaxRSSI )
		{
			nCh = g_devStat[idx].stat.nChRx;		//	채널 설정.
			nMaxRSSI = g_devStat[idx].nRSSI;
		}
	}

	return nCh;
}

//========================================================================
//	방송채널 : 송신기에서 수신기로 송신할 방송채널 설정.
int		g_nChPA	=	_ChTS1_1;		//	Default

//========================================================================
void	SetChPA( int nCh )
//========================================================================
{
	//	방송채널 설정.
	g_nChPA	= nCh;
}

//========================================================================
int		GetChPA( void )
//========================================================================
{
	//	방송채널 설정.
	return g_nChPA;
}

//========================================================================
int		GetCh2Car( int nCh )	//	채널 -> 호차정보 변환.
//========================================================================
{
	//	채널 -> 호차정보.
	int nCar;
	if ( nCh == ChTx_1 )		nCar = RFTCarNo1;	//	송신기#1
	else if ( nCh == ChTx_2 )	nCar = RFTCarNo2;	//	송신기#2
	else
	{
		//	ChTS1_1 - 21	:	1호차.
		//	1 ~ 10호차.
		nCar = ( nCh - ChTS1_1 ) / ChGap + 1;
	}

	return nCar;
}



int		g_nChRFT	=	0;		//	Default

//========================================================================
void	SetChPARFT( int nCh )	//	가까운 송신기 채널(방송/통화용) 설정.
//========================================================================
{
	//	방송채널 설정.
	g_nChRFT	= nCh;
}

//========================================================================
int		GetChPARFT( void )		//	가까운 송신기 채널(방송/통화용) 설정.
//========================================================================
{
	//	방송채널 설정.
	return g_nChRFT;
}

#if defined(USE_ROUTE_REQ)

int		g_nChRFMUp		=	0;		//	Default
int		g_nChRFMDown	=	0;		//	Default

int		g_nStampRouteRsp		=	0;	//	Route Rsp TimeStamp
int		g_nIdxRouteFindNext		=	0;	//	Next호차 Find Index / Route Rsp 수신시 Reset

//========================================================================
void	SetChRFMUp( int nCh )	//	가까운 수신기 채널 ( Up )
//========================================================================
{
	//	방송채널 설정.
	g_nChRFMUp	= nCh;
}

//========================================================================
int		GetChRFMUp( void )		//	가까운 수신기 채널 ( Up )
//========================================================================
{
	//	방송채널 설정.
	return g_nChRFMUp;
}

//========================================================================
void	SetChRFMDown( int nCh )	//	가까운 수신기 채널 ( Down )
//========================================================================
{
	//	방송채널 설정.
	g_nChRFMDown	= nCh;
}

//========================================================================
int		GetChRFMDown( void )	//	가까운 수신기 채널 ( Down )
//========================================================================
{
	//	방송채널 설정.
	return g_nChRFMDown;
}


#endif

//========================================================================

#include "eeprom.h"    //  EEPROM

//========================================================================
int		LoadTrainSetIdx	( void )
//========================================================================
{
    uint8_t     idxTrainSet = 0;

    if ( HAL_OK != HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x50 << 1 ), 2, 2 ) )
    {
        printf( "%s(%d) - EEPROM Error\n", __func__, __LINE__ );

        return -1;
    }

//    M24_HAL_ReadBytes( &hi2c1, 0xA0, 0x10, (uint8_t *)&idxTrainSet, 1 );
    M24_HAL_ReadBytes( &hi2c1, 0xA0, AddrEEPTrainSet, (uint8_t *)&idxTrainSet, 1 );

    if ( GetDbg() > 0 )
    	printf( "%s(%d) - %d\n", __func__, __LINE__, idxTrainSet );

    //========================================================================
#if defined(USE_ODD_TS_CH_SHIFT)
    if( (idxTrainSet%2) == 1 )
    {
    	//	홀수 편성 채널 ( Shift ChGap/2 )
    	ChTx_1		=	ChTx_1 + (ChGap/2);
    	ChTx_2		=	ChTx_2 + (ChGap/2);
        ChTS1_1		= 	ChTS1_1 + (ChGap/2);
    }
#endif
    //========================================================================


    return idxTrainSet;
}


//========================================================================
int		GetTrainSetIdx		( void )
//========================================================================
{
	static int s_bOnce 			=	0;

	if ( s_bOnce == 0 )
	{
		//	초기로딩시 I2C에서 Load
		g_idxTrainSet = LoadTrainSetIdx();
		s_bOnce = 1;
	}

	return g_idxTrainSet;
}


//========================================================================
void	SetTrainSetIdx	( int idxTrainSet )
//========================================================================
{
	g_idxTrainSet = idxTrainSet;

    if ( HAL_OK != HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x50 << 1 ), 2, 2 ) )
    {
        printf( "%s(%d) - EEPROM Error\n", __func__, __LINE__ );

        return ;
    }

    if ( GetDbg() > 0 )
    	printf( "%s(%d) - %d\n", __func__, __LINE__, idxTrainSet );
//    M24_HAL_WriteBytes( &hi2c1, 0xA0, 0x10, (uint8_t *)&idxTrainSet, 1 );
    M24_HAL_WriteBytes( &hi2c1, 0xA0, AddrEEPTrainSet, (uint8_t *)&idxTrainSet, 1 );
}

#if defined(USE_RF_COMM_MODE)

//========================================================================
int		GetRFMode	( void )
//========================================================================
{
    uint8_t     nRFMode = 0;

    if ( HAL_OK != HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x50 << 1 ), 2, 2 ) )
    {
        printf( "%s(%d) - EEPROM Error\n", __func__, __LINE__ );

        return -1;
    }

    M24_HAL_ReadBytes( &hi2c1, 0xA0, AddrEEPRFMode, (uint8_t *)&nRFMode, 1 );

    if ( nRFMode > RFModeMax || nRFMode < 1 ) nRFMode = RFModeDefault;	//	Default Hop Man

    if ( GetDbg() > 0 )
    	printf( "%s(%d) - %d\n", __func__, __LINE__, nRFMode );

    return nRFMode;
}

//========================================================================
void	SetRFMode	( int nRFMode )
//========================================================================
{
    if ( HAL_OK != HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x50 << 1 ), 2, 2 ) )
    {
        printf( "%s(%d) - EEPROM Error\n", __func__, __LINE__ );

        return ;
    }

    if ( GetDbg() > 0 )
    	printf( "%s(%d) - %d\n", __func__, __LINE__, nRFMode );

    M24_HAL_WriteBytes( &hi2c1, 0xA0, AddrEEPRFMode, (uint8_t *)&nRFMode, 1 );
}

#endif

#if defined(USE_HOP_MANUAL)

//========================================================================
int		GetManHop	( void )
//========================================================================
{
    uint8_t     nManHop = 0;

    if ( HAL_OK != HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x50 << 1 ), 2, 2 ) )
    {
        printf( "%s(%d) - EEPROM Error\n", __func__, __LINE__ );

        return -1;
    }

//    M24_HAL_ReadBytes( &hi2c1, 0xA0, 0x0D, (uint8_t *)&nManHop, 1 );
    M24_HAL_ReadBytes( &hi2c1, 0xA0, AddrEEPManHop, (uint8_t *)&nManHop, 1 );

//    if ( nManHop > 2 || nManHop < 0 ) nManHop = 0;
    if ( nManHop > 2 || nManHop < 0 ) nManHop = DEFAULT_HOP_MAN_VAL;	//	Default Hop Man

    if ( GetDbg() > 0 )
    	printf( "%s(%d) - %d\n", __func__, __LINE__, nManHop );

    return nManHop;
}

//========================================================================
void	SetManHop	( int nManHop )
//========================================================================
{
    if ( HAL_OK != HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x50 << 1 ), 2, 2 ) )
    {
        printf( "%s(%d) - EEPROM Error\n", __func__, __LINE__ );

        return ;
    }

    if ( GetDbg() > 0 )
    	printf( "%s(%d) - %d\n", __func__, __LINE__, nManHop );

//    M24_HAL_WriteBytes( &hi2c1, 0xA0, 0x0D, (uint8_t *)&nManHop, 1 );
    M24_HAL_WriteBytes( &hi2c1, 0xA0, AddrEEPManHop, (uint8_t *)&nManHop, 1 );
}

#endif	//	defined(USE_HOP_MANUAL)

//========================================================================
int		LoadCarNo		( void )
//========================================================================
{
    uint8_t     nCarNo = 0;

    if ( HAL_OK != HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x50 << 1 ), 2, 2 ) )
    {
        printf( "%s(%d) - EEPROM Error\n", __func__, __LINE__ );

        return -1;
    }

//    M24_HAL_ReadBytes( &hi2c1, 0xA0, 0x0E, (uint8_t *)&nCarNo, 1 );
    M24_HAL_ReadBytes( &hi2c1, 0xA0, AddrEEPCarNo, (uint8_t *)&nCarNo, 1 );

    if ( GetDbg() > 0 )
    	printf( "%s(%d) - %d\n", __func__, __LINE__, nCarNo );

    g_nCarNo = nCarNo;

    return nCarNo;
}

//========================================================================
int		GetCarNo		( void )
//========================================================================
{
	static int s_bOnce = 0;

	if ( s_bOnce == 0 )
	{
		//	초기로딩시 I2C에서 Load
		g_nCarNo = LoadCarNo();
		s_bOnce = 1;
	}

	return g_nCarNo;
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

	if ( GetDbg() > 0 )
    	printf( "%s(%d) - %d\n", __func__, __LINE__, nCarNo );

	g_nCarNo = nCarNo;

//    M24_HAL_WriteBytes( &hi2c1, 0xA0, 0x0E, (uint8_t *)&nCarNo, 1 );
    M24_HAL_WriteBytes( &hi2c1, 0xA0, AddrEEPCarNo, (uint8_t *)&nCarNo, 1 );
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

//    M24_HAL_ReadBytes( &hi2c1, 0xA0, 0x0F, (uint8_t *)&nSpkVol, 1 );
    M24_HAL_ReadBytes( &hi2c1, 0xA0, AddrEEPSpkVol, (uint8_t *)&nSpkVol, 1 );

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

//    M24_HAL_WriteBytes( &hi2c1, 0xA0, 0x0F, (uint8_t *)&nSpkVol, 1 );
    M24_HAL_WriteBytes( &hi2c1, 0xA0, AddrEEPSpkVol, (uint8_t *)&nSpkVol, 1 );

    //========================================================================
    //	Codec MAX9860ETG+
    if ( HAL_OK == HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x10 << 1 ), 2, 2 ) )
    {
    	AudioSpkVol( nSpkVol );
    }
}


//========================================================================
int		GetMicVol	    ( void )
//========================================================================
{
    uint8_t     nMicVol = 0;

    if ( HAL_OK != HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x50 << 1 ), 2, 2 ) )
    {
        printf( "%s(%d) - EEPROM Error\n", __func__, __LINE__ );

        return -1;
    }

//    M24_HAL_ReadBytes( &hi2c1, 0xA0, 0x0F, (uint8_t *)&nSpkVol, 1 );
    M24_HAL_ReadBytes( &hi2c1, 0xA0, AddrEEPMicVol, (uint8_t *)&nMicVol, 1 );

    printf( "%s(%d) - %d\n", __func__, __LINE__, nMicVol );

    return nMicVol;
}

//========================================================================
void	SetMicVol	    ( int nMicVol )
//========================================================================
{
    if ( HAL_OK != HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x50 << 1 ), 2, 2 ) )
    {
        printf( "%s(%d) - EEPROM Error\n", __func__, __LINE__ );

        return ;
    }

    printf( "%s(%d) - %d\n", __func__, __LINE__, nMicVol );

//    M24_HAL_WriteBytes( &hi2c1, 0xA0, 0x0F, (uint8_t *)&nSpkVol, 1 );
    M24_HAL_WriteBytes( &hi2c1, 0xA0, AddrEEPMicVol, (uint8_t *)&nMicVol, 1 );

    //========================================================================
    //	Codec MAX9860ETG+
    if ( HAL_OK == HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x10 << 1 ), 2, 2 ) )
    {
    	AudioMicVol( nMicVol );
    }
}
//========================================================================


//========================================================================
void RF_RSSI( void )
//========================================================================
{
    //  Send RF Ping Packet.
//    return;
//
    //  Get Modem Status.
    si446x_get_modem_status_fast_clear_read();
//	si446x_get_modem_status(0xff);

    if ( GetDbg() > 0 )
    {
        printf( "%s(%d) - ant1:%d / ant2:%d / curr:%d / latch:%d\n", __func__, __LINE__,
                Si446xCmd.GET_MODEM_STATUS.ANT1_RSSI,
                Si446xCmd.GET_MODEM_STATUS.ANT2_RSSI,
                Si446xCmd.GET_MODEM_STATUS.CURR_RSSI,
                Si446xCmd.GET_MODEM_STATUS.LATCH_RSSI
        );
    }

    uint16_t rssi;

    rssi = Si446xCmd.GET_MODEM_STATUS.LATCH_RSSI;

    if ( rssi > 200 )                        LCDRSSI( 5 );  //  RSSI 5
    else if ( 160 < rssi && rssi <= 200 )    LCDRSSI( 4 );  //  RSSI 4
    else if ( 130 < rssi && rssi <= 160 )    LCDRSSI( 3 );  //  RSSI 3
    else if ( 100 < rssi && rssi <= 130 )    LCDRSSI( 2 );  //  RSSI 2
    else if ( 85 < rssi && rssi <= 100 )     LCDRSSI( 1 );  //  RSSI 1
    else if ( rssi <= 85 )                   LCDRSSI( 0 );  //  RSSI 0
}

//========================================================================
void	RFM_Spk			( int bOnOff )		//	1(On) / 0(Off)
//========================================================================
{
	if( GetDbg() > 1 )
		printf("%s(%d) - %d\n", __func__, __LINE__, bOnOff);

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

    if( argc <= 1 )
    {
        printf( "Train Set : %d\n", 100 + GetTrainSetIdx() );

    	return 0;
    }

    switch ( argc )
    {
    case 2:		sscanf( argv[1], "%d", &nTrainSet );	        //	cmd [channel]
        break;
    }

    if ( nTrainSet < 0 || MaxTrainSet <= nTrainSet )  nTrainSet = 0;

   	printf( "%s(%d) - Train Set : %d\n", __func__, __LINE__, nTrainSet + 100 );

    SetTrainSetIdx( nTrainSet );

    return 0;
}

//========================================================================
int cmd_ch      ( int argc, char * argv[] )
//========================================================================
{
    //	ch [channel] ( 0 ~ 9 )
    int 		nCh = 0;

    if( argc <= 1 )
    {
        printf( "RF Channel: %d\n", GetChRx() );

    	return 0;
    }

    switch ( argc )
    {
    case 2:		sscanf( argv[1], "%d", &nCh );	        //	cmd [channel]
//	case 2:		sText = argv[1];						//	sscanf( argv[1], "%s", sText );		//	cmd [Text] 
        break;
    }

    if ( nCh < 0 || MaxTrainSet <= nCh )  nCh = 0;

    if ( GetDbg() > 0 )
    	printf( "%s(%d) - Channel : %d\n", __func__, __LINE__, nCh );

    SetTrainSetIdx( nCh );

	return 0;
}


//========================================================================
int cmd_car     ( int argc, char * argv[] )
//========================================================================
{
    //	car [Car No] ( 0 ~ 9 )
    int 		nCar = 0;

    if( argc <= 1 )
    {
        printf( "Car No : %d\n", GetCarNo() );

        return 0;
    }

    switch ( argc )
    {
    case 2:		sscanf( argv[1], "%d", &nCar );	        //	cmd [Car No]
//	case 2:		sText = argv[1];						//	sscanf( argv[1], "%s", sText );		//	cmd [Text] 
        break;
    }

    if ( nCar < 0 || MaxCarNo < nCar )  nCar = 0;

    printf( "%s(%d) - Car No : %d\n", __func__, __LINE__, nCar );

    SetCarNo( nCar );

	return 0;
}


//========================================================================
int cmd_hop     ( int argc, char * argv[] )
//========================================================================
{
    //	car [Car No] ( 0 ~ 9 )
    int 		nManHop = 0;

    switch ( argc )
    {
    case 2:		sscanf( argv[1], "%d", &nManHop );	        //	cmd [Car No]
//	case 2:		sText = argv[1];						//	sscanf( argv[1], "%s", sText );		//	cmd [Text]
        break;
    }

//  g_nManHopping;		//	On(1) / Off(2) / Unused(0 : Other)
    if ( nManHop < 0 || 2 < nManHop )  nManHop = 0;

    printf( "%s(%d) - Manual Hop : %d\n", __func__, __LINE__, nManHop );

    g_nManHopping = nManHop;
    SetManHop( nManHop );
}


//========================================================================
int cmd_rfmod     ( int argc, char * argv[] )
//========================================================================
{
    int 		nRFMode = 0;

    switch ( argc )
    {
    case 2:		sscanf( argv[1], "%d", &nRFMode );	        //	cmd [Car No]
//	case 2:		sText = argv[1];						//	sscanf( argv[1], "%s", sText );		//	cmd [Text]
        break;
    }

//  g_nManHopping;		//	On(1) / Off(2) / Unused(0 : Other)
    if ( nRFMode < 1 || RFModeMax < nRFMode )  nRFMode = RFModeDefault;

    printf( "%s(%d) - RFMode : %d\n", __func__, __LINE__, nRFMode );

    g_nRFMode = nRFMode;
    SetRFMode( nRFMode );
}

//========================================================================
int cmd_swinfo    ( int argc, char * argv[] )
//========================================================================
{
    printf( "[S/W Info]\n" );
    printf( " - Boot : %d Byte\n", FLASH_If_GetBootSize() );
    printf( " - App : %d Byte\n", FLASH_If_GetAppSize() );
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
    printf( " - RF Channel: %d\n", GetChRx() );
    printf( " - Mode : %s(%d)\n", StrRFMMode( GetRFMMode() ), GetRFMMode()  );	//	Normal / Tx / Rx / Upgrade
    printf( " - GetChNearRFM() : %d\n", GetChNearRFM() );
    printf( " - GetChNearRFT() : %d\n", GetChNearRFT( 190 ) );
    printf( " - GetChPARFT() : %d\n", GetChPARFT() );
    printf( " - GetChPA() : %d\n", GetChPA() );
    printf( " - GetChRFMUp() : %d\n", GetChRFMUp() );
    printf( " - GetChRFMDown() : %d\n", GetChRFMDown() );
}


//========================================================================
int cmd_rfstat    ( int argc, char * argv[] )
//========================================================================
{
    //	rfstat - RF Tx / Rx Status
    printf( "[RF Info] Tx : %d / Rx : %d\n", nTxPkt, nRxPkt );
}

#if defined(USE_TEST_RF_TX_LOOP)
//========================================================================
int cmd_rftx    ( int argc, char * argv[] )
//========================================================================
{
    //	RF Tx
    int 	nCh		=	0;

    switch ( argc )
    {
    case 2:		sscanf( argv[1], "%d", &nCh );	        //	ID
//	case 2:		sText = argv[1];						//	sscanf( argv[1], "%s", sText );		//	cmd [Text]
        break;
    }

    printf( "%s(%d) - CH(%d)\n", __func__, __LINE__, nCh );

    uint8_t sBuf[100];
    int nCnt = 0;

    while ( 1 )
    {
    	if ( ( nCnt % 300 ) == 0 )
    	{
    		printf(".");

			//	RFM(Board) Status LED
			HAL_GPIO_TogglePin ( LED_ST_GPIO_Port, LED_ST_Pin );
    	}

    	//*
    	SendPktCh(nCh, sBuf, 64);
    	HAL_Delay(3);

    	//	Watchdog Reload
    	__HAL_IWDG_RELOAD_COUNTER(&hiwdg);

    	/*/

    	SendPktCh(7, sBuf, 64);	//	Reserved

    	SendPktCh(ChRFT, sBuf, 64);

    	SendPktCh(ChTS2_1, sBuf, 64);	//	21

    	SendPktCh(nCh, sBuf, 64);

    	//*/

    	nCnt++;
    }
}
#endif

//========================================================================
int cmd_txpwr    ( int argc, char * argv[] )
//========================================================================
{
    //	RF Tx
    uint8_t		nPwr	=	0x7F;

    switch ( argc )
    {
    case 2:		sscanf( argv[1], "%d", &nPwr );	        //	ID
//	case 2:		sText = argv[1];						//	sscanf( argv[1], "%s", sText );		//	cmd [Text]
        break;
    }

    printf( "%s(%d) - Tx Pwr(%d)\n", __func__, __LINE__, nPwr );

    vRadio_Set_TxPower( nPwr );
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
    	return 0;
    }

    //	ID Flag Manual 설정.
    g_bSetRspIDManual = 1;

    if ( nSet )
    {
    	//	Set ID Flag
    	g_flagRspID |= ( 0x1 << nID );
		g_devStat[nID].nRSSI = 200;
    }
    else
    {
    	//	Clear ID Flag
    	g_flagRspID &= ~( 0x1 << nID );
		g_devStat[nID].nRSSI = 0;
    }

    printf( "%s(%d) - ID Flag : 0x%04X\n", __func__, __LINE__, g_flagRspID );

	return 1;
}


//========================================================================
int cmd_OccPa     ( int argc, char * argv[] )
//========================================================================
{
	//========================================================================
    //	occ [1(start)/0(stop)]	-	대승객방송

	printf("%s(%d)\n", __func__, __LINE__ );

	//	대승객방송 시작/종료
    int 		nOnOff 	= 0;

    switch ( argc )
    {
    case 2:		sscanf( argv[1], "%d", &nOnOff );	    //	On/Off
        break;
    }

    if ( nOnOff == 1 )
    {
    	printf("%s(%d) - Start\n", __func__, __LINE__ );

    	//	OCC Mode
    	SetRFMMode( RFMModeOcc );

#if USE_RFM_OCC_PA

#else
    	//	Audio Loopback On
		AudioDMALoopback();
#endif

		// 조명 On
		HAL_GPIO_WritePin ( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_SET );

		//	Spk On
		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );

		//  수신기 Spk Relay On
		HAL_GPIO_WritePin( AUDIO_ON_GPIO_Port, AUDIO_ON_Pin, GPIO_PIN_SET );
    }
    else
    {
    	printf("%s(%d) - Stop\n", __func__, __LINE__ );

    	//	Normal Mode
    	SetRFMMode( RFMModeNormal );

#if USE_RFM_OCC_PA

#else
    	//	Audio Loopback Off
    	AudioDMARFM();
#endif

    	//	Spk Off.
		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );

		//  수신기 Spk Relay Off
		HAL_GPIO_WritePin( AUDIO_ON_GPIO_Port, AUDIO_ON_Pin, GPIO_PIN_RESET );
    }
}


//========================================================================
//		RFM I2S Callback
//========================================================================


//========================================================================
//	interpolation compress ( 보간압축 )
#if defined( USE_AUDIO_INTERPOL_COMPRESS )	//	보간압축사용.

#define	FRAME_ENC_SIZE		(I2S_DMA_LOOP_SIZE * AUDIO_COMPR_RATE)

static int16_t	bufAudioEnc[FRAME_ENC_SIZE * 2];	//	Rx
static int16_t	bufAudioDec[FRAME_ENC_SIZE * 2];	//	Tx

#elif defined( USE_AUDIO_ADPCM )	//	ADPCM 사용. - 1/4 압축

#define	FRAME_ENC_SIZE		(I2S_DMA_LOOP_SIZE * 4)		//	1/4 압축

static int16_t	bufAudioEnc[FRAME_ENC_SIZE * 2];	//	Rx
static int16_t	bufAudioDec[FRAME_ENC_SIZE * 2];	//	Tx

signed short lowpass_Filter(signed short input)
{
	static signed short last_sample=0;
	signed short retvalue = ((input + (last_sample * 7)) >> 3);
	last_sample = retvalue;
	return retvalue;
}

#endif	//
//========================================================================

static int bRxBuffering = 1;	//  Rx Buffering. ( Packet 4 ~ Packet 0)

//========================================================================
void RFM_I2SEx_TxRxCpltCallback( I2S_HandleTypeDef *hi2s )
//========================================================================
{
	static int 	idx = 0;
	int16_t		*pAudioTx;
	int16_t		*pAudioRx;

	{
#if defined( USE_AUDIO_INTERPOL_COMPRESS )	|| defined( USE_AUDIO_ADPCM ) //	보간압축사용 or ADPCM 사용.

		pAudioTx = &bufAudioDec[FRAME_ENC_SIZE * idx];
		pAudioRx = &bufAudioEnc[FRAME_ENC_SIZE * (( idx + 1 ) % 2)];

		HAL_I2SEx_TransmitReceive_DMA( &hi2s3,
										pAudioTx,	//	Tx
										pAudioRx,	//	Rx
										FRAME_ENC_SIZE ); // 32byte

		pAudioRx = &bufAudioEnc[FRAME_ENC_SIZE * idx];
		idx = ( idx + 1 ) % 2;
		pAudioTx = &bufAudioDec[FRAME_ENC_SIZE * idx];

#else

		pAudioTx = &t_audio_buff[I2S_DMA_LOOP_SIZE * idx];
		pAudioRx = &r_audio_buff[I2S_DMA_LOOP_SIZE * (( idx + 1 ) % 2)];

		HAL_I2SEx_TransmitReceive_DMA( 	&hi2s3,
										pAudioTx,
										pAudioRx,
										I2S_DMA_LOOP_SIZE ); // 32byte

		pAudioRx = &r_audio_buff[I2S_DMA_LOOP_SIZE * idx];
		idx = ( idx + 1 ) % 2;
		pAudioTx = &t_audio_buff[I2S_DMA_LOOP_SIZE * idx];

#endif
	}

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

			qBufPut( &g_qBufAudioTx, (uint8_t *)r_audio_buff, ( I2S_DMA_LOOP_SIZE * 2 ) );

#elif defined( USE_AUDIO_ADPCM )	//	ADPCM 사용. - 1/4 압축

			//	ADPCM : 16 bit -> 4 bit
			int i;
			uint8_t *pABuf = (uint8_t *)r_audio_buff;
			for( i = 0; i < I2S_DMA_LOOP_SIZE*2; i++ )
			{
				//*
				pABuf[i] = (uint8_t)((uint8_t)((ADPCM_Encode(lowpass_Filter((int16_t)pAudioRx[i*2])<<1)&(0x0F))<<4)
								| (uint8_t)(ADPCM_Encode(lowpass_Filter((int16_t)pAudioRx[i*2+1])<<1)&0x0F));
				/*/
				pABuf[i] = (uint8_t)((uint8_t)((ADPCM_Encode((int16_t)pAudioRx[i*2])&(0x0F))<<4)
								| (uint8_t)(ADPCM_Encode((int16_t)pAudioRx[i*2+1])&0x0F));
				//	*/
			}

			qBufPut( &g_qBufAudioTx, (uint8_t *)r_audio_buff, ( I2S_DMA_LOOP_SIZE * 2 ) );

#else	//
			//========================================================================
			//	Queue Put
			qBufPut( &g_qBufAudioTx, (uint8_t *)pAudioRx, ( I2S_DMA_LOOP_SIZE * 2 ) );
#endif
		}

		/*

		//========================================================================
		//  Audio In -> Audio Out Loop
		memcpy( t_audio_buff, r_audio_buff, 64 );

		/*/

#if defined( USE_AUDIO_INTERPOL_COMPRESS ) || defined( USE_AUDIO_ADPCM )	//	보간압축사용 or ACPCM 사용
		memset( pAudioTx, 0, FRAME_ENC_SIZE * 2 );	//	Tx
#else
		memset( pAudioTx, 0, I2S_DMA_LOOP_SIZE * 2 );
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

				printf ( "F" );	 //  버퍼링종료 ( Buffering End - Buffer Full )
			}
		}

		if ( bRxBuffering == 0 )
		{
			//  Rx Audio Out
			if ( qBufCnt( &g_qBufAudioRx ) >= ( I2S_DMA_LOOP_SIZE * 2 ) )
			{
				//	printf ( "G" );
				//  Queue Audio Data

#if defined( USE_AUDIO_INTERPOL_COMPRESS )	//	보간압축사용.

				qBufGet( &g_qBufAudioRx, (uint8_t*)t_audio_buff, ( I2S_DMA_LOOP_SIZE * 2 ) );

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

#elif defined( USE_AUDIO_ADPCM )	//	ADPCM 사용. - 1/4 압축

				qBufGet( &g_qBufAudioRx, (uint8_t*)t_audio_buff, ( I2S_DMA_LOOP_SIZE * 2 ) );

				uint8_t *pABuf = (uint8_t *)t_audio_buff;
				//	Decoding : 4 bit -> 16 bit
				int i;
				for( i = 0; i < FRAME_ENC_SIZE/2; i++ )
				{
					pAudioTx[i*2] = (uint16_t)ADPCM_Decode((uint8_t)((pABuf[i]&(0xF0)))>>4);
					pAudioTx[i*2+1] = (uint16_t)ADPCM_Decode((uint8_t)(pABuf[i]&0x0F));
				}

#else	//

				qBufGet( &g_qBufAudioRx, (uint8_t*)pAudioTx, ( I2S_DMA_LOOP_SIZE * 2 ) );

#endif
			}
			else
			{
				printf ( "B" );	 //  버퍼링시작 ( Buffering )
				//  Data
				bRxBuffering = 1;

#if defined( USE_AUDIO_ADPCM )	//	ADPCM 사용. - 1/4 압축
				ADPCM_ClearDecodeBuf();
#endif

			}
		}
		//  */
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
//		HAL_I2SEx_TransmitReceive_DMA( &hi2s3, (uint16_t*)sine_table, (uint16_t*)bufAudio, 256 );

		//========================================================================
	}

	//========================================================================
	//	편성번호 Load
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
		//  Mic Vol Set
		//  Default (5) - 0 ~ 9

		int	 nMicVol;
		nMicVol = GetMicVol();

		if ( nMicVol < 0 || nMicVol > MaxMicVol )
		{
			nMicVol = DefaultMicVol;
			SetMicVol( nMicVol );
		}

		//========================================================================
#if defined(USE_AUDIO_INTERPOL_COMPRESS) || defined( USE_AUDIO_ADPCM )
		//	보간압축 or ADPCM 사용시 음량 Level 조절.
		WriteI2CCodec( 0x0B, 0x60 );	//  10 ( +18 dB )
#endif

#if defined(USE_HOP_MANUAL)
		//	g_nManHopping;		//	On(1) / Off(2) / Unused(0 : Other)
		g_nManHopping	=	2;	//	Hopping Diable
#endif	//	defined(USE_HOP_MANUAL)

	}
	else
	{
		//========================================================================
		//	OLED가 없으면 -> 수신기
		SetDevID( DevRF900M );	 //  수신기.

		//  수신기 스피커 레벨 변경.
		//	20.09.01 - 2호선 24칸차량설정.
		WriteI2CCodec( 0x09, 0x06 );	//  0x06 ( 0 )

#if defined(USE_HOP_MANUAL)
		//	g_nManHopping;		//	On(1) / Off(2) / Unused(0 : Other)
		g_nManHopping	=	GetManHop();	//	Hopping Diable

		printf("%s(%d) - Man Hopping ( %d )\n", __func__, __LINE__, g_nManHopping );
#endif	//	defined(USE_HOP_MANUAL)

	}
	//========================================================================

#if defined(USE_RF_COMM_MODE)
		//	g_nManHopping;		//	On(1) / Off(2) / Unused(0 : Other)
		g_nRFMode	=	GetRFMode();	//	Hopping Diable

		printf("%s(%d) - RFMode ( %d )\n", __func__, __LINE__, g_nRFMode );
#endif	//	defined(USE_HOP_MANUAL)

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

	//========================================================================
	//	Radio 초기화 이후 채널 설정해줌.
	{
		//========================================================================
		//  Radio Channel 설정.
		pRadioConfiguration->Radio_ChannelNumber = g_idxTrainSet;
		printf("%s(%d) - Radio Ch(%d) / g_idxTrainSet(%d)\n", __func__, __LINE__,
				pRadioConfiguration->Radio_ChannelNumber, g_idxTrainSet );
	}

	//========================================================================
	//	Random seed 설정.
	//	편성 & 호차 & TickCount로 설정.

	srand( HAL_GetTick() * 100 + GetTrainSetIdx() * 10 + GetCarNo() );

	//	Set Offset
	g_offsetCA = rand() % 1000;
	printf( "%s(%d) - offsetCA( %d )\n", __func__, __LINE__, g_offsetCA );

	//	초기 시작 Delay
	HAL_Delay(g_offsetCA);

	//========================================================================
}

//========================================================================
int RFM_main( void )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	InitRFM();

	//========================================================================
	//	Radio Spi
#if 0

	TestProcPkt();		//	RFM Main

#endif

#if defined(USE_TEST_RF_TX_LOOP)
	//========================================================================
	//cmd_rftx( int argc, char * argv[] )
//	ProcessCommand( "tx 0" );

	char *argv[] = {"tx", "0"};
	cmd_rftx( 2, argv );

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

		//	Loop Proc Main Watchdog Count Reload
		LoopProcMain( nTick );
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
				// PTT Key가 눌려있을시 전송.
				if( qBufCnt( &g_qBufAudioTx ) >= ( I2S_DMA_LOOP_SIZE * 2 ) )
				{
					//		printf ( "G" );
					qBufGet( &g_qBufAudioTx, (uint8_t*)bufRFTx.dat.data, ( I2S_DMA_LOOP_SIZE * 2 ) );

#if defined(USE_HOPPING)
					//========================================================================
					//	Packet Header
#if defined( USE_CH_ISO_DEV )
					//	Header #2
					if( GetKey(eKeyPtt) )
					{
						_MakePktHdr2( &bufRFTx, PktPA );
					}
					else
					{
						_MakePktHdr2( &bufRFTx, PktCall );
					}

#else
					if( GetKey(eKeyPtt) )
					{
						_MakePktHdr( &bufRFTx, GetDevID(), 0xFF, sizeof( RFMPktCtrlPACall ), PktPA );
					}
					else
					{
						_MakePktHdr( &bufRFTx, GetDevID(), 0xFF, sizeof( RFMPktCtrlPACall ), PktCall );
					}
#endif

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

#if defined( USE_CH_ISO_DEV )
					if( GetKey(eKeyPtt) )
					{
						//========================================================================
						//	PA

						if ( GetChPARFT() != 0 )
						{
							//	송신기에 전송.
							SendPktCh( GetChPARFT(), (uint8_t *)&bufRFTx,
								(U8)sizeof( RFMPktHdr ) + sizeof( RFMPktCtrlPACall ) );
						}

						SendPktCh( GetChPA(), (uint8_t *)&bufRFTx,
							pRadioConfiguration->Radio_PacketLength );
					}
					else
					{
						//========================================================================
						//	Call
						if ( GetChPARFT() != 0 )
						{
							//	송신기에 직접 전송.
							SendPktCh( GetChPARFT(), (uint8_t *)&bufRFTx,
								(U8)sizeof( RFMPktHdr ) + sizeof( RFMPktCtrlPACall ) );
						}
						else
						{
							//	수신기를 통해 전송.
							SendPktCh( GetChPA(), (uint8_t *)&bufRFTx,
								pRadioConfiguration->Radio_PacketLength );
						}
					}

#else
					SendPacket( (uint8_t *)&bufRFTx,
						pRadioConfiguration->Radio_PacketLength );
#endif
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
				if ( IsMenuMaint() )
				{
					//	Maint Mode : 송신채널 표시.
					char sBuf[20];
					sprintf( sBuf, "수신중(%d/%d)", GetCh2Car(GetChNearRFM()), GetChNearRFM() );	//	Channel -> Car
					LCDPrintf( sBuf );
				}
				else
				{
					LCDPrintf( "수신중..." );
				}
				break;

			case RFMModeNormal:
				//========================================================================
				//  편성 : XXX
				UpdateLCDMain();
				//	송신기 Speaker Mute
#if defined(USE_RFT_ONLY_RX_SPK_ON)
				if ( GetDevID() == DevRF900T )
				{
					//  송신기 : 수신중이 아닌경우 SPK OFF
					RFM_Spk(0);

					//  Red LED Off
					HAL_GPIO_WritePin ( LED_ON_B_GPIO_Port, LED_ON_B_Pin, GPIO_PIN_RESET ); //  RED LED
				}
#endif
				break;
			}

			nOldRFMMode = nRFMMode;
		}

		//========================================================================
		//	상태정보 모니터링중 - 상태정보값 갱신.
		if( GetRFMMode() == RFMModeNormal )
		{
			UpdateLCDMonitor( nTick );		//	LCD : 모니터링 상태 Update
		}
	}
	//========================================================================
	//  RFM - 수신기 - 대승객방송
	else if ( GetDevID() == DevRF900M )
	{
		if( GetRFMMode() == RFMModeOcc )
		{
			//	대승객 방송 모드인 경우 대승객 방송 음성 송출.
			if( qBufCnt( &g_qBufAudioTx ) >= ( I2S_DMA_LOOP_SIZE * 2 ) )
			{
				//		printf ( "G" );
				//========================================================================
				//	Audio ( 대승객방송 음성 )
				qBufGet( &g_qBufAudioTx, (uint8_t*)bufRFTx.dat.data, ( I2S_DMA_LOOP_SIZE * 2 ) );
				//	Audio Loopback ( 대승객방송 음성 )
				qBufPut( &g_qBufAudioRx, (uint8_t*)bufRFTx.dat.data, ( I2S_DMA_LOOP_SIZE * 2 ) );

				//========================================================================
				//	Packet Header	-	OCC PA 전송

				//	Header #2
				_MakePktHdr2( &bufRFTx, PktPA );

				if( GetChRx() == ChTS1_1 )	//	1호차 수신기
				{
					//	1 -> 2 ... -> 10
					SendPktCh( GetChPA() + 1, (uint8_t *)&bufRFTx,
						pRadioConfiguration->Radio_PacketLength );
				}
				else
				{
					// 10 -> 9 ... -> 1
					SendPktCh( GetChPA() - 1, (uint8_t *)&bufRFTx,
						pRadioConfiguration->Radio_PacketLength );
				}

				// 조명 On
				HAL_GPIO_WritePin ( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_SET );
				//========================================================================
			}
		}
	}

	//========================================================================
	//	수신중 해제
	if ( (nTick - g_nStampRxPkt) > TIMEOUT_RXSTAT && GetRFMMode() == RFMModeRx
			&& ( GetRFMMode() != RFMModeOcc )		//	OCC Mode - Skip
			&& ( GetRFMMode() != RFMModeUpgr )		//	Upgrade Mode - Skip
		)
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

	//========================================================================
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

			//	RFM(Board) Status LED
			HAL_GPIO_TogglePin ( LED_ST_GPIO_Port, LED_ST_Pin );
		}

		if ( GetRFMMode() == RFMModeNormal )
		{
			//	Normal 모드인 경우 상태정보 갱신.
		    //	Reflash Status
		    ReflashStat( nTick );	//	상태정보 갱신.
		}

#if defined(USE_SEND_STATUS)	//	상태 정보 전송.
		//========================================================================
		if ( GetRFMMode() == RFMModeNormal )
		{
		    SendStat(0);		//	상태정보전송.

		    //	Reflash Status
		    ReflashStat( nTick );	//	상태정보 갱신.
		}
		//========================================================================
#endif
		s_nTickStandby = nTick;
	}

#if defined(TIMEOUT_UPGRADE)

	//========================================================================
	//	Upgrade Mode 진입후 5분간 Rx가 없는경우 Normal모드로 변경.
	if ( ( GetDevID() == DevRF900M )						//	수신기.
		&& ( ( nTick - g_nStampRxPkt ) > (TIMEOUT_UPGRADE * 1000) )	//	Timeout
		&& ( GetRFMMode() == RFMModeUpgr )
		)
	{
		//========================================================================
		//	Normal 모드로 변경.
		SetRFMMode( RFMModeNormal );	//	Normal Mode 로 변경
		//========================================================================

		//  RF 수신 Start
		g_nChRx = GetChRx();	//	ChTS1_1 + g_idxTrainSet * 2 + ((g_nCarNo+1) % 2);	// 현재 호차 채널

		vRadio_StartRX(
			g_nChRx,	//g_idxTrainSet,	//		pRadioConfiguration->Radio_ChannelNumber,
			pRadioConfiguration->Radio_PacketLength );
	}

#endif

#if defined(USE_STAT_REQ)	//	송신기 : 상태 정보 요청 100 msec간격.
	//========================================================================
	//	송신기 장치 상태정보 요청.
	static int oldTickStatReq = 0;
	static int s_idxCh = 0;

	if	( 	GetDevID() == DevRF900T						//	송신기
			&&	GetRFMMode() == RFMModeNormal			//	Normal모드 : 상태정보 요청.
			)
	{
#if defined(USE_ROUTE_REQ_RFM)//	수신기에서 요청의 경우 송신기가 요청하지 않음.
#else
#if defined(USE_ROUTE_REQ)	//	송신기 : Route 정보 요청.
		static int oldTickRouteReq = 0;

		if( (nTick - oldTickRouteReq ) > (TIME_ROUTE_REQ * 1000) )
		{
			//	송신기 -> 수신기 : Route 정보 요청.
			SendRouteReq( GetChNearRFM() );		//	가까운 수신기로 Route정보 요청.

			oldTickRouteReq = nTick + 1000;		//	1초 이후 부터 시작.
			oldTickStatReq = nTick + 1000;		//	1초 이후 부터 시작.
		}
		else
#endif		//	defined(USE_ROUTE_REQ)	//	송신기 : Route 정보 요청.
#endif
		if	( (nTick - oldTickStatReq) > TIME_STAT_REQ )	//	주기 : 300 msec
		{
			//	상태정보 요청.
			if ( s_idxCh < MaxRFMNo )
			{
				//========================================================================
				//	수신기 상태정보
				SendStatReq( ChTS1_1 + (s_idxCh*ChGap) );
			}
			else
			{
				//========================================================================
				//	송신기 상태정보
				if( ChTx_1 + ( s_idxCh % 2 )*ChGap != GetChRx() )
				{
					//	타 송신기에 상태정보 요청.
					SendStatReq( ChTx_1 + ( s_idxCh % 2 )*ChGap );
				}
			}

	//		if ( s_idxCh == 0 )
	//		{
	//		    //	Reflash Status
	//		    ReflashStat( nTick );	//	상태정보 갱신.
	//		}

			s_idxCh = ( s_idxCh + 1 ) % ( MaxRFMNo + 2 );	//	MaxRFMNo : 10 + 2(송신기 2채널)

			oldTickStatReq = nTick;
		}
	}
#endif


#if defined(USE_ROUTE_REQ_RFM)	//	수신기 Route 요청.
	static int oldTickRouteReq = 0;

	static int s_ChkRsp = 0;

	if	( 	GetDevID() == DevRF900M						//	수신기
			&&	GetRFMMode() == RFMModeNormal			//	Normal모드 : 상태정보 요청.
			)
	{

		if( (nTick - oldTickRouteReq ) > (TIME_ROUTE_REQ * 1000) )
		{
			//	수신기 -> 수신기 : Route 정보 요청.
			//	1 -> 2
			//		 2 -> 3
			//			  3 -> 4
			if ( g_nCarNo != 10 )	//	10호차 Skip
			{
				//	다음번 수신기에 정보 요청
				SendRouteReq( GetChRx() + ChGap );		//

				//	Timeout 발생시 그 다음 수신기로 정보 요청.
			}

			oldTickRouteReq = nTick;			//	1초 이후 부터 시작.
			s_ChkRsp = 1;
		}

		if( s_ChkRsp == 1
				&& (nTick - oldTickRouteReq ) > 100		//	송신후 응답시간 ( 100 msec )
				&& ( g_nCarNo != 9 && g_nCarNo != 10 )	//	다음호차 검색은 9/10호차 Skip
				)
		{
			//	다음 호차검색

			if ( ( nTick - g_nStampRouteRsp ) > TIMEOUT_RECV_ROUTE * 1000 )
			{
				//	Timeout 발생시.
				//	다음호차부터  ~ 10까지 상태정보 전송 검색.

				//	g_nIdxRouteFindNext	Index 증가하면서 10호차 까지 상태정보 검색.
				//	[1] [2-X] ->[3] ->[4] ... -> [10]
				SendRouteReq( GetChRx() + (2 + g_nIdxRouteFindNext) * ChGap );

				if ( ( g_nCarNo + 2 + g_nIdxRouteFindNext ) > 10 )
					g_nIdxRouteFindNext = 0;
				else
					g_nIdxRouteFindNext++;
			}
			s_ChkRsp = 0;
		}
	}

#endif

}

//========================================================================
void UpdateStat( RFMPktStat *pStat )
//========================================================================
{
	//	버전정보 갱신.
	if ( pStat == NULL ) return ;

	if ( 0 < pStat->nCarNo && pStat->nCarNo <= MaxCarNo )
	{
		int idx = pStat->nCarNo;

		//========================================================================
		//	상태정보 갱신.
		memcpy( &g_devStat[idx].stat, pStat, sizeof(RFMPktStat) );

		//========================================================================
		//	RSSI 갱신
		g_devStat[idx].nRSSI = g_nRSSI;

		//========================================================================
		//	버전정보 갱신.

#if defined( USE_COMM_MODE_CH_GRP )
		sprintf(_sVerList[idx], "%02d:%d/R%d,%03d", idx,
				pStat->ver_build,
				pStat->nRFMode,
				g_devStat[idx].nRSSI
				);
#elif defined( USE_HOP_MANUAL )
		sprintf(_sVerList[idx], "%02d:v%d/hop(%d)", idx,
				pStat->ver_build,
				pStat->nManHop
				);
#else
		sprintf(_sVerList[idx], "%02d:v%d.%d.%d.%d", idx,
				pStat->ver_main,
				pStat->ver_sub,
				pStat->ver_det,
				pStat->ver_build
				);
#endif
	}
}

//========================================================================
void SetStat( int nRspID )
//========================================================================
{
	//	상태정보 저장.
	//	상태정보 수신 시간 저장.

	//	장치 응답 Flag 설정.
	g_flagRspID	|= ( 0x1 << nRspID );

	//	TimeStamp 저장.
//DEL	stampStat[nRspID] = HAL_GetTick();
	g_devStat[nRspID].stampRx = HAL_GetTick();
}

//========================================================================
void ReflashStat( int nTick )
//========================================================================
{
	//	상태정보 갱신.
	//	Timeout 초과 상태정보 Disable
	if( g_bSetRspIDManual )
	{
		//	ID 수동설정모드인 경우 return
		return ;
	}

	int idx;

	for( idx = 0; idx < MaxCarNo; idx++ )
	{
		if ( GetCarNo() == idx )
		{
			//	자신의 ID Skip
			continue;
		}

		if ( ( nTick - g_devStat[idx].stampRx ) > TIMEOUT_RECV_STATUS * 1000 )
		{
			g_flagRspID &= ~( 0x1 << idx );

			//========================================================================
			//	Timeout 발생시 RSSI값 초기화.
			g_devStat[idx].nRSSI 	= 	0;
			g_devStat[idx].nNearCh 	= 	0;	//	nNearCh 초기화.
			//========================================================================
		}
	}
}


//========================================================================
void ReloadStampStat( void )
//========================================================================
{
	//	상태정보 시간 갱신.
	//	-> 상태정보 시간 최신값으로 유지.
	//	-> 방송/통화 중 상태정보 전송을 하지 않기 때문에 방송통화 이후에 상태정보가 Reset되는 현상 방지.

	int nStamp = HAL_GetTick();

	int idx;

	for( idx = 0; idx < 16; idx++ )
	{
		if( g_flagRspID & ( 0x1 << idx ) )
		{
			g_devStat[idx].stampRx = nStamp;
		}
	}

	g_nStampRouteRsp = HAL_GetTick();		//	Normal 모드로 전환시 응답 시간 Reset
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



