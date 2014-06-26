/*
*	CopyRight : ESTI_資策會新興智慧所
*	Description : III_Pedestrian-Route Route 對外介面API
*	Version & Date : v0.3  2010/5/24
*/

#include "III_RouteAPI.h"
#include "III_Route_Pedestrian.h"



//FOR TEST PERFORMANCE 
III_FILE *performaceTestFile = NULL;

//III-Route Method

/*
*		Function:
*			III_SetRoutingParam
*                   依照TYPE_ROUTE_PD的不同[See Route_Manager define]呼叫行人, 大眾, 室內導航的RouteEngine初始化
*		parameter:
*			III_RTPARAM *paramRouting - [in] Route 所需的資訊(ex. routetable name and number , typeRouting, modeRouting )
*			[out] see III_Route_Manager.h error definition
*/
III_INT III_SetRoutingParam(III_RTPARAM *paramRounting)
{
	III_INT checkSetRoutingParamFlag = 0;
	III_INT STARTTIME = 0; //FOR PERFORMANCE TEST
	III_INT ENDTIME = 0;   //FOR PERFORMANCE TEST
	III_CHAR buf[256] = ""; //FOR PERFORMANCE TEST

	typetoroute = paramRounting->typeRouting;

	if ( TYPE_ROUTE_PD == typetoroute)
	{	
		/*
		 *     Function:
		 *	        III_Pedestrian_Route初始化(RouteTable的開啟及讀檔, 初始化Route模式和所需的資料結構和快取記憶體)
		 *     parameter:
		 *			III_RTPARAM *paramRouting - [in] PD_Route 所需的資訊(ex. routetable name and number , typeRouting, modeRouting )
		 *			 [out] see III_Route_Manager.h error definition
		 */
		STARTTIME = GetTickCount();	
		checkSetRoutingParamFlag = III_PD_SetRoutingParam(paramRounting);
		ENDTIME = GetTickCount() - STARTTIME;
		//sprintf(buf, "API_III_PD_SetRoutingParam_SPEND_TIME = %d \n", ENDTIME);   
		III_fwrite(buf, 1, strlen(buf), performaceTestFile);

		if(ERROR_SUCCESS != checkSetRoutingParamFlag)
		{
			return checkSetRoutingParamFlag;
		}
		else
		{
			return ERROR_SUCCESS;
		}
	}
	
}

/*
*		Function:
*			III_Routing
*                   依照TYPE_ROUTE_PD的不同[See Route_Manager define]呼叫行人, 大眾, 室內導航的起,終點設定
*		parameter:
*			III_RTPOINT pntDeparture - [in] 輸入起點所需資料 (鎖路後的edgeID, F,TNode經緯度)
*			III_RTPOINT pntArrival - [in] 輸入終點點所需資料 (鎖路後的edgeID, F,TNode經緯度)
*			[out] see III_Route_Manager.h error definition
*/
III_INT III_Routing(III_RTPOINT pntDeparture, III_RTPOINT pntArrival)
{
	III_INT checkRoutingFlag = 0;
	III_INT STARTTIME = 0; //FOR PERFORMANCE TEST
	III_INT ENDTIME = 0;   //FOR PERFORMANCE TEST
	III_CHAR buf[256] = ""; //FOR PERFORMANCE TEST

	if ( TYPE_ROUTE_PD == typetoroute)
	{	
		/*
		 *     Function:
		 *	        III_Pedestrian_Route
		 *				起點, 終點設定
		 *     parameter:
		 *			III_RTPOINT pntDeparture - [in] 輸入起點所需資料 (鎖路後的edgeID, F,TNode經緯度)
		 *			III_RTPOINT pntArrival - [in] 輸入終點點所需資料 (鎖路後的edgeID, F,TNode經緯度)
		 *			[out] see III_Route_Manager.h error definition
		 *
		*/
		STARTTIME = GetTickCount();	
		checkRoutingFlag = III_PD_Routing(pntDeparture, pntArrival);
		ENDTIME = GetTickCount() - STARTTIME;
		//sprintf(buf, "API_III_PD_Routing_SPEND_TIME = %d \n", ENDTIME);   
		III_fwrite(buf, 1, strlen(buf), performaceTestFile);

		if(ERROR_SUCCESS != checkRoutingFlag)
		{
			return checkRoutingFlag;
		}
		else
		{
			return ERROR_SUCCESS;
		}
	}
}

