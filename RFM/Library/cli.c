/*------------------------------------------------------------------------------------------
	Project			: RFM
	Description		: console과 관련된 함수들 및 command line 처리

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

#if defined(USE_FREERTOS)
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "list.h"
#endif

#if defined(STM32F407xx)
#include "stm32f4xx.h"
#elif defined(STM32F207xx)
#include "stm32f2xx.h"
#endif

//#include "i2c.h"

//=============================================================================
#endif
//=============================================================================

#include "cli.h"
#include "serial.h"

#include "version.h"		//	APP_VER / APP_BUILD_DATE

#include "rfm.h"			//	cmd_ch()

#include "diag.h"

#include "eeprom.h"			//	cmd_eepromRead() / cmd_eepromWrite()

#include "audio.h"			//	cmd_audio() / cmd_codec()

#include "ProcPkt.h"		//	cmd_showpkt()

#include "bootloader.h"		//	cmd_stboot()

#include "upgrade.h"		//	cmd_upgrade()

#define		NELEMENTS(array)	(sizeof(array) / sizeof((array)[0]))
#define		MAX_COMMAND_LENGTH	64

#if			defined(USE_BOOTLOADER)
#define		PROMPT_STRING	"bootloader> "
#else	//	LED
//#define		PROMPT_STRING	"STM> "
#define		PROMPT_STRING	"RFM> "
#endif

#define		CLEAR_VT_SCREEN()	printf("\033[2J")
#define		GOTO_VT_XY()		printf("\033[1;1H")

int			input_check		( void );

char		prompt_string[0x10];

int			data_option = 1;

//=============================================================================

/*
 * command tables
 */
