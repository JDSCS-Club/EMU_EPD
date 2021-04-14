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

#ifndef	_DIAG_H_
#define	_DIAG_H_

//========================================================================

extern	unsigned char *mem_addr;
extern	int	dumpsize, data_option;
extern	int	debug_level;

int		GetDbg		( void );

void	memory_modify	( void *mem_addr, int unit );

//========================================================================

//int		cmd_test		( int argc, char *argv[] );
int		cmd_test1		( int argc, char *argv[] );
int		cmd_test2		( int argc, char *argv[] );
int		cmd_test3		( int argc, char *argv[] );
int		cmd_test4		( int argc, char *argv[] );
int		cmd_test4		( int argc, char *argv[] );

int		cmd_ttest		( int argc, char *argv[] );
int		cmd_md			( int argc, char *argv[] );
int		cmd_mm			( int argc, char *argv[] );
int		cmd_do			( int argc, char *argv[] );

int		cmd_diag		( int argc, char *argv[] );

//========================================================================

void	display_diag_RF_WDS	( void );
int		cmd_diag_RF_WDS		( int argc, char *argv[] );

int		cmd_rfwds_info	( int argc, char *argv[] );
int		cmd_rfwds1		( int argc, char *argv[] );
int		cmd_rfwds2		( int argc, char *argv[] );
int		cmd_rfwds3		( int argc, char *argv[] );
int		cmd_rfwds_init	( int argc, char *argv[] );
int		cmd_rfwds_debug	( int argc, char *argv[] );

int		cmd_diag_Audio	( int argc, char *argv[] );

//========================================================================

int		cmd_debug		( int argc, char *argv[] );
int		cmd_uptime		( int argc, char *argv[] );
int		cmd_ver			( int argc, char *argv[] );
int		cmd_help		( int argc, char *argv[] );
int		cmd_wr			( int argc, char *argv[] );
int		cmd_rd			( int argc, char *argv[] );
int		cmd_reset		( int argc, char *argv[] );
int		logprintf		( const char *fmt, ... );
int		cmd_logpr		( int argc, char *argv[] );
int		cmd_logx		( int argc, char *argv[] );

int		cmd_id			( int argc, char *argv[] );

int		cmd_rts			( int argc, char *argv[] );

int		cmd_test		( int argc, char * argv[] );

//========================================================================

int		cmd_audio		( int argc, char *argv[] );
int		cmd_codec		( int argc, char *argv[] );

//========================================================================


#endif	/* _DIGA_H_ */