/*
*		Function:
*			III_Routing
*                   依照TYPE_ROUTE_PD的不同[See Route_Manager define]呼叫行人, 大眾, 室內導航的A*演算法並將結果(PathID & Path distance)儲存在III_RESULTPDRT結構裡
*		parameter:
*			III_RESULTPDRT *resultPDRT - [in] routing結果(pathID & path distance)儲存在III_RESULTPDRT結構裡
*			[out] see III_Route_Manager.h error definition
*/
III_INT III_GetPdRTResult(III_RESULTPDRT *resultPDRT)
{
	III_INT checkPdRTFlag = 0;
	
	if ( TYPE_ROUTE_PD == typetoroute)
	{
		/*
		 *	Function:
		 *	     III_PD_GetPdRTResult 
		 *			開始執行A* 演算法並將routing結果(pathID & path distance)儲存在III_RESULTPDRT結構裡
		 *	Parameter:
		 *		 III_RESULTPDRT *resultPDRT - [in] routing結果(pathID & path distance)儲存在III_RESULTPDRT結構裡
		 *		 [out] see III_Route_Manager.h error definition
		*/
		checkPdRTFlag = III_PD_GetPdRTResult(resultPDRT);
	
		if(ERROR_SUCCESS != checkPdRTFlag)
		{
			return checkPdRTFlag;
		}
		else
		{
			return ERROR_SUCCESS;
		}
		
	}
}


//III-Route Method

III_INT GetNearEdge(III_INT X, III_INT Y, III_RTPOINT *SEPoint)
{
	III_INT dist = 0;
	III_INT range = 0;
	III_INT mx;
	III_INT my;
	III_INT modD = 0x7fffffff;
	struct ListStreet stList;
	struct MdbStreet *cur;
	SRect rect;

	stList.stHead = stList.stTail = NULL;
	stList.count = 0;

	while (0 == stList.count && range<=10000)
	{
		range += 500;
		rect.x_max = X + range;
		rect.x_min = X - range;
		rect.y_max = Y + range;
		rect.y_min = Y - range;
		readStreetData(&stList, rect);
	}
	
	for(cur=stList.stHead; cur ; cur= cur->next)
	{
		dist = ComputeDist(X, Y, cur->m_ptsX, cur->m_ptsY, cur->m_ptNum, &mx, &my);
		
		if(dist < modD)
		{
			modD = dist;

			SEPoint->nLineSegIdxFix = cur->m_sid;
			SEPoint->pntFix.longitude = mx;
			SEPoint->pntFix.latitude = my;
			SEPoint->pntFPoint.longitude = cur->m_ptsX[cur->m_ptNum - 1];	//Fix StartPath Tx(TNode 經度)
			SEPoint->pntFPoint.latitude = cur->m_ptsY[cur->m_ptNum - 1];	//Fix StartPath Ty(TNode 緯度)
			SEPoint->pntTPoint.longitude = cur->m_ptsX[0];					//Fix StartPath Fx(FNode 經度)
			SEPoint->pntTPoint.latitude = cur->m_ptsY[0];					//Fix StartPath Fy(FNode 緯度)
		}
	}

	return ERROR_SUCCESS;
}

III_INT ComputeDist(III_INT x, III_INT y, III_INT *X, III_INT *Y, III_INT n, III_INT *mx, III_INT *my)
{
	//dot:a dot b, dis:dot/square(length of vector a)
	//D:shortest-dist from p to edge(p1,p2), *minD:shortest-dist overall
	//a:vector of p2-p1, b:vector of p-p1
	//利用空間向量求最短距離
	III_DOUBLE dis, dot, D, minD = (III_DOUBLE)(0x7fffffff), a1, a2, b1, b2;
	III_INT i, tx, ty;

	if (n<2) 
		return 0x7fffffff;

	for (i=0;i<n-1;i++)
	{
		a1 = X[i+1] - X[i];
		a2 = Y[i+1] - Y[i];
		b1 = x - X[i];
		b2 = y - Y[i];
		dot = a1*b1 + a2*b2;
		dis = dot/(a1*a1+a2*a2);

		if(dot<=0.0f)
		{ //D: dist of p to p1
			D = (III_FLOAT)(x-X[i])*(x-X[i])+(III_FLOAT)(y-Y[i])*(y-Y[i]);
			tx = X[i];
			ty = Y[i];
		}
		else if(dis*dot>=a1*a1+a2*a2)
		{ //D: dist of p to p2
			D = (III_FLOAT)(x-X[i+1])*(x-X[i+1])+(III_FLOAT)(y-Y[i+1])*(y-Y[i+1]);
			tx = X[i+1];
			ty = Y[i+1];
		}
		else
		{ //D: dist between p and the projection of p on edge(p1,p2)
			tx = X[i] + (III_INT)(dis*a1);
			ty = Y[i] + (III_INT)(dis*a2);
			D = (III_FLOAT)(tx-x)*(tx-x)+(III_FLOAT)(ty-y)*(ty-y);
		}

		if(D<minD)
		{
			minD = D;
			*mx = tx;
			*my = ty;
		}
	} //end of for loop

	return ((III_INT)sqrt(minD));
}

