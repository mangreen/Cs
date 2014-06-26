/*
*	CopyRight : ESTI_資策會新興智慧所
*	Description : III_Pedestrian-Route Route 對外介面API 
*	Version & Date : v0.3  2010/5/24
 */
#ifndef __III_FUSION_ROUTE_API_H__
#define __III_FUSION_ROUTE_API_H__

//#include "III_Route_Manager.h"
#include "III_RouteAPI.h"

enum FUSION_ROUTE_RESULT
{
    RESULT_OUTDOOR_FIRST = 1,
	RESULT_INDOOR_FIRST = 2,
	RESULT_INDOOR_BOTH = 3,
	RESULT_INDOOR_OUTDOOR_INDOOR = 4,
};

typedef struct _III_RESULT_FUSION_RT
{
  int            resultSequence;
  III_RESULTPDRT resultPDRT;
  III_RESULTINDOOR resultIdrRT;
  III_RESULTINDOOR resultIdrRT2;
}III_RESULT_FUSION_RT;

class III_FusionRouteAPI
{
	private:
		III_RTPARAM  *GlobalparamRounting;
		III_RouteAPI *IIIRoute[3];
		III_RESULT_FUSION_RT *resultFusionRT;
		void CopyResultIdrRT(III_RESULTINDOOR TempResultIdrRT);
	public:
	    III_FusionRouteAPI();
	    ~III_FusionRouteAPI(void);	
		/*
		 *		Function:
		 *			III_SetRoutingParam
		 *                   依照TYPE_ROUTE_PD的不同[See Route_Manager define]呼叫行人, 大眾, 室內導航的RouteEngine初始化
		 *		parameter:
		 *			III_RTPARAM *paramRouting - [in] Route 所需的資訊(ex. routetable name and number , typeRouting, modeRouting )
		 *			[out] see III_Route_Manager.h error definition
		*/
		III_INT III_SetFusionRoutingParam(III_RTPARAM *paramRounting);
		
		/*
		 *		Function:
		 *			III_Routing
		 *                   依照TYPE_ROUTE_PD的不同[See Route_Manager define]呼叫行人, 大眾, 室內導航的起,終點設定
		 *		parameter:
		 *			III_POINT Departure - [in] 輸入起點所需資料 (鎖路後的edgeID, F,TNode經緯度)
		 *			III_POINT Arrival - [in] 輸入終點點所需資料 (鎖路後的edgeID, F,TNode經緯度)
		 *			[out] see III_Route_Manager.h error definition
		*/		
		III_INT III_FusionRouting(III_POINT Departure, III_POINT Arrival);
		III_INT III_OutdoorRoutingFunction(III_RouteAPI *IIIRoute, III_POINT Departure, III_POINT Arrival);
		III_INT III_IndoorRoutingFunction(III_RouteAPI *IIIRoute, III_POINT Departure, III_POINT Arrival, char *sBuildingString);
		/*
 		 *		Function:
		 *			III_Routing
		 *                   依照TYPE_ROUTE_PD的不同[See Route_Manager define]呼叫行人, 大眾, 室內導航的A*演算法並將結果(PathID & Path distance)儲存在III_RESULT_FUSION_RT結構裡
		 *		parameter:
		 *			III_RESULT_FUSION_RT *resultRT - [in] routing結果(pathID & path distance)儲存在III_RESULT_FUSION_RT結構裡
		 *			[out] see III_Route_Manager.h error definition
		*/		
		III_INT III_GetFusionRTResult(III_RESULT_FUSION_RT **resultRT);
		void DeleteIIIRouteObject(void);	
		void NewResultFusionRT(void);	
};

#endif