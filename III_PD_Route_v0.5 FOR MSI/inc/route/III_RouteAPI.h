/*
*	CopyRight : ESTI_資策會新興智慧所
*	Description : III_Pedestrian-Route Route 對外介面API 
*	Version & Date : v0.3  2010/5/24
*/
#ifndef __III_ROUTE_API_H__
#define __III_ROUTE_API_H__

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "III_Route_Manager.h"

III_INT typetoroute;

		
/*
 *		Function:
 *			III_SetRoutingParam
 *                   依照TYPE_ROUTE_PD的不同[See Route_Manager define]呼叫行人, 大眾, 室內導航的RouteEngine初始化
 *		parameter:
 *			III_RTPARAM *paramRouting - [in] Route 所需的資訊(ex. routetable name and number , typeRouting, modeRouting )
 *			[out] see III_Route_Manager.h error definition
*/
III_INT III_SetRoutingParam(III_RTPARAM*);
		
/*
 *		Function:
 *			III_Routing
 *                   依照TYPE_ROUTE_PD的不同[See Route_Manager define]呼叫行人, 大眾, 室內導航的起,終點設定
 *		parameter:
 *			III_RTPOINT pntDeparture - [in] 輸入起點所需資料 (鎖路後的edgeID, F,TNode經緯度)
 *			III_RTPOINT pntArrival - [in] 輸入終點點所需資料 (鎖路後的edgeID, F,TNode經緯度)
 *			[out] see III_Route_Manager.h error definition
*/
III_INT III_Routing(III_RTPOINT pntDeparture, III_RTPOINT pntArrival);
		
/*
 *		Function:
 *			III_Routing
 *                   依照TYPE_ROUTE_PD的不同[See Route_Manager define]呼叫行人, 大眾, 室內導航的A*演算法並將結果(PathID & Path distance)儲存在III_RESULTPDRT結構裡
 *		parameter:
 *			III_RESULTPDRT *resultPDRT - [in] routing結果(pathID & path distance)儲存在III_RESULTPDRT結構裡
 *			[out] see III_Route_Manager.h error definition
*/
III_INT III_GetPdRTResult(III_RESULTPDRT*);

#endif