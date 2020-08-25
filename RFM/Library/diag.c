/*------------------------------------------------------------------------------------------
	Project			: RFM
	Description		: 보드 시험용 함수 모음들

	Writer			: $Author: zlkit $
	Revision		: $Rev: 1891 $
	Date			: 2020. 08.
	Copyright		: Piranti Corp. ( zlkit@piranti.co.kr )
	 
	Revision History 
	1. 2020. 08.	: Created
-------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

//=============================================================================
#if defined(_WIN32)
//=============================================================================

#include "x86_hal_driver.h"

//=============================================================================
#else	//	stm32f207
//=============================================================================

#if defined(STM32F407xx)
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#elif defined(STM32F207xx)
#include "stm32f2xx.h"
#include "stm32f2xx_hal.h"
#endif

#if defined(USE_FREERTOS)
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#endif

#include "main.h"				//	GPIO Pin

#include "typedef.h"			//	ip_addr

#include "serial.h"

#include "radio_si4463.h"		//	RF-IC

#include "radio.h"				//	RF-IC WDS

#include "si446x_api_lib.h"		//	Pro2Cmd

#include "radio_comm.h"			//	g_bEnableDbgRadioComm

//=============================================================================
#endif
//=============================================================================

#include "audio.h"		//	Audio Diag
#include "cli.h"
#include "diag.h"

#define		CLEAR_VT_SCREEN()	printf("\033[2J")
#define		GOTO_VT_XY()		printf("\033[1;1H")
#define		BUFSIZE	512

#if	defined(USE_BOOTLOADER)
uint8_t	dbuf[MAX_Y][MAX_X_BYTE];		//	bootloader에서는 dbuf를 사용하지 않음.
#endif

unsigned char *	mem_addr;
int				dumpsize = 0x100;	//, data_option = 2;
int				debug_level = 0;

extern char	sLineInput[BUFSIZE];		// Prompt Line Input

/*
 * diag main menu를 display
 */
//========================================================================
void display_diag_menu( void )
//========================================================================
{
	CLEAR_VT_SCREEN();
	GOTO_VT_XY();

	printf( "\n" );
	printf( "\n" );
	printf( "\n" );
	printf( "\n" );

	printf( "       **************************************************************\n" );
	printf( "       *                       Board Test V1.0                      *\n" );
	printf( "       *                                                            *\n" );
	printf( "       *  1 : Audio Test.                                           *\n" );
	printf( "       *  2 : RF test.                                              *\n" );
	printf( "       **************************************************************\n" );
	printf( "       *  4 : spi test.                                             *\n" );
	printf( "       *  5 : External Flash Memory Test.(Write/Read/Compare)       *\n" );
	printf( "       **************************************************************\n" );
	printf( "       *  e : print log.                                            *\n" );
	printf( "       **************************************************************\n" );
	printf( "\n" );
	printf( "           Please select number[to quit, push 'q', 'Q']:" );
}

//========================================================================
int cmd_diag_proc( char *cmd )
//========================================================================
{
//	sel = ibuf[0];
	char sel = cmd[0];

	switch ( sel )
	{
	case '1':	cmd_diag_Audio( 0, 0 );		break;
	case '2':	cmd_diag_RF( 0, 0 );		break;
		//		case '2':	cmd_test2( 0, 0 );			break;
		//		case '3':	cmd_test3( 0, 0 );			break;
		//		case '4':	cmd_test4( 0, 0 );			break;
		//		case '5':	cmd_test5( 0, 0 );			break;
	case 'q':
	case 'Q':
#if defined(USE_FREERTOS)
#else
		SetCLIPrompt( CLIPrompt );
		SetCLIProc( ProcessCommand );
#endif
		return 0;
	default:
		break;
	}

	return 1;
}

//========================================================================
//					diag main command function
//========================================================================
int cmd_diag( int argc, char *argv[] )
//========================================================================
{
#if defined(USE_FREERTOS)

	char	ibuf[80];
	int		v, sel;

	while ( 1 )
	{
		display_diag_menu();

		v = readline( ibuf );

		if ( v <= 0 )
		{
			continue;
		}

		if ( cmd_diag_proc( ibuf ) == 0 )
		{
			return 0;
		}

		printf( "Press ENTER key to continue. " );
		readline( ibuf );

		taskYIELD();
	}
#else
//	HAL_Delay( 0 );
	SetCLIPrompt( display_diag_menu );
	SetCLIProc( cmd_diag_proc );

#endif

	/* NOTREACHED */

	return 0;
}


