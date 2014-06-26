/*
*	CopyRight : ESTI_�굦�|�s�����z��
*	Description : III_Pedestrian-Route Route ��~����API 
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
 *                   �̷�TYPE_ROUTE_PD�����P[See Route_Manager define]�I�s��H, �j��, �Ǥ��ɯ誺RouteEngine��l��
 *		parameter:
 *			III_RTPARAM *paramRouting - [in] Route �һݪ���T(ex. routetable name and number , typeRouting, modeRouting )
 *			[out] see III_Route_Manager.h error definition
*/
III_INT III_SetRoutingParam(III_RTPARAM*);
		
/*
 *		Function:
 *			III_Routing
 *                   �̷�TYPE_ROUTE_PD�����P[See Route_Manager define]�I�s��H, �j��, �Ǥ��ɯ誺�_,���I�]�w
 *		parameter:
 *			III_RTPOINT pntDeparture - [in] ��J�_�I�һݸ�� (����᪺edgeID, F,TNode�g�n��)
 *			III_RTPOINT pntArrival - [in] ��J���I�I�һݸ�� (����᪺edgeID, F,TNode�g�n��)
 *			[out] see III_Route_Manager.h error definition
*/
III_INT III_Routing(III_RTPOINT pntDeparture, III_RTPOINT pntArrival);
		
/*
 *		Function:
 *			III_Routing
 *                   �̷�TYPE_ROUTE_PD�����P[See Route_Manager define]�I�s��H, �j��, �Ǥ��ɯ誺A*�t��k�ñN���G(PathID & Path distance)�x�s�bIII_RESULTPDRT���c��
 *		parameter:
 *			III_RESULTPDRT *resultPDRT - [in] routing���G(pathID & path distance)�x�s�bIII_RESULTPDRT���c��
 *			[out] see III_Route_Manager.h error definition
*/
III_INT III_GetPdRTResult(III_RESULTPDRT*);

#endif