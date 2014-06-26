/*
 * CJNIIndoor.h
 *
 *  Created on: 2010/8/31
 *      Author: ASUS
 */

#ifndef CJNIINDOOR_H_
#define CJNIINDOOR_H_
#define _WIN32

//#include "IdrLockRoad.h"
//#include "Route_IdrAstar.h"
//#include "Route_IdrAstar_Out.h"
//#include "III_Route_Indoor.h"
//#include "prototype.h"
//#include "III_Route_Manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <windows.h>
#include <math.h>

#include "jni.h"
#include "json.h"
#include "bean_RouteManager.h"

#include "prototype.h"
#include "III_Route_Manager.h"

#define ERROR_IDRJSON_SUCCESS	(0)
#define ERROR_GETIDRSTEPSJSON_FAIL	(1)
#define ERROR_GETIDRSUBSTEPSJSON_FAIL	(1)
#define ERROR_GETIDRLOCATIONJSON_FAIL	(1)
/*
* Function:
* Parameters:
* Return value:
*/

#if defined(__cplusplus)
extern "C" {
#endif

/*
III_INT TEST_setRouteParameter(int , int , char *);
III_INT TEST_startRouting(int , int , int , int , int , int , char *);
III_INT TEST_getResult();
III_INT TEST_freeIndoor();
*/

int getIdrStepJSON(III_RESULTINDOOR *resultIdrRT, char *build, json_object *steps);
int getIdrSubstepJSON(III_RESULTNODE *startNode, III_RESULTNODE *endNode, json_object *substepobj, int *dist);
int getIdrLocationObj(III_POINT point, III_INT attr, json_object *locationObj);
int getIdrDurationObj(int dist, json_object *durationObj);
int getIdrDistanceObj(int dist, json_object *distanceObj);

#if defined(__cplusplus)
}
#endif

#endif /*CJNIINDOOR_H_*/
