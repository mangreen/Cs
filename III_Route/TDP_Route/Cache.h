/*
*	CopyRight : ESTI_資策會新興智慧所
*	Description : III_Pedestrian-Route I/O 快取記憶體
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
	*	    初始化Cache
	*	parameter:
	*	     c - [in] Cache struct
	*		 bsizeBit - [in] pagesize
	*		 bcountBit - [in] pagenumber
	*		 [out] see III_Route_Manager.h error definition
	*/
	III_INT CacheInit ( Cache *, III_INT, III_INT );
	
	/*
	 *	Function:
	 *	    從Cache取出已暫存的資料
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
	 *		清除Cache內容
	 *	Parameter:
	 *		c - [in] Cache struct
	 *		[out] see III_Route_Manager.h error definition
	*/
	III_INT CacheClean ( Cache * );

#ifdef __cplusplus

}

#endif

#endif
