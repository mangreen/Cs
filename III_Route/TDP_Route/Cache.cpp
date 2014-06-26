/*
*	CopyRight : ESTI_資策會新興智慧所
*	Description : III_Pedestrian-Route I/O 快取記憶體
*	ex: CacheInit(c, 10, 12)
*	it means each block size is 2^10=1024 Bytes
*	and there are totally 2^12=4096 blocks
*	so size of cache is 1024*4096 = 4MB
*	Version & Date : v0.3  2010/5/24
*/

#include "Cache.h"

/*
*	Function:
*	    初始化Cache
*	parameter:
*	     c - [in] Cache struct
*		 bsizeBit - [in] pagesize
*		 bcountBit - [in] pagenumber
*		 [out] see III_Route_Manager.h error definition
*/
III_INT CacheInit(Cache *c, III_INT bsizeBit, III_INT bcountBit)
{
	III_INT bcount = 1 << bcountBit;
	c->log_bsize = bsizeBit;
	c->bsize = 1 << bsizeBit;

	c->tagFlag = 0xffffffff - ( ( 1 << ( bsizeBit + bcountBit ) ) - 1 );
	c->tag = (III_INT *)malloc ( sizeof ( III_INT ) * bcount );
	//初始化c->tag
	memset ( c->tag, bcount * 4, 0 );
	
	c->ptr = (III_CHAR *)malloc ( sizeof ( III_CHAR ) * c->bsize * bcount );
	c->offsetFlag = ( bcount - 1 ) << bsizeBit;
	c->blockFlag = c->bsize - 1;
	c->hit = c->miss = 0;
	return ERROR_SUCCESS;
}

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
III_VOID CacheRead(Cache *c, III_INT offset, III_INT size, III_INT id, void *buf, III_FILE *fp)
{
	III_INT idx = (offset & c->offsetFlag) >> c->log_bsize;
	III_INT tmpTag = id + (offset &c->tagFlag);

	if ( c->tag[idx] == tmpTag )
	{   //hit
	    III_INT tmp = offset & c->blockFlag;
	
	    if ( tmp + size <= c->bsize )
	    {
			c->hit++;
			memcpy ( buf, c->ptr + idx * c->bsize + tmp, size );
			return ;
		}
		memcpy ( buf, c->ptr + idx * c->bsize + tmp, c->bsize - tmp ); //overflow
		CacheRead ( c, offset + c->bsize - tmp, size + tmp - c->bsize, id, (III_CHAR *)buf + c->bsize - tmp, fp );
		return ;
	}

	c->miss++;
	c->tag[idx] = tmpTag; //miss

	III_fseek ( fp, offset - (offset & c->blockFlag), SEEK_SET );
	III_fread ( c->ptr + idx * c->bsize, sizeof ( III_CHAR ), c->bsize, fp );
	CacheRead ( c, offset, size, id, buf, fp );
}

/*
*	Function:
*		清除Cache內容
*	Parameter:
*		c - [in] Cache struct
*		[out] see III_Route_Manager.h error definition
*/
III_INT CacheClean(Cache *c)
{
	free ( c->ptr );
	free ( c->tag );
	return ERROR_SUCCESS;
}
