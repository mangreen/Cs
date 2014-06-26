#ifndef __ROUTE_INDOOR_H__
#define __ROUTE_INDOOR_H__

#include "III_Route_Manager.h"
#include "Route_IdrAStar.h"
#include "Route_IdrAStar_Out.h"

#define ERROR_IDR_SUCCESS			(0)
#define	ERROR_IDR_ASTAR_NULL		(1)
#define	ERROR_IDR_RESULTPDRT_NULL	(2)

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

III_INT III_Indoor_SetRoutingParam(III_RTPARAM*);
III_INT III_Indoor_Routing(III_RTPOINT pntDeparture, III_RTPOINT pntArrival);
III_INT III_Indoor_GetIdrRTResult(III_RESULTINDOOR *resultIdrRT);
//III_INT III_Indoor_GetIdrRTResult(III_RESULTPDRT*);
III_INT III_Indoor_FreeIdrAStar(void);

#if defined(__cplusplus)
}
#endif

#endif