/*
*	CopyRight : ESTI_資策會新興智慧所
*	Description : III_Pedestrian-Route Route內部介面API
*	Version & Date : v0.3  2010/5/24
*   AUTHOR : 資策會 凌家強
*/
#include "III_Route_Manager.h"
#include "III_Route_Pedestrian.h"
#include "Route_Astar.h"

Astar *rm = NULL;

//FOR PERFORMANCE TEST
extern III_FILE *performaceTestFile;

/*
*     Function:
*	        III_Pedestrian_Route 
*				初始化(RouteTable的開啟及讀檔, 初始化Route模式和所需的資料結構和快取記憶體)
*     parameter:
*			III_RTPARAM *paramRouting - [in] PD_Route 所需的資訊(ex. routetable name and number , typeRouting, modeRouting )
*			 [out] see III_Route_Manager.h error definition
*
*/
III_INT III_PD_SetRoutingParam(III_RTPARAM *paramRounting)
{
	III_INT checkAstarInitialFlag = 0;
	III_INT STARTTIME = 0;
	III_INT ENDTIME = 0;
	III_CHAR buf[256] = "";

	//資策會Route API
	STARTTIME = GetTickCount();
	if(NULL == rm)
	{
		rm = (Astar *)III_malloc(sizeof(Astar));
	}
	/*
	*	Function:
	*	    III_Pedestrian_Route
	*			初始化(RouteTable的開啟及讀檔, 初始化Route模式和所需的資料結構和快取記憶體)
	*	parameter:
	*	     Astar *a - [in] Astar struct (see Route_Astar.h)
	*	     [out] see III_Route_Manager.h error definition
	*/
	checkAstarInitialFlag = AstarInit(rm, paramRounting);
	ENDTIME = GetTickCount() - STARTTIME;
	//sprintf(buf, "API_AstarInit_SPEND_TIME = %d \n", ENDTIME);   
	III_fwrite(buf, 1, strlen(buf), performaceTestFile);

	if(ERROR_SUCCESS != checkAstarInitialFlag)
	{
		return checkAstarInitialFlag;
	}
	else
	{
		return ERROR_SUCCESS;
	}
}

/*
*     Function:
*	        III_Pedestrian_Route
*				起點, 終點設定
*     parameter:
*			III_RTPOINT pntDeparture - [in] 輸入起點所需資料 (鎖路後的edgeID, F,TNode經緯度)
*			III_RTPOINT pntArrival - [in] 輸入終點點所需資料 (鎖路後的edgeID, F,TNode經緯度)
*			 [out] see III_Route_Manager.h error definition
*
*/
III_INT III_PD_Routing(III_RTPOINT pntDeparture, III_RTPOINT pntArrival)
{
	III_INT checkStartFlag = 0; //StartPoint Check Flag	
	III_INT checkEndFlag = 0;   //EndPoint Check Flag
	III_INT STARTTIME = 0;
	III_INT ENDTIME = 0;
	III_CHAR buf[256] = "";

	rm->misc.eid = pntDeparture.nLineSegIdxFix;
	
	/*
	*	Function:
	*	    III_Pedestrian_Route
	*			起點設定
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
	STARTTIME = GetTickCount();
	checkStartFlag = AstarSetStart(rm, pntDeparture.pntFix.longitude, pntDeparture.pntFix.latitude, pntDeparture.pntFPoint.longitude, pntDeparture.pntFPoint.latitude, pntDeparture.pntTPoint.longitude, pntDeparture.pntTPoint.latitude);
	ENDTIME = GetTickCount() - STARTTIME;
	//sprintf(buf, "API_AstarSetStart_SPEND_TIME = %d \n", ENDTIME);   
	III_fwrite(buf, 1, strlen(buf), performaceTestFile);

	if(0 != checkStartFlag)
	{
		return checkStartFlag;
	}

	rm->misc.eid = pntArrival.nLineSegIdxFix;	
	/*
	*	Function:
	*	    III_Pedestrian_Route 
	*			終點設定
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
	STARTTIME = GetTickCount();
	checkEndFlag = AstarSetEnd(rm, pntArrival.pntFix.longitude, pntArrival.pntFix.latitude, pntArrival.pntFPoint.longitude, pntArrival.pntFPoint.latitude, pntArrival.pntTPoint.longitude, pntArrival.pntTPoint.latitude);
	ENDTIME = GetTickCount() - STARTTIME;
	//sprintf(buf, "API_AstarSetEnd_SPEND_TIME = %d \n", ENDTIME);   
	III_fwrite(buf, 1, strlen(buf), performaceTestFile);

	if(0 != checkEndFlag)
	{
		return checkEndFlag;
	}
	else
	{
		return 0;
	}
	
}

/*
*	Function:
*	     III_PD_GetPdRTResult 
*			開始執行A* 演算法並將routing結果(pathID & path distance)儲存在III_RESULTPDRT結構裡
*	Parameter:
*		 III_RESULTPDRT *resultPDRT - [in] routing結果(pathID & path distance)儲存在III_RESULTPDRT結構裡
*		 [out] see III_Route_Manager.h error definition
*/
III_INT III_PD_GetPdRTResult(III_RESULTPDRT *resultPDRT)
{
	III_INT astarRoutingCheckFlag = 0;
	III_INT STARTTIME = 0;				//FOR PERFORMANCE TEST
	III_INT ENDTIME = 0;				//FOR PERFORMANCE TEST
	III_CHAR buf[256] = "";				//FOR PERFORMANCE TEST

	/*
	*	Function:
	*	    III_Pedestrian_Route 
	*			A* 演算法及權重設定
	*	parameter:
	*	    Astar *a - [in] Astar struct (see Route_Astar.h)
	*		III_RESULTPDRT *resultPDRT - [in] III_PD_RouteOutStruct
	*		[out] see III_Route_Manager.h error definition
	*	
	*/
	STARTTIME = GetTickCount();
	astarRoutingCheckFlag = AstarRouting(rm, resultPDRT);
	ENDTIME = GetTickCount() - STARTTIME;
	//sprintf(buf, "API_AstarRouting_SPEND_TIME = %d \n", ENDTIME);   
	III_fwrite(buf, 1, strlen(buf), performaceTestFile);
	sprintf(buf, "執行A* 演算法並得到一條Path所花時間 = %d 毫秒 \n\n", ENDTIME);   
	III_fwrite(buf, 1, strlen(buf), performaceTestFile);
	/*
	*	Function:
	*	    測試III_Pedestrian_Route合理性及驗證結果，並將結果輸出在 path.txt
	*	parameter:
	*	     Astar *a - [in] Astar struct (see Route_Astar.h)
	*		 III_RESULTPDRT *resultPDRT - [in] III_PD_RouteOutStruct
	*	
	*/
	//Test(rm, resultPDRT);

	return astarRoutingCheckFlag;
}
