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


//========================================================================
// Function


//========================================================================
int UpgrSendImage		( uint32_t nAddrBase, uint32_t nSizeImage )
//========================================================================
{
	//	송신기 Upgrade 명령. - Bootloader
	printf( "%s(%d) - baseAddr(0x%08X) / size(%d)\n", __func__, __LINE__, nAddrBase, nSizeImage );

	uint32_t	nAddrTarget;

	uint8_t		sBuf[100];
	int			nSize;

	//========================================================================
	//	0x0800 0000 ~ 0x0807 FFFF	:	Bootloader + Application
	//========================================================================
	//	0x0808 0000 ~ 0x080F FFFF	:	Upgrade Image
	//========================================================================
	nAddrTarget		=	nAddrBase + SIZE_FLASH_BOOTAPP;

	int i;
	int nTotPkt = ( ( nSizeImage + 49 ) / 50 );

	char	sLCD[100];

	for ( i = 0; i < nTotPkt; i++ )
	{
		//	Flash Data 전송.
		memcpy( sBuf, *(__IO uint8_t*)(nAddrBase + (i * 50)), 50 );
		SendUpgrData( nAddrTarget, nTotPkt, i, sBuf, 50 );

		//	수신기 Message 표시.
		//	159,596 Byte = 3191 Pkt * 50 Byte
		//	Upgr:[   1 / 3191 ]
		sprintf( sLCD, "Upgr:[%4d/%4d]", i, nTotPkt );
		LCDSetCursor( 1, 13 );
		LCDPrintf( sLCD );

		HAL_Delay( 4 );	//	4 msec Delay

		//========================================================================
		//	Watchdog Reload
		if ( ( i % 100 ) == 0 )
		{
			__HAL_IWDG_RELOAD_COUNTER(&hiwdg);
		}
		//========================================================================
	}
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
	//	upgrade 		//

	printf( "%s(%d)\n", __func__, __LINE__ );

	//========================================================================
	//	채널변경. => CH #1
//	ProcessCommand("ch 1");
	g_idxTrainSet = 1;
	//	수신기
	vRadio_StartRX (
		g_idxTrainSet,	//	pRadioConfiguration->Radio_ChannelNumber,
		pRadioConfiguration->Radio_PacketLength );

	//========================================================================
	SetRFMMode( RFMModeRx );	//	수신모드로 설정. ( 상태정보 전송 X )
	//========================================================================

	//========================================================================

//    switch ( argc )
//    {
//    case 3:		sscanf( argv[2], "%d", &nVal );			//	cmd [address] [value]
//    case 2:		sscanf( argv[1], "%d", &nAddr );		//	cmd [address]
//        break;
//    }
}

//========================================================================