III_INT readpoints(III_UCHAR *buf, III_INT Ptr, III_USHORT poNum, III_INT * ptsX, III_INT * ptsY) 
{
	III_SHORT dx, dy;
	III_INT bx, by, ebx, eby, i;

	memcpy ( ptsX, buf + Ptr, 4 );
	Ptr += 4;
	memcpy ( ptsY, buf + Ptr, 4 );
	Ptr += 4;
	bx = ptsX[0];
	by = ptsY[0];

	for ( i = 1 ; i < poNum ; i++ )
	{
	    memcpy ( &dx, buf + Ptr, 2 );
	    Ptr += 2;

	    if ( dx == 32767 )
	    {
			memcpy ( &ebx, buf + Ptr, 4 );
			Ptr += 4;
			ptsX[i] = bx + ebx;
		}
		else
		{
			ptsX[i] = bx + dx;
		}

		memcpy ( &dy, buf + Ptr, 2 );
		Ptr += 2;

		if ( dy == 32767 )
		{
			memcpy ( &eby, buf + Ptr, 4 );
			Ptr += 4;
			ptsY[i] = by + eby;
		} 
		else
		{
			ptsY[i] = by + dy;
		}

		bx = ptsX[i];
		by = ptsY[i];
	}
	return Ptr;
}

III_INT GetStData(struct SObjData *objelem , III_FILE *pdtfd, struct MdbStreet ** stRecord)
{
	III_UCHAR *buf;
	III_INT Ptr = 0;
	III_UINT stAttr[2];
	III_UINT rId = 0;
	III_USHORT poNum = 0;
	III_USHORT rNumLen = 0;

	buf = (III_UCHAR *)III_malloc ( objelem->size );

	if(NULL == pdtfd)
	{
		printf("ERROR_ Can't Load pdtfd files");
	}
	III_fseek ( pdtfd, objelem->objPOS, SEEK_SET ); //取得資料所在位置
	III_fread ( buf, objelem->size, 1, pdtfd );

	memcpy ( &stAttr[0], buf, 4 );
	memcpy ( &stAttr[1], buf + 4, 4 );

	Ptr += 8;
	rId = stAttr[0] >> 4;							//道路ID

	//讀出路段屬性(轉檔寫入為ushort)
	poNum = stAttr[1] >> 16;						//路段座標點數

	rNumLen = stAttr[1] & 0xf;						//道路番號長度
	
	(*stRecord)->m_ptNum = poNum;					//設定座標點數
	
	(*stRecord)->m_sid = objelem->objID;			//設定物件id

	memcpy ((*stRecord)->m_rNumName, (III_CHAR*)(buf + Ptr), rNumLen);

	(*stRecord)->m_rNumName[rNumLen] = '\0';		//設定道路番號
	Ptr += rNumLen;

	(*stRecord)->m_ptsX = (III_INT *)III_malloc ( sizeof ( III_INT ) * poNum );
	(*stRecord)->m_ptsY = (III_INT *)III_malloc ( sizeof ( III_INT ) * poNum );

	Ptr = readpoints(buf, Ptr, poNum, ((*stRecord)->m_ptsX), ((*stRecord)->m_ptsY));

	(*stRecord)->m_countryID = 1;					//設定國家ID
	(*stRecord)->m_rid = rId;

	III_free ( buf );

	return ERROR_SUCCESS;
}

III_VOID QueryStreetByID(III_UINT sid, III_FILE *idxfd, III_FILE *pdtfd, struct MdbStreet ** stobj)
{
	//將傳入sid建成linking list結構
	III_INT tarr[2];
	struct SObjData *objelem = (struct SObjData *)III_malloc ( sizeof ( struct SObjData ) );
	objelem->objID = sid;
	objelem->next = III_NULL;

	if(NULL == idxfd)
	{
		printf("ERROR_ Can't Load rtxfd files");
	}
	III_fseek ( idxfd, objelem->objID << 2, 0 );
	III_fread ( tarr, 4, 2, idxfd );
	
	objelem->objPOS = tarr[0];
	objelem->size = tarr[1] - tarr[0];

	//取得street物件資料
	GetStData (  objelem, pdtfd, stobj );
	III_free ( objelem );
}

