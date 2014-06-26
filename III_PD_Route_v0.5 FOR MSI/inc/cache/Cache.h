/*
*	CopyRight : ESTI_�굦�|�s�����z��
*	Description : III_Pedestrian-Route I/O �֨��O����
*	Version & Date : v0.3  2010/5/24
*/

#ifndef CACHE_H
#define CACHE_H

#include "prototype.h"
#include "platform.h"

typedef struct _Cache
{
  III_INT bsize;  // block size
  III_INT log_bsize; // block size index
  III_INT tagFlag; // block flag
  III_INT offsetFlag; // offset flag
  III_INT blockFlag; //block flag
  III_INT hit;   //hit content
  III_INT miss;  //miss content
  III_CHAR *ptr; //I/O File pointer
  III_INT *tag; //block number
} Cache;

#ifdef __cplusplus
extern "C"
{
#endif
    
	/*
	*	Function:
	*	    ��l��Cache
	*	parameter:
	*	     c - [in] Cache struct
	*		 bsizeBit - [in] pagesize
	*		 bcountBit - [in] pagenumber
	*		 [out] see III_Route_Manager.h error definition
	*/
	III_INT CacheInit ( Cache *, III_INT, III_INT );
	
	/*
	 *	Function:
	 *	    �qCache���X�w�Ȧs�����
	 *	parameter:
	 *	     c - [in] Cache struct
	 *		 offset - [in] Data offset
	 *		 size - [in] Data size
	 *		 id - [in] RouteTable ID
	 *		 buf - [in] Data buffer
	 *		 fp - [in] I/O file pointer
	*/
	III_VOID CacheRead ( Cache *, III_INT, III_INT, III_INT, void *, III_FILE * );

	/*
	 *	Function:
	 *		�M��Cache���e
	 *	Parameter:
	 *		c - [in] Cache struct
	 *		[out] see III_Route_Manager.h error definition
	*/
	III_INT CacheClean ( Cache * );

#ifdef __cplusplus

}

#endif

#endif
