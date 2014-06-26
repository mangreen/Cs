/*
 * Route_IdrAStar_Out.h - Porting Layer for III Navi system
 *
 * (c)2010 Institute for Information Industry, All Rights Reserved
 *
 * Maintainer: mangreen@iii.org.tw
 *
 * CVS: $Id: Route_IdrAStar_Out.h, v1.5 2010/05/05 04:16:00 MikeLin Exp $
 */
#ifndef _ROUTE_IDRASTAR_OUT_H_
#define _ROUTE_IDRASTAR_OUT_H_

#include "prototype.h"

/*
 * Error Codes
 */
#define NODE_HAVE_NO_PARENT				(-1)

#define CLLIST_HAS_NOT_ADDED			(0)		//node has not added CloseList
#define CLLIST_HAS_ADDED				(1)		//node has added CloseList

#define ERROR_CLLIST_ADD_SUCCESS		(0)		//node add CloseList success
#define ERROR_CLLIST_NID_LTZ			(-1)		//nodeID is less than zero

#define HASH_GET_SUCCESS				(0)		//get Hash success
#define HASH_GET_FAIL					(1)		//get Hash fail
#define HASH_NEW_NODE					(0)		//add new Node to Hash
#define HASH_REFRASH_NODE				(1)		//refrash Node info in Hash
#define HASH_GET_GVALUE_FAIL			(-1)	//get G value from Hash fail

#define PATH_ELEVATOR					(4)
#define PATH_STAIR_AND_ESCALATOR		(3)
#define PATH_ESCALATOR					(2)
#define PATH_STAIR						(1)
#define PATH_PASSAGE					(0)

#define MODE_SHORTEST					(1)
#define	MODE_ACCESSABLE					(2)
#define	MODE_RAIN_AVOID					(3)
//#define ERROR_SET_INDOOR_WEIGHT		(4)

#define EROOR_SET_START_SUCCESS			(0)
#define ERROR_SET_START_EDGE_NULL		(1)
#define ERROR_EDGE_NO_EXSIT				(3)
#define EROOR_SET_END_SUCCESS			(0)
#define ERROR_SET_END_EDGE_NULL			(1)
#define ERROR_SET_END_POINT_NULL		(2)

#define ERROR_ASTAR_SUCCESS				(0)
#define ERROR_ASTAR_CANT_FIND_ROAD		(1)

#define ERROR_SET_RESULT_SUCCESS		(0)
#define ERROR_SET_RESULT_FAIL			(1)

#define ERROR_SET_RESULT_NODE_SUCCESS	(0)
#define ERROR_SET_RESULT_NODE_FAIL		(1)

/*
 * Route_IdrAStar out interface functions
 */

#if defined(__cplusplus)

extern "C" {
#endif
III_INT initialAStar(IndoorAStar *);
III_INT finishAStar(IndoorAStar *);
III_INT setIdrAStarStart(IndoorAStar *);
III_INT	setIdrAStarEnd(IndoorAStar *);

III_INT freeIdrResult(IndoorAStar *);

#if defined(__cplusplus)
}

#endif

#endif/*_ROUTE_IDRASTAR_OUT_H_*/