user_command_t	user_command_table[] = {
	/*
	 * {"xxxx",		명령
	 * 	abc		help string1
	 * 	def		help string2
	 * 	func,},		실제 처리 함수
	 */
	{"help",
		"help		-	display commands",
		(char *)0,
		cmd_help,},
	{"uptime",
		"uptime		-	display uptime",
		(char *)0,
		cmd_uptime,},
	{"ver",
		"ver		-	display version.",
		(char *)0,
		cmd_ver,},
	{"reset",
		"reset		-	restart system",
		(char *)0,
		cmd_reset,},

#if defined(USE_HOP_MANUAL)

#endif	//	defined(USE_HOP_MANUAL)

#if defined(USE_HOP_MANUAL)
	{"hop",
		"hop		-	hopping",
		"hop -	hop [ 0:default / 1:On / 2:Off ]",
		cmd_hop},
#endif	//	 defined(USE_HOP_MANUAL)

#if defined(_DIAG_H_)
	{"diag",
		"diag		-	diagnostic test",
		(char *)0,
		cmd_diag,},
#endif	//	defined(_DIAG_H_)

	{"debug",
		"debug		-	set debug level.",
		(char *)0,
		cmd_debug, },

#if defined(RFM_H)
	{"ch",
		"ch		-	Setting Channel",
		"ch -	ch [channel:0-9]",
		cmd_ch},
	{"ts",
		"ts		-	Setting Train Set",
		"ts -	ts [channel:0-9]",
		cmd_ts},
	{"car",
		"car	-	Setting Car No.",
		"car -	car [channel:0-9]",
		cmd_car},
	{"info",
		"info	-	Display Information",
		(char *)0,
		cmd_info},
	{"swinfo",
		"info	-	Software Information",
		(char *)0,
		cmd_swinfo},
	{"rfstat",
		"rfstat	-	RF Tx / Rx Information",
		(char *)0,
		cmd_rfstat},
	{"rspid",
		"rspid	-	Manual Set Resp ID",
		"rspid	-	rspid [id] [set/clear]",
		cmd_rspid},
	{"occ",
		"occ		-	OCC PA Start/Stop",
		"occ [1(Start)/0(Stop)]",
		cmd_OccPa},
#endif	//	defined(RFM_H)

#if defined(EEPROM_I2C_H)
		{"eepw",
			"eepw			-	write byte to address",
			"eepw [addr] [data]",
			cmd_eepromWrite,},
		{"eepr",
			"eepr			-	read byte from address",
			"eepr [addr]",
			cmd_eepromRead,},
		{"eepdump",
			"eepdump		-	eeprom dump",
			"eepdump [addr] [size]",
			cmd_eepromDump,},
#endif	//	defined(EEPROM_I2C_H)

#if defined(USE_ENV_TEST)
	{"rftx",
		"rftx	-	Loop RF Tx Data",
		"rttx [1(On) / 0(Off)] [u(unmodulated)/m(modulated)] [ch:0~9] [power:0~127]",
		cmd_rftx },

#endif	//	defined(USE_ENV_TEST)

#if defined( AUDIO_H )
	{"audio",
		"audio	-	audio command",
		"audio [loop/null/sine/spk [0/1] / mute [0/1] ]",
		cmd_audio},
	{"codec",
		"codec	-	codec command",
		"codec [init/loop/mute [0/1] ]",
		cmd_codec},
#endif

#if defined( BOOTLOADER_H )
	{"stboot",
		"stboot	- Enter STM32 Bootloader ( DFU Mode )",
		(char *)0,
		cmd_stboot},
#endif

#if defined( UPGRADE_H )
	{"upgrade",
		"upgrade	- upgrade command",
		(char *)0,
		cmd_upgrade},
#endif

#if defined( PROC_PKT_H )
	{"mon",
		"mon	-	show packet monitoring",
		"mon [0/1]",
		cmd_pktmon},
#endif


#if 0

	{"test",
		(char *)0,
		(char *)0,
		cmd_test,},
	{"do",
		"do			-	set do value",
		(char *)0,
		cmd_do,},
	{"wr",
		"wr			-	write byte to address",
		"wr[.b|w|l] addr data",
		cmd_wr,},
	{"rd",
		"rd			-	read byte from address",
		"rd[.b|w|l] [-l] addr",
		cmd_rd,},
	{"md",
		"md			-	memory dump",
		"md[.b|w|l] [address] [count]",
		cmd_md},
	{"i2cdetect",
		"i2cdetect	-	I2C detect",
		"i2cdetect -l",
		cmd_md},
	{"i2cset",
		"i2cset	-	I2C Write",
		"i2cset	I2CBUS CHIP-ADDRESS DATA-ADDRESS [VALUE]",
		cmd_md},
	{"i2cget",
		"i2cget	-	I2C Read",
		"i2cget	I2CBUS CHIP-ADDRESS [DATA-ADDRESS [MODE]]",
		cmd_md},
	{"rftx",
		"rftx	-	RF Tx Test",
		"rftx [start/1/stop/0]",
		cmd_rftx},

#endif

#if defined(_WIN32)

	//	Windows

#else	//	STM32

#if defined( _SPI_H_ )
	{"spi",
		"spi -	SPI Read / Write",
		"spi[.b|w|l] [wr/rd] [dev [data]]",
		cmd_spi},
#endif 	//	defined( _SPI_H_ )

#endif

/*
	{"ts",
		"ts       - display task status",
		(char *)0,
		cmd_ts,},

 */
};

char	old_cmd[MAX_COMMAND_LENGTH];

//=============================================================================

//========================================================================
void dump( unsigned char *a, int delta, int width )
//========================================================================
{
	int     i, j, k;
	unsigned char   *b;
	unsigned char   c;
	int		disp_unit = 16 / width;
	char	*format[] = {
							"",
							"   ",			/* width 1 */
							"     ",		/* width 2 */
							"",
							"         ",	/* width 4 */
						};

	for ( i = 0; i < delta; )
	{
		b = a;
		j = i;
		printf( "%08p  ", a );
		for ( k = 0; k < disp_unit && i < delta; k++, i += width )
		{
			if ( k == 8 )
			{
				printf( " " );
			}
			if ( width == 1 )
			{
				printf( "%02x ", *a );
			}
			else if ( width == 2 )
			{
				printf( "%04x ", *(unsigned short *)a );
			}
			else if ( width == 4 )
			{
				printf( "%08x ", *(unsigned long *)a );
			}
			a += width;
		}
		for ( ; k < disp_unit; k++ )
		{
			if ( k == 8 )
				printf( " " );
			printf( format[width] );
		}
		printf( "|" );
		for ( k = 0; k < 16 && j < delta; k++, j++ )
		{
			if ( k == 8 )
				printf( " " );
			c = *b++ & 0x7f;
			if ( c < ' ' || c == 0x7f )
				printf( "%c", '.' );
			else
				printf( "%c", c );
		}
		for ( ; k < 16; k++ )
		{
			if ( k == 8 )
				printf( " " );
			printf( " " );
		}
		printf( "|\n" );
	}
}

