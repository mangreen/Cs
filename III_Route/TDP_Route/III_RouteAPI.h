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

class III_RouteAPI
{
	private:
		
	public:
        III_INT typetoroute;
        
		//Road Lock
		III_INT GetNearEdge(III_INT x, III_INT y,  III_RTPOINT * );

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
		////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		III_INT III_GetIdrRTResult(III_RESULTINDOOR *resultIdrRT);
};


#endif