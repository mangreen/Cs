/*
*	CopyRight : ESTI_�굦�|�s�����z��
*	Description : III_Pedestrian-Route Route��������API
*	Version & Date : v0.3  2010/5/24
*   AUTHOR : �굦�| ��a�j
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
*				��l��(RouteTable���}�Ҥ�Ū��, ��l��Route�Ҧ��M�һݪ���Ƶ��c�M�֨��O����)
*     parameter:
*			III_RTPARAM *paramRouting - [in] PD_Route �һݪ���T(ex. routetable name and number , typeRouting, modeRouting )
*			 [out] see III_Route_Manager.h error definition
*
*/
III_INT III_PD_SetRoutingParam(III_RTPARAM *paramRounting)
{
	III_INT checkAstarInitialFlag = 0;
	III_INT STARTTIME = 0;
	III_INT ENDTIME = 0;
	III_CHAR buf[256] = "";

	//�굦�|Route API
	STARTTIME = GetTickCount();
	if(NULL == rm)
	{
		rm = (Astar *)III_malloc(sizeof(Astar));
	}
	/*
	*	Function:
	*	    III_Pedestrian_Route
	*			��l��(RouteTable���}�Ҥ�Ū��, ��l��Route�Ҧ��M�һݪ���Ƶ��c�M�֨��O����)
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
*				�_�I, ���I�]�w
*     parameter:
*			III_RTPOINT pntDeparture - [in] ��J�_�I�һݸ�� (����᪺edgeID, F,TNode�g�n��)
*			III_RTPOINT pntArrival - [in] ��J���I�I�һݸ�� (����᪺edgeID, F,TNode�g�n��)
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
	*			�_�I�]�w
	*	parameter:
	*	     Astar *a - [in] Astar struct (see Route_Astar.h)
	*		 PointX - [in] ����᪺�g��
	*		 PointY - [in] ����᪺�n��
	*		 Fx - [in] ����᪺FNode�g��
	*		 Fy - [in] ����᪺FNode�n��
	*		 Tx - [in] ����᪺TNode�g��
	*		 Tx - [in] ����᪺TNode�n��
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
	*			���I�]�w
	*	parameter:
	*	     Astar *a - [in] Astar struct (see Route_Astar.h)
	*		 PointX - [in] ����᪺�g��
	*		 PointY - [in] ����᪺�n��
	*		 Fx - [in] ����᪺FNode�g��
	*		 Fy - [in] ����᪺FNode�n��
	*		 Tx - [in] ����᪺TNode�g��
	*		 Tx - [in] ����᪺TNode�n��
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
*			�}�l����A* �t��k�ñNrouting���G(pathID & path distance)�x�s�bIII_RESULTPDRT���c��
*	Parameter:
*		 III_RESULTPDRT *resultPDRT - [in] routing���G(pathID & path distance)�x�s�bIII_RESULTPDRT���c��
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
	*			A* �t��k���v���]�w
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
	sprintf(buf, "����A* �t��k�ño��@��Path�Ҫ�ɶ� = %d �@�� \n\n", ENDTIME);   
	III_fwrite(buf, 1, strlen(buf), performaceTestFile);
	/*
	*	Function:
	*	    ����III_Pedestrian_Route�X�z�ʤ����ҵ��G�A�ñN���G��X�b path.txt
	*	parameter:
	*	     Astar *a - [in] Astar struct (see Route_Astar.h)
	*		 III_RESULTPDRT *resultPDRT - [in] III_PD_RouteOutStruct
	*	
	*/
	//Test(rm, resultPDRT);

	return astarRoutingCheckFlag;
}