//========================================================================
void display_diag_Audio( void )
//========================================================================
{
	CLEAR_VT_SCREEN();
	GOTO_VT_XY();

	printf( "\n" );
	printf( "\n" );
	printf( "\n" );
	printf( "\n" );

	printf( "       **************************************************************\n" );
	printf( "       *                      Audio Test.                           *\n" );
	printf( "       *                                                            *\n" );
	printf( "       *  1 : audio tx sine.                                        *\n" );
	printf( "       *  2 : audio tx null.                                        *\n" );
	printf( "       *  3 : audio tx stop.                                        *\n" );
	printf( "       *  4 : Audio Loopback. ( Internal Codec )                    *\n" );
	printf( "       *  5 : Audio Loopback. ( DMA )                               *\n" );
	printf( "       *  6 : Audio RF Tx/Rx. ( DMA )                               *\n" );
	printf( "       *  7 : Audio Loopback. ( DMA - speex )                       *\n" );
	printf( "       *  8 : Audio RF Tx. ( DMA )                                  *\n" );
	printf( "       *  9 : Audio RF Rx. ( DMA )                                  *\n" );
	printf( "       *  a : Audio Sampling ( 8 KHz )                              *\n" );
	printf( "       *  b : Audio Sampling ( 4 KHz )                              *\n" );
	printf( "       *  c : Audio Sampling ( 2 KHz )                              *\n" );
	printf( "       *  d : Audio Sampling ( 1 KHz )                              *\n" );
	printf( "       *  i : audio Init                                            *\n" );
	printf( "       *  e : Enable Debug SPI                                      *\n" );
	printf( "       **************************************************************\n" );
	printf( "\n" );
	printf( "           Please select number[to quit, push 'q', 'Q']:" );
}


//========================================================================
int cmd_diag_proc_Audio( char *cmd )
//========================================================================
{
//	sel = ibuf[0];
	char sel = cmd[0];

	switch ( sel )
	{
#if 0
		case '1':	AudioSine();							break;
		case '2':	AudioTxNull();							break;
		case '3':	AudioTxStop();							break;
		case '4':	AudioLoopback();						break;
		case '5':	AudioLoopbackDMA();						break;
		case '6':	AudioRFRxTx();							break;
		case '7':	AudioLoopbackDMASpeex();				break;
		case '8':	AudioRFTx();							break;
		case '9':	AudioRFRx();							break;
		case 'a':	SetAudioLoopSampling( eASample8KHz );	break;
		case 'b':	SetAudioLoopSampling( eASample4KHz );	break;
		case 'c':	SetAudioLoopSampling( eASample2KHz );	break;
		case 'd':	SetAudioLoopSampling( eASample1KHz );	break;
		case 'i':	InitCodecXE3005();							break;
		case 'e':	AudioDebugEnable();						break;
#endif
	case 'q':
	case 'Q':
#if defined(USE_FREERTOS)
#else
		cmd_diag(0, 0);
#endif
		return 0;
	default:
		break;
	}

	return 1;
}

//========================================================================
int cmd_diag_Audio( int argc, char *argv[] )
//========================================================================
{
#if defined(USE_FREERTOS)

	char	ibuf[80];
	int		v, sel;

	while ( 1 )
	{
		display_diag_Audio();

		memset( ibuf, 0, sizeof( ibuf ) );
		v = readline( ibuf );

		if ( v <= 0 )
		{
			continue;
		}

		if ( cmd_diag_proc_Audio( ibuf ) == 0 )
		{
			break;
		}

		printf( "Press ENTER key to continue. " );
		readline( ibuf );

		taskYIELD();
	}

#else
	//	HAL_Delay( 0 );
	SetCLIPrompt( display_diag_Audio );
	SetCLIProc( cmd_diag_proc_Audio );

#endif

	return 0;
}



