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
// Header

#include <stdio.h>			//	printf()
#include <string.h>			//	memcpy()

#include "queueBuf.h"

//========================================================================
// Define


//========================================================================
// Function

//------------------------------------------------------------------------------
//         serial queue functions
//------------------------------------------------------------------------------


//===========================================================================
void init_queue( Queue_t *q )
//===========================================================================
{
	q->front = q->rear = 0;
}

//===========================================================================
void clear_queue( Queue_t *q )
//===========================================================================
{
	q->front = q->rear;
}

//===========================================================================
int qcount( Queue_t *q )
//===========================================================================
{
	return (((q->rear+QMAX)-q->front) % QMAX);
}

//===========================================================================
int qput( Queue_t *q, unsigned char k )
//===========================================================================
{
	if ( (q->rear + 1) % QMAX == q->front )
	{
//		printf( "\n\tCircular Queue Overflow." );
//		printf( "\n\tQOver" );
		return -1;
	}
	q->queue[q->rear] = k;
	q->rear = (q->rear + 1) % QMAX;
	return k;
}

//===========================================================================
unsigned char qget( Queue_t *q )
//===========================================================================
{
	unsigned char i;
	/*
	if(front == rear){
		printf("\n\tCircular Queue Underflow.");
		return -1;
	}
	*/
	i = q->queue[q->front];
	q->front = (q->front + 1) % QMAX;

	return i;
}
//------------------------------------------------------------------------------


//===========================================================================
void qBufInit( QBuf_t *q, uint8_t *_qBuf, int _size )
//===========================================================================
{
	q->front = q->rear = 0;

	q->qBuf = _qBuf;
	q->size = _size;

//	printf( "%s(%d) - q(%p) / _qBuf(%p) / _size(%d)\n", __func__, __LINE__,
//		q, _qBuf, _size);
}

//===========================================================================
void qBufClear( QBuf_t *q )
//===========================================================================
{
	q->front = q->rear;
}

//===========================================================================
int qBufCnt( QBuf_t *q )
//===========================================================================
{
	return ( ( ( q->rear + q->size ) - q->front ) % q->size );
}

//===========================================================================
int qBufPut( QBuf_t *q, uint8_t *pBuf, int size )
//===========================================================================
{
	if ( ( q->rear + size ) % q->size == q->front )
	{
//		printf( "\n\tCircular Queue Overflow." );
		return -1;
	}

	memcpy( &q->qBuf[q->rear], pBuf, size );
//	q->qBuf[q->rear] = k;
	q->rear = ( q->rear + size ) % q->size;

//	printf( "%s(%d) - q(%p) / q->rear(%d) / q->front(%d)\n", __func__, __LINE__,
//		q, q->rear, q->front );

	return size;
}

//===========================================================================
int qBufGet( QBuf_t *q, uint8_t *pBuf, int size )
//===========================================================================
{
	//*
	if ( q->front == q->rear )
	{
//		printf( "\n\tCircular Queue Underflow." );
		return -1;
	}
	//	*/

	memcpy( pBuf, &q->qBuf[q->front], size );

	q->front = ( q->front + size ) % q->size;

//	printf( "%s(%d) - q(%p) / q->rear(%d) / q->front(%d)\n", __func__, __LINE__,
//		q, q->rear, q->front );

	return size;
}


//===========================================================================
void	QBufTest	( QBuf_t *q, int blkSize )
//===========================================================================
{
	int i, j;
	int idx;

	uint8_t	bufPut[128];
	uint8_t	bufGet[128];


	for ( i = 0, idx = 0; i < 20; i++, idx++ )
	{
		//===========================================================================
		memset( bufPut, idx, blkSize );

		printf( "Put : " );
		for ( j = 0; j < blkSize; j++ )
		{
			printf( "%02X ", bufPut[j] );
		}
		printf( "\n" );

		qBufPut( q, bufPut, blkSize );

		//===========================================================================
		qBufGet( q, bufGet, blkSize );

		printf( "Get : " );
		for ( j = 0; j < blkSize; j++ )
		{
			printf( "%02X ", bufGet[j] );
		}
		printf( "\n" );

		//===========================================================================
	}
}

//========================================================================