III_VOID initialMdbStreet(struct MdbStreet *st) 
{
	st->m_sid = 0;
	st->m_countryID = 0;
	st->m_ptNum = 0;
	st->m_ptsX = III_NULL;
	st->m_ptsY = III_NULL;
	st->m_rid = 0;
	st->next = III_NULL;
}

III_VOID freeSt(struct MdbStreet ** head) 
{
	struct MdbStreet *temp;

	while ( *head ) 
	{
	    temp = (*head)->next;

	    III_free ((*head)->m_ptsX);
	    III_free ((*head)->m_ptsY);

		III_free ( ( *head) );
		( *head) = temp;
	}
}

III_VOID quicksort(III_INT *arr, III_INT left, III_INT right) 
{
	III_INT l = left, r = right, mid, mpos, pivot, temp;

	if ( l >= r )
	    return;

	mpos = ( l + r ) >> 1;
	pivot = arr[mpos];
	temp = arr[l];
	arr[l] = arr[mpos];
	arr[mpos] = temp;

	while ( l < r ) 
	{
	    while ( arr[l] <= pivot && l < right )
		{
			l++;
		}

	    while ( arr[r] > pivot && r >= left )
		{
			r--;
		}

	    if ( l < r ) 
		{
			temp = arr[l];
			arr[l] = arr[r];
			arr[r] = temp;
		}
	}
	mid = r;
	temp = arr[left];
	arr[left] = arr[mid];
	arr[mid] = temp;

	quicksort ( arr, left, mid - 1 );
	quicksort ( arr, mid + 1, right );
}



III_INT Overlap(SRect *Rect1, SRect *Rect2) 
{
	return ( !( Rect1->x_min > Rect2->x_max || Rect1->y_min > Rect2->y_max || Rect2->x_min > Rect1->x_max
			|| Rect2->y_min > Rect1->y_max ) );
}

III_INT * GetObjID2(SRect *rect, III_INT *count, III_FILE *rtxfd)
{
	
	III_BOOL flag = 1;
	SR_Record *sr = (SR_Record *)III_malloc ( sizeof ( SR_Record ) * 6 );
	III_INT n[6];
	III_INT lvl = 0;
	III_INT *arr2;
	III_INT c = 0;
	III_INT **arr = (III_INT**)III_malloc(sizeof(void*)*256); 
	III_INT t1;
	III_INT t2;
	III_SHORT maxL;

	if(NULL == rtxfd)
	{
		printf("ERROR_ Can't Load rtxfd files");
	}

	III_fseek ( rtxfd, 0, SEEK_SET );
	III_fread ( &sr[0], 1, 1024, rtxfd );
	maxL = sr[0].Level;
	memset ( n, 0, 24 );
	
	while ( flag )
	{
		if ( sr[lvl].Level == 0 )
		{
			if ( Overlap ( rect, &sr[lvl].Entry[n[lvl]].Rect ) )
			{
				t1 = c >> 9;  //2^9 = 512
				t2 = c % 512; //allowed max = 256*512 = 128K items

				if ( t2 == 0 )
				{
					arr[t1] = (III_INT *)III_malloc ( sizeof ( III_INT ) * 512 );
				}
				arr[t1][t2] = sr[lvl].Entry[n[lvl]].Child;
				c++;
			}
			n[lvl]++;
		}
		else
		{
			if ( Overlap ( rect, &sr[lvl].Entry[n[lvl]].Rect ) )
			{
				if(NULL == rtxfd)
				{
					printf("ERROR_ Can't Load rtxfd files");
				}
				III_fseek ( rtxfd, sr[lvl].Entry[n[lvl]].Child * 1024, SEEK_SET ); //remove 1024 for new rtx format
				III_fread ( &sr[++lvl], 1, 1024, rtxfd );
				n[lvl] = 0;
			}
			else
			{
				n[lvl]++;
			}
		}

		while ( sr[lvl].EntCount == n[lvl] && flag )
		{
			if ( n[0] == sr[0].EntCount )
			{
				flag = III_FALSE;
			}
			else
			{
				n[--lvl]++;
			}
		}
	}
	
	*count = c;
	t1 = ( c - 1 ) >> 9;
	arr2 = (III_INT *)III_malloc ( sizeof ( III_UINT ) * ( t1 + 1 ) * 512 );

	for ( c = 0 ; c <= t1 ; c++ )
	{
		memcpy ( arr2 + c * 512, arr[c], sizeof ( III_UINT ) * 512 );
		III_free ( arr[c] );
	}
	III_free ( sr );
	III_free ( arr );

	quicksort ( arr2, 0, *count - 1 );

	return arr2;
}

