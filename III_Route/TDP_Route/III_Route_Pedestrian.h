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
III_INT III_PD_GetPdRTResult(III_RESULTPDRT*);


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