//========================================================================
void display_diag_RF( void )
//========================================================================
{
	CLEAR_VT_SCREEN();
	GOTO_VT_XY();

	printf( "\n" );
	printf( "\n" );
	printf( "\n" );
	printf( "\n" );

	printf( "       **************************************************************\n" );
	printf( "       *                         RF Test.                           *\n" );
	printf( "       *                                                            *\n" );
	printf( "       *  0 : RF-IC Info                                            *\n" );
	printf( "       *  1 : RF Tx.                                                *\n" );
	printf( "       *  2 : RF Rx.                                                *\n" );
	printf( "       *  3 : RF Tx->Rx Loopback                                    *\n" );
	printf( "       *  i : RF-Init                                               *\n" );
	printf( "       *  v : Verify RF-Init                                        *\n" );
	printf( "       *  d : Enable Debug SPI                                      *\n" );
	printf( "       **************************************************************\n" );
	printf( "\n" );
	printf( "           Please select number[to quit, push 'q', 'Q']:" );
}


//========================================================================
int cmd_diag_proc_RF( char *cmd )
//========================================================================
{
//	sel = ibuf[0];
	char sel = cmd[0];

	switch ( sel )
	{
#if defined(_WIN32)
#else
		case '0':	RF_Info();					break;
		case '1':	cmd_test_rf1( 0, 0 );		break;
		case '2':	cmd_test_rf2( 0, 0 );		break;
		case '3':	cmd_test_rf3( 0, 0 );		break;
		case 'i':	cmd_test_rf_init( 0, 0 );	break;
		case 'v':	cmd_test_rf_vinit( 0, 0 );	break;
		case 'd':	cmd_test_rf_debug( 0, 0 );	break;
#endif
	case 'q':
	case 'Q':
#if defined(USE_FREERTOS)
#else
		cmd_diag(0, 0);
#endif
		return 0;
	default:
		break;
	}

	return 1;
}

//========================================================================
int cmd_diag_RF( int argc, char *argv[] )
//========================================================================
{
#if defined(USE_FREERTOS)

	char	ibuf[80];
	int		v, sel;

	while ( 1 )
	{
		display_diag_RF();

		v = readline( ibuf );

		if ( v <= 0 )
		{
			continue;
		}

		if ( cmd_diag_proc_Audio( ibuf ) == 0 )
		{
			break;
		}

		printf( "Press ENTER key to continue. " );
		readline( ibuf );

		taskYIELD();
	}

#else
	//	HAL_Delay( 0 );
	SetCLIPrompt( display_diag_RF );
	SetCLIProc( cmd_diag_proc_RF );

#endif

	/* NOTREACHED */

	return 0;
}


//========================================================================
int	cmd_test1(int argc, char *argv[])
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

#if defined(_WIN32)
#else

	AudioLoopback();		//	Audio Loopback Test

#endif

	return 0;
}

//========================================================================
int	cmd_test2(int argc, char *argv[])
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	printf( "spi test\n" );

#if defined(_WIN32)
#else

	spi_test();

#endif

	return 0;
}

//========================================================================
int	cmd_test3( int argc, char *argv[] )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

#if defined(_WIN32)
#else

	InitCodecXE3005();	//	Audio Codec Init

#endif

	return 0;
}

//========================================================================
int	cmd_test4( int argc, char *argv[] )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );
	printf( "audio tx sine.\n" );

#if defined(_WIN32)
#else

	AudioSine();

#endif

	return 0;
}

//========================================================================
int	cmd_test5( int argc, char *argv[] )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );
	printf( "audio tx null.\n" );

#if defined(_WIN32)
#else

//	AudioTxNull();
	AudioTxStop();

#endif

	return 0;
}


//========================================================================
int	cmd_test_rf1( int argc, char *argv[] )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	printf( "*  1 : RF Tx test.*\n" );

#if defined(_WIN32)
#else

	//	Select Tx Mode
	RF_Tx();

	SI4463_Test();		//	RF-Test

#endif

	return 0;
}


//========================================================================
int	cmd_test_rf2( int argc, char *argv[] )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	printf( "*  1 : RF Rx test.*\n" );

#if defined(_WIN32)
#else

	//	Select Rx Mode
	RF_Rx();

	SI4463_Test();		//	RF-Test

#endif

	return 0;
}

//========================================================================
int	cmd_test_rf3( int argc, char *argv[] )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	printf( "*  1 : RF Loopback ( Tx -> Rx ) test.*\n" );

#if defined(_WIN32)
#else

	//	Select Rx Mode
	RF_Loopback();

	SI4463_Test();		//	RF-Test