//Road Lock
III_VOID readStreetData(struct ListStreet *stList, SRect rect)
{
	struct MdbStreet *cur = stList->stHead;
	struct MdbStreet *temp = NULL;
	struct MdbStreet *prev = NULL;
	
	III_INT i = 0;
	int count ;
	int c = 0;
	III_UINT *arr = NULL;
	SRect tmp;

	struct SFileSet fileSet;
	fileSet.idxfd = fileSet.pdtfd = fileSet.rtxfd = NULL;

	
	memcpy ( &tmp, &rect, 16 );	

	freeSt ( &stList->stHead );
	stList->count = 0;
	cur = stList->stHead = NULL;

	fileSet.rtxfd = III_fopen ( "\\Storage Card\\III_RouteTable\\routeTable-rd.rtx", "rb" );
	fileSet.idxfd = III_fopen ( "\\Storage Card\\III_RouteTable\\routeTable-rd.idx", "rb" );
	fileSet.pdtfd = III_fopen ( "\\Storage Card\\III_RouteTable\\routeTable-rd.pdt", "rb" );
	
	if(NULL == fileSet.rtxfd || NULL == fileSet.idxfd || NULL == fileSet.pdtfd)
	{
		printf("ERROR_ Can't Load Road files");
		return;
	}

	arr = (III_UINT *)GetObjID2 ( &rect, &count, fileSet.rtxfd);
	c = 0;

	while (cur)
	{
			if ( prev )
			{
				prev->next = cur->next;
			}

			temp = cur;
			cur = cur->next;

			III_free ( temp->m_ptsX );
			III_free ( temp->m_ptsY );
			III_free ( temp );
			stList->count--;
	}

	while ( c != count && cur)
	{
			if ( cur->m_sid == arr[c] )
			{ //hit
				if ( prev )
				{
					prev->next = cur;
				}
				else
				{
					stList->stHead = prev = cur;
				}
				prev = cur;
				cur = cur->next;
				c++;
			}
			else if ( cur->m_sid < arr[c] )
			{ //delete
				if ( prev )
				{
					prev->next = cur->next;
				}
				temp = cur;
				cur = cur->next;
				III_free ( temp->m_ptsX );
				III_free ( temp->m_ptsY );
				III_free ( temp );
				stList->count--;
			}
			else
			{ //insert
				temp = (struct MdbStreet *)III_malloc ( sizeof ( struct MdbStreet ) );
				initialMdbStreet(temp);
				QueryStreetByID ( arr[c++], fileSet.idxfd, fileSet.pdtfd, &temp);

				if ( prev )
				{
					prev = prev->next = temp;
				}
				else
				{
					stList->stHead = prev = temp;
				}

				temp->next = cur;
				stList->count++;
			}
	}

	if ( count - c > 0 )
	{
			stList->count += ( count - c );

		    for ( ; c < count ; c++ )
			{
				temp = (struct MdbStreet *)III_malloc ( sizeof ( struct MdbStreet ) );
				initialMdbStreet(temp);
				QueryStreetByID ( arr[c], fileSet.idxfd, fileSet.pdtfd, &temp);

				if ( prev )
				{
					prev->next = temp;
					prev = prev->next;
				}
				else
				{
					stList->stHead = prev = temp;
				}
			}
	}

	while ( cur)
	{
			temp = cur;
			cur = cur->next;
			III_free ( temp->m_ptsX );
			III_free ( temp->m_ptsY );
			III_free ( temp );

			stList->count--;
	}

	if ( prev )
	{
		prev->next = cur;
	}

		III_fclose ( fileSet.idxfd );
		III_fclose ( fileSet.pdtfd );
		III_fclose ( fileSet.rtxfd );
		III_free ( arr );
	
	while ( cur )
	{
			temp = cur;
			cur = cur->next;
			III_free ( temp->m_ptsX );
			III_free ( temp->m_ptsY );
			III_free ( temp );
			stList->count--;
	}
	

	if ( prev )
	{
	    prev->next = III_NULL;
	}

	if ( stList->count == 0 )
	{
		stList->stHead = III_NULL;
	}
}


