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

#ifndef	_CLI_H_

#include "typedef.h"	//	uint8_t

#define	OPENED_COMMAND_IDX	5	/* for release version: 3 */
#define	CLOSED_COMMAND_IDX	0

extern	int	open_command_idx;

typedef	struct	user_commnand
{
	char	*command_name;
	char	*command_help1;
	char	*command_help2;
	int		( *func )(int argc, char *argv[]);
} user_command_t;

extern	user_command_t	user_command_table[];

int 	a2hex		( char *pv );
int 	readline	( char *bufp );
void 	dump		( unsigned char *a, int delta, int width );
int 	cmd_period	( int argc, char *argv[] );
int 	cmd_ts		( int argc, char *argv[] );

void	LoopProcCLI ( void );

#if defined(_WIN32)
DWORD	vCLITask	( void *pvParameters );
#else	//	RTOS
void	vCLITask	( void *pvParameters );	//	LED Task
#endif

extern int			data_option;

//========================================================================
//	Default CLI Process
void	CLIPrompt		( void );
int		ProcessCommand	( char *cmd );
//========================================================================

//========================================================================
void	SetCLIPrompt	( void ( *fnPrompt )( void ) );
void	SetCLIProc		( int ( *fnProc )( char *cmd ) );
//========================================================================

//========================================================================
//	Default Command

int cmd_help	( int argc, char *argv[] );
int cmd_reset	( int argc, char *argv[] );
int cmd_uptime	( int argc, char *argv[] );
int cmd_ver		( int argc, char *argv[] );

int cmd_wr		( int argc, char *argv[] );
int cmd_rd		( int argc, char *argv[] );

int cmd_rfm		( int argc, char *argv[] );


//========================================================================

#endif	/* _CLI_H_ */