#endif

	return 0;
}

//========================================================================
int	cmd_test_rf_init( int argc, char *argv[] )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

//	int8_t ret;
//	ret = SI4463_Init( &si4463 );

#if defined(_WIN32)
#else

	RF_Init();

#endif

//	printf( "%s(%d) - ret(%d)\n", __func__, __LINE__, ret );

	return 0;
}

//========================================================================
int	cmd_test_rf_vinit( int argc, char *argv[] )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

#if defined(_WIN32)
#else

	int8_t ret;
	ret = SI4463_VerifyInit( &si4463 );

	printf( "%s(%d) - ret(%d)\n", __func__, __LINE__, ret );

#endif

	return 0;
}

//========================================================================
int	cmd_test_rf_debug( int argc, char *argv[] )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

#if defined(_WIN32)
#else

	static int s_bEnableDebug = 0;

	if ( s_bEnableDebug )
	{
		s_bEnableDebug = 0;
		printf( "Disable Debug\n" );

		SI4463_Debug( 0 );
	}
	else
	{
		s_bEnableDebug = 1;
		printf( "Enable Debug\n" );

		SI4463_Debug( 1 );
	}
	
#endif

	return 0;
}

//========================================================================
int		cmd_rfwds_info	( int argc, char *argv[] )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

#if defined(_WIN32)
#else

	uint8_t i;

	si446x_part_info();

	printf( "[" );
	for ( i = 0; i < 6; i++ )
	{
		printf( "%02X:", Pro2Cmd[i] );
	}
	printf( "%02X]\r\n", Pro2Cmd[i] );

#endif

	return 0;
}

//========================================================================
int		cmd_rfwds1		( int argc, char *argv[] )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	return 0;
}

//========================================================================
int		cmd_rfwds2		( int argc, char *argv[] )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	return 0;
}

//========================================================================
int		cmd_rfwds3		( int argc, char *argv[] )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	return 0;
}

//========================================================================
int		cmd_rfwds_init	( int argc, char *argv[] )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

#if defined(_WIN32)
#else

	vRadio_Init();

#endif

	return 0;
}

//========================================================================
int		cmd_rfwds_vinit	( int argc, char *argv[] )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	return 0;
}

//========================================================================
int		cmd_rfwds_debug	( int argc, char *argv[] )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	/*
#if defined(_WIN32)
#else

	if ( g_bEnableDbgRadioComm )
	{
		g_bEnableDbgRadioComm = 0;
		printf( "Disable Debug\n" );
	}
	else
	{
		g_bEnableDbgRadioComm = 1;
		printf( "Enable Debug\n" );
	}

#endif

	*/
	
	return 0;
}


#if	defined(USE_BOOTLOADER)
////========================================================================
//int cmd_test3(int argc, char *argv[])
////========================================================================
//{
//	return 0;
//}
//
////========================================================================
//int cmd_test4(int argc, char *argv[])
////========================================================================
//{
//	return 0;
//}

#else		//		Application

#endif


//========================================================================
int GetDbgLevel( void )
//========================================================================
{
	return debug_level;
}

//========================================================================
void memory_modify(void *mem_addr, int unit)
//========================================================================
{
	unsigned char	v1;
	unsigned short	v2;
	unsigned int	v4;
	unsigned char	*addrp1 = (unsigned char *)mem_addr;
	unsigned short	*addrp2 = (unsigned short *)mem_addr;
	unsigned int	*addrp4 = (unsigned int *)mem_addr;
	char			inbuf[0x80];

	if (unit == 1)
	{
		while (1)
		{
			v1 = *addrp1;
			printf("%08p - %02x ", addrp1, v1);
			v1 = readline(inbuf);
			if (0 < v1)
			{
				if (inbuf[0] == '.')
				{
					break;
				}
				v1 = a2hex(inbuf);
				*addrp1 = v1;
			}
			addrp1++;
		}
	}
	else if (unit == 2)
	{
		while (1)
		{
			v2 = *addrp2;
			printf("%08p - %04x ", addrp2, v2);
			v2 = readline(inbuf);
			if (0 < v2)
			{
				if (inbuf[0] == '.')
				{
					break;
				}
				v2 = a2hex(inbuf);
				*addrp2 = v2;
			}
			addrp2++;
		}
	}
	else if (unit == 4)
	{
		while (1)
		{
			v4 = *addrp4;
			printf("%08p - %08x ", addrp4, v4);
			v4 = readline(inbuf);
			if (0 < v4)
			{
				if (inbuf[0] == '.')
				{
					break;
				}
				v4 = a2hex(inbuf);
				*addrp4 = v4;
			}
			addrp4++;
		}
	} 
}

