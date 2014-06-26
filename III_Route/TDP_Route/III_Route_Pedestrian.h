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
*	        III_Pedestrian_Route��l��(RouteTable���}�Ҥ�Ū��, ��l��Route�Ҧ��M�һݪ���Ƶ��c�M�֨��O����)
*     parameter:
*			III_RTPARAM *paramRouting - [in] PD_Route �һݪ���T(ex. routetable name and number , typeRouting, modeRouting )
*			 [out] see III_Route_Manager.h error definition
*/
III_INT III_PD_SetRoutingParam(III_RTPARAM*);

/*
*     Function:
*	        III_Pedestrian_Route�_�I, ���I�]�w
*     parameter:
*			III_RTPOINT pntDeparture - [in] ��J�_�I�һݸ�� (����᪺edgeID, F,TNode�g�n��)
*			III_RTPOINT pntArrival - [in] ��J���I�I�һݸ�� (����᪺edgeID, F,TNode�g�n��)
*			 [out] see III_Route_Manager.h error definition
*
*/
III_INT III_PD_Routing(III_RTPOINT pntDeparture, III_RTPOINT pntArrival);
III_INT III_PD_GetPdRTResult(III_RESULTPDRT*);


/*
*	Function:
*	     III_PD_GetPdRTResult 
*			�}�l����A* �t��k�ñNrouting���G(pathID & path distance)�x�s�bIII_RESULTPDRT���c��
*	Parameter:
*		 III_RESULTPDRT *resultPDRT - [in] routing���G(pathID & path distance)�x�s�bIII_RESULTPDRT���c��
*		 [out] see III_Route_Manager.h error definition
*/
III_INT III_PD_GetPdRTResult(III_RESULTPDRT*);

#if defined(__cplusplus)
}
#endif

#endif