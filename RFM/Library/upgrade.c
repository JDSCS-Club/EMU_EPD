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

#include "upgrade.h"

#include "rfm.h"				//	g_idxTrainSet

#include "radio.h"				//	pRadioConfiguration

#include "flash_if.h"			//	ADDR_FLASH_BOOT

#include "main.h"				//	hiwdg

//========================================================================
// Define

static int s_bUpgrRetry = 0;		//	Upgrade Retry

//========================================================================
// Function

//========================================================================
void SetUpgrReTry( int _bUpgrRetry )
//========================================================================
{
	s_bUpgrRetry = _bUpgrRetry;
}

//========================================================================
int GetUpgrReTry( void )
//========================================================================
{
	return s_bUpgrRetry;
}


//========================================================================
int UpgrSendImage		( uint32_t nAddrBase, uint32_t nSizeImage )
//========================================================================
{
	//	송신기 Upgrade 명령.
	printf( "%s(%d) - baseAddr(0x%08X) / size(%d)\n", __func__, __LINE__, nAddrBase, nSizeImage );

	uint32_t	nAddrTarget;

	uint8_t		sBuf[100];
	int			nSize;

	//========================================================================
	//	0x0800 0000 ~ 0x0807 FFFF	:	Bootloader + Application
	//========================================================================
	//	0x0808 0000 ~ 0x080F FFFF	:	Upgrade Image
	//========================================================================
	nAddrTarget		=	nAddrBase + 0x80000;// + SIZE_FLASH_BOOTAPP;

	int i, j;
//	int nTotPkt = ( ( nSizeImage + 49 ) / 50 );
	int nTotPkt = ( ( nSizeImage + (PktUpgrDataSize - 1) ) / PktUpgrDataSize );

	char	sLCD[100];

	LCDEnableDebug( 0 );	//	LCD - console printf Off

	for ( i = 0; i < nTotPkt; i++ )
	{
		//	Flash Data 전송.
//		memcpy( sBuf, (__IO uint8_t*)(nAddrBase + (i * 50)), 50 );
//		SendUpgrData( nAddrTarget, nTotPkt, i, sBuf, 50 );
		memcpy( sBuf, (__IO uint8_t*)(nAddrBase + (i * PktUpgrDataSize)), PktUpgrDataSize );
		SendUpgrData( nAddrTarget, nTotPkt, i, sBuf, PktUpgrDataSize );

		//========================================================================
		if ( i == 0 && GetUpgrReTry() == 0 )
		{
			//	첫번째 패킷 전송후  Flash Erase Delay : 3 sec
			__HAL_IWDG_RELOAD_COUNTER(&hiwdg);
			sprintf( sLCD, "Upgr:FL Erase", i, nTotPkt );
			LCDSetCursor( 1, 13 );
			LCDPrintf( sLCD );
//			HAL_Delay( 3000 );		//	sleep 3 sec
			for( j = 0; j < 7; j++ )	//	sleep 7 sec
			{
				HAL_Delay( 1000 );
				__HAL_IWDG_RELOAD_COUNTER(&hiwdg);	//	Watchdog Re-Flash
			}
		}
		//========================================================================

		//	수신기 Message 표시.
		//	159,596 Byte = 3191 Pkt * 50 Byte
		//	Upgr:[   1 / 3191 ]
		if ( ( i % 10 ) == 0 )
		{
			sprintf( sLCD, "Upgr:[%4d/%4d]", i, nTotPkt );
			LCDSetCursor( 1, 13 );
			LCDPrintf( sLCD );
		}

//		HAL_Delay( 4 );	//	4 msec Delay
//		HAL_Delay( 3 );	//	2 msec Delay
		HAL_Delay( 2 );	//	2 msec Delay

		//========================================================================
		//	Watchdog Reload
		if ( ( i % 50 ) == 0 )
		{
			__HAL_IWDG_RELOAD_COUNTER(&hiwdg);
		}
		//========================================================================
	}

	sprintf( sLCD, "Upgr:[%4d/%4d]", i, nTotPkt );
	LCDSetCursor( 1, 13 );
	LCDPrintf( sLCD );
}


//========================================================================
int UpgrSendImageBoot	( void )
//========================================================================
{
	//	송신기 Upgrade 명령. - Bootloader
	UpgrSendImage( ADDR_FLASH_BOOT, FLASH_If_GetBootSize() );
}

//========================================================================
int UpgrSendImageApp	( void )
//========================================================================
{
	//	송신기 Upgrade 명령. - Application
	UpgrSendImage( ADDR_FLASH_APP, FLASH_If_GetAppSize() );
}

//========================================================================
int cmd_upgrade	( int argc, char * argv[] )
//========================================================================
{
	//========================================================================
	//	Upgrade 수신측에서 동작.
	//========================================================================
	//	upgrade ch 1	//	Upgrade 채널 변경.
	//	upgrade [1/0]	//	1(Start) / 0(End)

	int nVal = 1;

	printf( "%s(%d)\n", __func__, __LINE__ );

	//========================================================================
	switch ( argc )
	{
	case 2:		sscanf( argv[2], "%d", &nVal );			//	cmd [address] [value]
//    case 2:		sscanf( argv[1], "%d", &nAddr );		//	cmd [address]
		break;
	}

	if ( nVal == 1 )
	{
		//========================================================================
		//	Upgrade Start

		//========================================================================
		//	채널변경. => CH #1
		g_nChRx = ChUpgrade;		//	Upgrade Channel

		vRadio_StartRX (
			g_nChRx,	//g_idxTrainSet,	//	pRadioConfiguration->Radio_ChannelNumber,
			pRadioConfiguration->Radio_PacketLength );

		//========================================================================
		//	Log 출력 Off

		//========================================================================
		//	Upgrade Mode
		SetRFMMode( RFMModeUpgr );		//	Upgrade Mode 설정. ( 상태정보 전송 X )
		//========================================================================
	}
	else if ( nVal == 0 )
	{
		//========================================================================
		//	Upgrade Mode
		SetRFMMode( RFMModeNormal );	//	Normal Mode 로 변경
		//========================================================================
	}
}
//========================================================================

