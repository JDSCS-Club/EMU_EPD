/*------------------------------------------------------------------------------------------
	Project			: It Library
	Description		: Queue

	Writer			: $Author: zlkit $
	Revision		: $Rev: 1891 $
	Date			: 2019. 10.
	Copyright		: Piranti Corp.	( zlkit@piranti.co.kr )
	 
	Revision History 
	1. 2019. 10.	: Created
-------------------------------------------------------------------------------------------*/

//========================================================================
#ifndef QUEUE_BUF_H
#define QUEUE_BUF_H
//========================================================================

//#include "typedef.h"	//	uint8_t
#include <stdint.h>		//	uint8_t

typedef struct _Queue_t
{
#define QMAX 256

	unsigned char queue[QMAX];

	int front, rear;

} Queue_t;

//========================================================================
int				qcount		( Queue_t *q );
void			init_queue	( Queue_t *q );
void			clear_queue	( Queue_t *q );
int				qput		( Queue_t *q, unsigned char k );
unsigned char	qget		( Queue_t *q );
//========================================================================


typedef struct _QBuf_t
{
//#define QBUFMAX 128

	uint8_t	*qBuf;
	int	size;

	int front, rear;

} QBuf_t;

//===========================================================================
void	qBufInit	( QBuf_t *q, uint8_t *_qBuf, int _size );
void	qBufClear	( QBuf_t *q );
int		qBufCnt		( QBuf_t *q );
int		qBufPut		( QBuf_t *q, uint8_t *pBuf, int size );
int		qBufGet		( QBuf_t *q, uint8_t *pBuf, int size );
//===========================================================================

//===========================================================================
void	QBufTest	( QBuf_t *q, int blkSize );
//===========================================================================


//========================================================================
#endif		//	QUEUE_BUF_H
//========================================================================


