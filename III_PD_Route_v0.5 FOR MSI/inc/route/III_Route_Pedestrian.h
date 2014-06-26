/*
*	CopyRight : ESTI_資策會新興智慧所
*	Description : III_Pedestrian-Route Route內部介面API
*	Version & Date : v0.3  2010/5/24
*   AUTHOR : 資策會 凌家強
*/
#ifndef __ROUTE_PEDESTRIAN_H__
#define __ROUTE_PEDESTRIAN_H__

#include "III_Route_Manager.h"

/*
*    Constant/Macro/Type Definitions
*/

/*
*    Variable Declarations
*/

/*
*    Function Declarations
*/

/*
* Function:
* Parameters:
* Return value:
*/

#if defined(__cplusplus)
extern "C" {
#endif
	

	//Road Lock inner method ///////////////////////////////////////////////////////////////////////////////////
	//Caculate mini-square method
	III_INT ComputeDist(III_INT, III_INT, III_INT*, III_INT*, III_INT, III_INT*, III_INT*);
	//Output real x,y
	III_INT readpoints(III_UCHAR *buf, III_INT Ptr, III_USHORT poNum, III_INT * ptsX, III_INT * ptsY); 
	//Get Street Data
	III_INT GetStData(struct SObjData *objelem, III_FILE *pdtfd, struct MdbStreet ** stRecord);
	//Use Eid Query Street Data
	III_VOID QueryStreetByID( III_UINT sid, III_FILE *idxfd, III_FILE *pdtfd, struct MdbStreet ** stobj);
	//Initial Struct Street
	III_VOID initialMdbStreet(struct MdbStreet *st) ;
	//Free Struct Street Data
	III_VOID freeSt(struct MdbStreet ** head); 
	//quick-sort method
	III_VOID quicksort(III_INT *arr, III_INT left, III_INT right);
	//R-Tree Overlap part
	III_INT Overlap(SRect *Rect1, SRect *Rect2); 

	//GetTreeObject
	III_INT * GetObjID2(SRect *rect, III_INT *count, III_FILE *rtxfd);
	III_VOID readStreetData(struct ListStreet *stList, SRect rect);
/*
*     Function:
*	        III_Pedestrian_Route初始化(RouteTable的開啟及讀檔, 初始化Route模式和所需的資料結構和快取記憶體)
*     parameter:
*			III_RTPARAM *paramRouting - [in] PD_Route 所需的資訊(ex. routetable name and number , typeRouting, modeRouting )
*			 [out] see III_Route_Manager.h error definition
*/
III_INT III_PD_SetRoutingParam(III_RTPARAM*);

/*
*     Function:
*	        III_Pedestrian_Route起點, 終點設定
*     parameter:
*			III_RTPOINT pntDeparture - [in] 輸入起點所需資料 (鎖路後的edgeID, F,TNode經緯度)
*			III_RTPOINT pntArrival - [in] 輸入終點點所需資料 (鎖路後的edgeID, F,TNode經緯度)
*			 [out] see III_Route_Manager.h error definition
*
*/
III_INT III_PD_Routing(III_RTPOINT pntDeparture, III_RTPOINT pntArrival);


/*
*	Function:
*	     III_PD_GetPdRTResult 
*			開始執行A* 演算法並將routing結果(pathID & path distance)儲存在III_RESULTPDRT結構裡
*	Parameter:
*		 III_RESULTPDRT *resultPDRT - [in] routing結果(pathID & path distance)儲存在III_RESULTPDRT結構裡
*		 [out] see III_Route_Manager.h error definition
*/
III_INT III_PD_GetPdRTResult(III_RESULTPDRT*);

#if defined(__cplusplus)
}
#endif

#endif