//========================================================================
int cmd_md( int argc, char *argv[] )
//========================================================================
{
	if ( 1 < argc )
	{
		mem_addr = (unsigned char *)a2hex( argv[1] );
	}
	if ( 2 < argc )
	{
		dumpsize = a2hex( argv[2] );
	}
	if ( mem_addr == 0 )
	{
		return -1;
	}

	dump( (unsigned char *)mem_addr, dumpsize, data_option );
	mem_addr += dumpsize;

	return 0;
}

//========================================================================
int cmd_mm( int argc, char *argv[] )
//========================================================================
{
	if ( 1 < argc )
	{
		mem_addr = (unsigned char *)a2hex( argv[1] );
	}
	if ( mem_addr == 0 )
	{
		return -1;
	}

	memory_modify( (void *)mem_addr, data_option );

	return 0;
}

//========================================================================
int cmd_debug(int argc, char *argv[])
//========================================================================
{
	if (argc == 1)
	{
		printf("current debug level = 0x%x\n", debug_level);
	}
	else
	{
		debug_level = a2hex(argv[1]);
		printf("debug level = 0x%x\n", debug_level);
	}

	return 0;
}

//========================================================================
int cmd_wr( int argc, char *argv[] )
//========================================================================
{
	unsigned int	addr = 0;
	unsigned int	v;

	int	index = 1;
	int	option_loop = 0;

	if ( (index <= argc) && (strcmp( argv[index], "-l" ) == 0) )
	{
		option_loop = 1;
		index++;
	}
	if ( index < argc )
	{
		addr = a2hex( argv[index] );
		index++;
	}
	if ( index < argc )
	{
		v = a2hex( argv[index] );
	}
	else
	{
		return -1;
	}
	do
	{
		if ( data_option == 1 )
		{
			*(unsigned char *)addr	=	(unsigned char)v;
		}
		else if ( data_option == 2 )
		{
			*(unsigned short *)addr	=	(unsigned short)v;
		}
		else if ( data_option == 4 )
		{
			*(unsigned int *)addr	=	(unsigned int)v;
		}

#if defined(USE_FREERTOS)
		vTaskDelay( 100 / portTICK_RATE_MS );
#else
		HAL_Delay( 0 );
#endif

	} while ( option_loop == 1 && input_check() == 0 );

	printf( "\n" );

	return 0;
}

//========================================================================
int cmd_rd( int argc, char *argv[] )
//========================================================================
{
	unsigned int	addr;
	unsigned int	v;

	int		index = 1;
	int		option_loop = 0;

	if ( (index <= argc) && (strcmp( argv[index], "-l" ) == 0) )
	{
		option_loop = 1;
		index++;
	}
	if ( index < argc )
	{
		addr = a2hex( argv[index] );
	}
	else
	{
		return -1;
	}

	do
	{
		if ( data_option == 1 )
		{
			v = *(unsigned char *)addr;
			printf( "addr 0x%08x, value 0x%02x\r", addr, v );
		}
		else if ( data_option == 2 )
		{
			v = *(unsigned short *)addr;
			printf( "addr 0x%08x, value 0x%04x\r", addr, v );
		}
		else if ( data_option == 4 )
		{
			v = *(unsigned int *)addr;
			printf( "addr 0x%08x, value 0x%08x\r", addr, v );
		}

#if defined(USE_FREERTOS)
		vTaskDelay( 10 / portTICK_RATE_MS );
#else
		HAL_Delay( 0 );
#endif

	} while ( option_loop == 1 && input_check() == 0 );

	printf( "\n" );

	return 0;
}

//========================================================================
int cmd_do(int argc, char *argv[])
//========================================================================
{
	int	val = 0;
	extern	void	gpio_write(int val);

	if (2 <= argc)
	{
		sscanf(argv[1], "%x", &val);
	}
	//gpio_write(val);

	return 0;
}

//========================================================================
int cmd_test( int argc, char * argv[] )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );
}
