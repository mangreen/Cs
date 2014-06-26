/*
*	CopyRight : ESTI_�굦�|�s�����z��
*	Description : III_Pedestrian-Route Route ��~����API 
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
		 *                   �̷�TYPE_ROUTE_PD�����P[See Route_Manager define]�I�s��H, �j��, �Ǥ��ɯ誺RouteEngine��l��
		 *		parameter:
		 *			III_RTPARAM *paramRouting - [in] Route �һݪ���T(ex. routetable name and number , typeRouting, modeRouting )
		 *			[out] see III_Route_Manager.h error definition
		*/
		III_INT III_SetFusionRoutingParam(III_RTPARAM *paramRounting);
		
		/*
		 *		Function:
		 *			III_Routing
		 *                   �̷�TYPE_ROUTE_PD�����P[See Route_Manager define]�I�s��H, �j��, �Ǥ��ɯ誺�_,���I�]�w
		 *		parameter:
		 *			III_POINT Departure - [in] ��J�_�I�һݸ�� (����᪺edgeID, F,TNode�g�n��)
		 *			III_POINT Arrival - [in] ��J���I�I�һݸ�� (����᪺edgeID, F,TNode�g�n��)
		 *			[out] see III_Route_Manager.h error definition
		*/		
		III_INT III_FusionRouting(III_POINT Departure, III_POINT Arrival);
		III_INT III_OutdoorRoutingFunction(III_RouteAPI *IIIRoute, III_POINT Departure, III_POINT Arrival);
		III_INT III_IndoorRoutingFunction(III_RouteAPI *IIIRoute, III_POINT Departure, III_POINT Arrival, char *sBuildingString);
		/*
 		 *		Function:
		 *			III_Routing
		 *                   �̷�TYPE_ROUTE_PD�����P[See Route_Manager define]�I�s��H, �j��, �Ǥ��ɯ誺A*�t��k�ñN���G(PathID & Path distance)�x�s�bIII_RESULT_FUSION_RT���c��
		 *		parameter:
		 *			III_RESULT_FUSION_RT *resultRT - [in] routing���G(pathID & path distance)�x�s�bIII_RESULT_FUSION_RT���c��
		 *			[out] see III_Route_Manager.h error definition
		*/		
		III_INT III_GetFusionRTResult(III_RESULT_FUSION_RT **resultRT);
		void DeleteIIIRouteObject(void);	
		void NewResultFusionRT(void);	
};

#endif