//========================================================================
int 	a2hex(char *pv)
//========================================================================
{
	char **pos=NULL;

	return strtol(pv, pos, 16 );
}

//=============================================================================
#if defined(_WIN32)
//=============================================================================

char *
strtok_r ( char *s, const char *delim, char **save_ptr )
{
	char *end;

	if ( s == NULL )
		s = *save_ptr;

	if ( *s == '\0' )
	{
		*save_ptr = s;
		return NULL;
	}

	/* Scan leading delimiters.  */
	s += strspn ( s, delim );
	if ( *s == '\0' )
	{
		*save_ptr = s;
		return NULL;
	}

	/* Find the end of the token.  */
	end = s + strcspn ( s, delim );
	if ( *end == '\0' )
	{
		*save_ptr = end;
		return s;
	}

	/* Terminate the token and make *SAVE_PTR point past it.  */
	*end = '\0';
	*save_ptr = end + 1;
	return s;
}

//========================================================================
#endif
//========================================================================

/*
 * parse cmd & argument, return argc
 */
//========================================================================
int getarg(char *buffer, char *argv[])
//========================================================================
{
	int		argc;
	char	*p;
	char	*sepchr = " \t";
	char	*saveptr;

#define	MAXP	20

	p = (char *)strtok_r(buffer, sepchr, &saveptr);

	for ( argc = 0; argc < MAXP && p != NULL; p = (char *)strtok_r( NULL, sepchr, &saveptr ) )
	{
//		printf( "%s(%d) - %s\n", __func__, __LINE__, p );
		argv[argc++] = p;
	}

	return argc;
}

//========================================================================
int ProcessCommand( char *cmd )
//========================================================================
{
	char	*argv[MAXP];
	int		argc;
	char	*p;
	int		v = 0;
	int		i;
	char	temp[80];
	char	upstr[4] = { 0x1b, '[', 'A', 0 };

	if ( strcmp( "!!", cmd ) == 0 || strcmp( cmd, upstr ) == 0 )
	{
		strcpy( temp, old_cmd );
		strcpy( cmd, old_cmd );
	}
	else
	{
		strcpy( temp, cmd );
	}

	memset( argv, 0, sizeof( argv ) );

	// ignore after # char
	p = (char *)strchr( cmd, '#' );
	if ( p )
	{
		*p = (char)NULL;
	}

	// 명령라인을 arg,argv 로 분해
	argc = getarg( cmd, argv );

//	printf( "%s(%d) - argc(%d) / argv(%s)\n", __func__, __LINE__, argc, argv[0] );

	if ( argc != 0 )
	{

		// md/mm 등에 따르는 "[.b|.w|.l]에 대한 처리
		p = strchr( argv[0], '.' );
		if ( p != NULL )
		{
			if ( p[1] == 'b' )
			{
				data_option = 1;
			}
			else if ( p[1] == 'w' )
			{
				data_option = 2;
			}
			else if ( p[1] == 'l' )
			{
				data_option = 4;
			}
			*p = (char)NULL;
		}
		else
		{
			//	default
			data_option = 1;
		}

		// 명령을 table에서 찾아서 
		for ( i = 0; i < NELEMENTS( user_command_table ); i++ )
		{
			if ( strcmp( argv[0], user_command_table[i].command_name ) == 0 )
			{
				if ( argc >= 2 )
				{
					if ( (strcmp( argv[1], "help" ) == 0) || (strcmp( argv[1], "-h" ) == 0) )
					{
						printf( "%s\n", user_command_table[i].command_help1 );
						if ( user_command_table[i].command_help2 != NULL )
						{
							printf( "%s\n", user_command_table[i].command_help2 );
						}
						v = 1;
						break;
					}
				}
				// 처리함수를 호출
				if ( user_command_table[i].func( argc, argv ) < 0 )
				{
					printf( "%s\n", user_command_table[i].command_help1 );
					if ( user_command_table[i].command_help2 != NULL )
					{
						printf( "%s\n", user_command_table[i].command_help2 );
					}
				}
				v = 1;	// 명령을 찾아서 처리했음을 표시
				break;
			}

		}
		if ( v == 0 )
		{
			// 엉뚱하게 입력한 경우
			printf( "%s: command not found\n\r", argv[0] );
		}
		else
		{
			// !! 처리를 위해서 old_cmd로 명령 복사
			strcpy( old_cmd, temp );
		}
	}

	return 1;
}

