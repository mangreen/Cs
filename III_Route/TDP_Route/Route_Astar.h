/*
*	CopyRight : ESTI_資策會新興智慧所
*	Description : III_Pedestrian-Route Route內部 & 對外介面API 及 結構定義
*	Version & Date : v0.3  2010/5/24
*/
#ifndef __ROUTE_ASTAR_H__
#define __ROUTE_ASTAR_H__

#include <stdio.h>
#include <math.h>
#include "platform.h"
#include "prototype.h"
#include "Cache.h"
#include "III_Route_Manager.h"

#define MAXOPENLIST		(4096) 
#define LENGTHM			(256)	//number of 1D array of Node2 **list
#define LENGTHN			(1024)	//the length of each 1D array of Node2 **list
#define ADJSIZE			(16)

typedef struct _Edge { //for openlist
	III_UINT eid; //edgeID
	III_UINT pid; //parent edgeID
	III_INT Pg; //edge的距離(PAPAGO g)
	III_INT g; //edge的距離(與終點的距離差)
	III_UINT off; //direction
	III_INT x, y; //經緯度
	III_INT f; //edge與edge之間的直角距離
}Edge;

typedef struct _Edge2 { //for closelist
	III_UINT eid; //edgeID
	III_UINT pid; //parent edgeID
	III_UINT dist; //edge distance
}Edge2;

typedef struct _Area { //for 國別 or 區域 
	III_INT off1;   //至Full Data的起始位置
	III_INT maxEdge; //此國別or區域 edge的總數
	III_INT active; //是否初始化過
	III_FILE *fp; //RouteTable I/O 檔案指標
}Area;

typedef struct _Misc {
	III_INT fx, fy; //fnode經緯度
	III_INT tx, ty; //tnode經緯度
	III_INT mx, my; //鎖路後的經緯度
	III_UINT eid;   //edgeID
	III_UCHAR cid; //country id
}Misc;

typedef struct _AdjNode {
	III_UINT a; //a={nid off/4(25), num(3), reserved(4) }
	III_UINT b; //b={eid(24), reserved(8)}
	III_SHORT dx,dy; //dx, dy
	III_USHORT dist,type; //distance, type (無障礙空間, 避雨路徑, 最短路徑)
}AdjNode;

typedef struct _OpenList {
	III_INT limit; //OpenList 最大容納個數
	III_INT count; //OpenList的個數
	Edge *heap; //save Edge struct
}OpenList;

typedef struct _CloseList {
	III_INT count; //CloseList的個數
	III_CHAR **visitEdge; //是否為拜訪過
	Edge2 **list; //save Edge2 struct 
}CloseList;

typedef struct _Astar {
	III_INT mode; //Route模式設定
	III_INT HW; //為一固定浮點數，加快Route結果
	III_INT distG; //Total distance 
	III_INT stopFlag; //Do~Loop flag until Route have Result
	III_UINT offs[2]; //offs[0],offs[1]:record edge direction
	Misc start, end, misc; //see Misc struct
	Area *area; //see Area struct
	OpenList open; //see OpenList struct
	CloseList close; //see CloseList struct
	Cache *c1; //see Cache struct (Cache.h)
}Astar;

//行人導航動態Weight設定
enum PD_ROUTE_TYPE
{
	SIDEWALK = 0x00,
	SHELTER = 0x01,
	ACCESSABLE = 0x02,
	STAIR = 0x04,
	MAX_PD_TYPE = 8,
};