static char		command_buf[MAX_COMMAND_LENGTH];
static int		command_index = 0;

//========================================================================
int readline( char *bufp )
//========================================================================
{
	//	printf( "%s\n", __func__ );

	char	ch;
	char	*command_buf = bufp;	//[MAX_COMMAND_LENGTH];

	int		ret = 0;

	for ( ;;)
	{
		ch = getchar();

#if defined(USE_FREERTOS)
#else
		if ( ch == (char)-1 )
		{
			//	Input Data가 없는 경우 ( Non-Blocking - OS less )
			return -1;
		}
#endif

		if ( ch == 0x08 || ch == 0x7F )	// Backspace
		{
			if ( command_index > 0 )
			{
				printf( "%c %c", 8, 8 );
				command_index--;
			}
		}
		else if ( ch == 0x15 )			// control-u, all input clear
		{
			while ( 0 < command_index )
			{
				printf( "%c %c", 8, 8 );
				command_index--;
			}
		}
		else if ( ch == 0x17 )			// control-w, erase one word
		{
			/* erase space */
			while ( (0 < command_index) && (command_buf[command_index] == ' ') ) {
				printf( "%c %c", 8, 8 );
				command_index--;
			}
			/* erase char not space */
			while ( (0 < command_index) && (command_buf[command_index] != ' ') ) {
				printf( "%c %c", 8, 8 );
				command_index--;
			}
		}
		else if ( (ch == 0x0D) || (ch == 0x0A) )
		{
			printf( "\n\r" );
			command_buf[command_index] = 0; // Null terminate the input command

			ret = command_index;
			command_index = 0;		//	입력후 index 초기화.

			return ret;
		}
		else if ( (0x20 <= ch) && (ch <= 0x7E) )
		{
			//	ANSI 입력.
			if ( command_index < (MAX_COMMAND_LENGTH - 2) )
			{
				command_buf[command_index++] = ch;
			}
			printf( "%c", ch );    // Echo the character
//			printf( "[%02X]", ch );    // Echo the character
		}
		else 
		{
			//	한글입력.
			if ( command_index < ( MAX_COMMAND_LENGTH - 2 ) )
			{
				command_buf[command_index++] = ch;
			}
			printf( "%c", ch );    // Echo the character
//			printf( "[%02X]", ch );    // Echo the character
		}

#if defined(USE_FREERTOS)
		//	FreeRTOS - Task Switching
		taskYIELD();
#else
//		HAL_Delay( 0 );
#endif
	}

	return 0;
}

//========================================================================
void	CLIPrompt( void )
//========================================================================
{
	printf( "%s", PROMPT_STRING );
}

void	( *g_fnCLIPrompt )( void ) = CLIPrompt;
int		( *g_fnCLIProc )( char * ) = ProcessCommand;

//========================================================================
void	SetCLIPrompt( void ( *fnPrompt )( void ) )
//========================================================================
{
	g_fnCLIPrompt = fnPrompt;
}

//========================================================================
void	SetCLIProc( int ( *fnProc )( char *cmd ) )
//========================================================================
{
	g_fnCLIProc = fnProc;
}


//========================================================================
void	LoopProcCLI ( void )
//========================================================================
{
	int len;
	if ( input_check() )
	{
		len = readline( command_buf );
		if ( len >= 0 )
		{
//			printf( "%s(%d) : %s\n", __func__, __LINE__, command_buf );
			//	Process Command
//			ProcessCommand( command_buf );
			g_fnCLIProc( command_buf );

			//	Print Prompt
//			printf( "%s", PROMPT_STRING );
			g_fnCLIPrompt();
		}
	}
}


//========================================================================
#if defined(_WIN32)
DWORD	vCLITask	( void *pvParameters )
#else	//	RTOS
void	vCLITask	( void *pvParameters )
#endif
//========================================================================
{
	/* The parameters are not used. */
	( void ) pvParameters;

	/*
	 * print Logo & version
	 */
//	CLEAR_VT_SCREEN();
//	GOTO_VT_XY();

//	user_commands = NELEMENTS(user_command_table);

	for(;;)
	{
		LoopProcCLI();

#if defined(USE_FREERTOS)
		taskYIELD();
#else
		HAL_Delay( 0 );
#endif
	}
}

//========================================================================
int cmd_help( int argc, char *argv[] )
//========================================================================
{
	int	i;
	int	ok = 0;

//	printf( "%s(%d)\n", __func__, __LINE__ );

	for ( i = 1; i < NELEMENTS( user_command_table ); i++ )
	{
		if ( 1 < argc )
		{
			if ( strcmp( argv[1], user_command_table[i].command_name ) == 0 )
			{
				if ( user_command_table[i].command_help1 != NULL )
				{
					printf( "%s\n", user_command_table[i].command_help1 );
				}
				if ( user_command_table[i].command_help2 != NULL )
				{
					printf( "%s\n", user_command_table[i].command_help2 );
				}
				ok = 1;
			}
		}
		else
		{
			if ( user_command_table[i].command_help1 != NULL )
			{
				printf( "%s\n", user_command_table[i].command_help1 );
			}
			ok = 1;
		}
	}
	if ( ok == 0 )
	{
		printf( "unknown command %s\n", argv[1] );
	}
	return 0;
}

//========================================================================
int cmd_uptime(int argc, char *argv[])
//========================================================================
{

//=============================================================================
#if defined(_WIN32)
//=============================================================================

//=============================================================================
#else	//	stm32f207
//=============================================================================

#if defined( USE_FREERTOS )
	unsigned long	tmp = xTaskGetTickCount() / configTICK_RATE_HZ;
#else
	unsigned long	tmp = HAL_GetTick() / 1000;
#endif

	int	sec, min, hour;
	int	day;

	day = (tmp / 3600 / 24);
	tmp = tmp % (3600 * 24);

	hour = tmp / 3600;
	tmp = tmp % 3600;

	min = tmp / 60;
	sec = tmp % 60;

	if (0 < day)
	{
		printf("%d day(s) %d:%02d:%02d up.\n", day, hour, min, sec);
	}
	else
	{
		printf("%d:%02d:%02d up.\n", hour, min, sec);
	}

//=============================================================================
#endif
//=============================================================================

	return 0;
}

//========================================================================
int cmd_ver(int argc, char *argv[])
//========================================================================
{
	printf("Version = %s(%s)\n", APP_VER, APP_BUILD_DATE );

	return 0;
}


//========================================================================
int cmd_reset(int argc, char *argv[])
//========================================================================
{
  	NVIC_SystemReset();
	return 0;
}

/*
//========================================================================
int cmd_ts(int argc, char *argv[])
//========================================================================
{
	char	*pbuffer;

	pbuffer = pvPortMalloc( 512 );
	vTaskList(pbuffer);
	printf("Name            Status  Pri     StkHigh Task#\n");
	printf(pbuffer);
	vPortFree(pbuffer);
	return 0;
}
*/