#ifdef __cplusplus
extern "C" {
#endif

/*
*	Function:
*	    測試III_Pedestrian_Route合理性及驗證結果，並將結果輸出在 path.txt
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		 III_RESULTPDRT *resultPDRT - [in] III_PD_RouteOutStruct
*	
*/	
III_INT Test(Astar* , III_RESULTPDRT *); //for testing

/*
*	Function:
*	    III_Pedestrian_Route初始化(RouteTable的開啟及讀檔, 初始化Route模式和所需的資料結構和快取記憶體)
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		[out] see III_Route_Manager.h error definition
*/
III_INT AstarInit(Astar*, III_RTPARAM*);

/*
*	Function:
*		重新路徑規劃時，可以呼叫此API清除Astar結構
*	Parameter:
*		Astar *a - [in] Astar struct (see Route_Astar.h)
*		[out] see III_Route_Manager.h error definition
*/
III_INT AstarClear(Astar*); 

/*
*	Function:
*		系統結束時，可以呼叫此API清除Astar結構
*	Parameter:
*		Astar *a - [in] Astar struct (see Route_Astar.h)
*		 [out] see III_Route_Manager.h error definition
*/
III_INT AstarFinish(Astar*); 

/*
*	Function:
*	    III_Pedestrian_Route Astar 演算法及權重設定
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		 III_RESULTPDRT *resultPDRT - [in] III_PD_RouteOutStruct
*		 [out] see III_Route_Manager.h error definition
*	
*/
III_INT AstarRouting(Astar* , III_RESULTPDRT*); 

/*
*	Function:
*	    III_Pedestrian_Route 起點設定
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		 PointX - [in] 鎖路後的經度
*		 PointY - [in] 鎖路後的緯度
*		 Fx - [in] 鎖路後的FNode經度
*		 Fy - [in] 鎖路後的FNode緯度
*		 Tx - [in] 鎖路後的TNode經度
*		 Tx - [in] 鎖路後的TNode緯度
*		 [out] see III_Route_Manager.h error definition
*	
*/
III_INT AstarSetStart(Astar *, III_INT , III_INT , III_INT , III_INT , III_INT , III_INT ); 

/*
*	Function:
*	    III_Pedestrian_Route 終點設定
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		 PointX - [in] 鎖路後的經度
*		 PointY - [in] 鎖路後的緯度
*		 Fx - [in] 鎖路後的FNode經度
*		 Fy - [in] 鎖路後的FNode緯度
*		 Tx - [in] 鎖路後的TNode經度
*		 Tx - [in] 鎖路後的TNode緯度
*		 [out] see III_Route_Manager.h error definition
*	
*/
III_INT AstarSetEnd(Astar *, III_INT , III_INT , III_INT , III_INT , III_INT , III_INT ); 

/*
*	Function:
*	    III_Pedestrian_Route Astar 權重設定
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		  [out] see III_Route_Manager.h error definition
*	
*/
III_INT AstarSetWeight(Astar *);


//inner API

/*
*	Function:
*	    記錄起點與終點的edge的左,右端經緯度, 國別or區域, 鎖路後的點
*	parameter:
*	     Misc *a - [in] Record Edge Info (see Route_Astar.h)
*		 PointX - [in] 鎖路後的經度
*		 PointY - [in] 鎖路後的緯度
*		 Fx - [in] edge的左端經度
*		 Fy - [in] edge的左端緯度
*		 Tx - [in] edge的右端經度
*		 Ty - [in] edge的右端緯度
*		 [out] see III_Route_Manager.h error definition
*/
III_INT SaveEdgeInfo(Misc*, III_INT, III_INT, III_INT, III_INT, III_INT, III_INT);

/*
*	Function:
*	    III_Pedestrian_Route 清除Route所需資料結構內容
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		 [out] see III_Route_Manager.h error definition
*/
III_INT AstarClean(Astar*);

/*
*
*	Function:
*		加入到CloseList裡，並將Flag設定為拜訪過
*	parameter:
*		CloseList - [in] CloseList struct (see Route_Astar.h)
*		e - [in] EdgeID
*        [out] see III_Route_Manager.h error definition
*/
III_INT SetVisit(CloseList*, Edge*);

/*
*
*	Function:
*		檢查是否已在CloseList裡
*	parameter:
*		CloseList - [in] CloseList struct (see Route_Astar.h)
*		cId - [in] 國別 or 區域 ID
*		e - [in] EdgeID
*       [out] True : 已在CloseList裡, False : 不在CloseList裡
*/
III_INT IsVisit(CloseList*, III_UCHAR, III_INT);

/*
*
*	Function:
*		Pop在OpenList裡的Edge (權重最輕的在頂端)
*	parameter:
*		OpenList - [in] OpenList struct (see Route_Astar.h)
*		[out] 如果在OpenList裡，則回傳Edge struct，不成功則回傳NULL
*/
Edge OpenlistPop(OpenList*);

/*
*
*	Function:
*		start of OpenList's member function - Stack push
*		Min-Heap tree algorithm
*		Push的演算法為將edge push進stack時，
*		會將stack裡的edge做f值(f=g+h)的比較，如果f值越大的元素，則越靠近stack的頂端。
*	parameter:
*		OpenList - [in] OpenList struct (see Route_Astar.h)
*		Edge - [in] Edge struct (see Route_Astar.h)
*		[out] see III_Route_Manager.h error definition
*/
III_INT OpenlistPush(OpenList*, Edge);

/*
*	Function:
*	    III_Pedestrian_Route Astar 演算法主程式
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		 III_RESULTPDRT *resultPDRT - [in] III_PD_RouteOutStruct
*		 [out] see III_Route_Manager.h error definition
*	
*/
III_INT RunAstar2(Astar* , III_RESULTPDRT* );

/*
*
*	Function:
*		檢查在OpenList裡是否有此Edge
*	parameter:
*		OpenList - [in] OpenList struct (see Route_Astar.h)
*		e - [in] EdgeID
*		[out] 如果在OpenList裡，則回傳Edge struct，不成功則回傳NULL
*/
III_INT InOpenlist(OpenList*, III_UINT);

/*
*	Function:
*	    III_Pedestrian_Route 清除CloseList內容
*	parameter:
*	    CloseList - [in] CloseList struct (see Route_Astar.h)
*		Area - [in] Area struct (see Route_Astar.h)
*       [out] see III_Route_Manager.h error definition
*/
III_INT FreeCloselist(CloseList*, Area*);

/*
*
*	Function:
*		當Route結束時，取得CloseList裡的內容
*	parameter:
*		CloseList - [in] CloseList struct (see Route_Astar.h)
*        III_RESULTPDRT *resultPDRT - [in] III_PD_RouteOutStruct
*		[out] see III_Route_Manager.h error definition
*/
III_INT PGetPath(CloseList*, III_RESULTPDRT*);

/*
*
*	Function:
*		重新排列OpenList裡的順序
*	parameter:
*		OpenList - [in] OpenList struct (see Route_Astar.h)
*		H1 - [in] weight 1
*		H2 - [in] weight 2
*		[out] see III_Route_Manager.h error definition
*/
III_INT RearrangeOpen(OpenList*, III_INT, III_INT);


#ifdef __cplusplus
}
#endif


#endif
