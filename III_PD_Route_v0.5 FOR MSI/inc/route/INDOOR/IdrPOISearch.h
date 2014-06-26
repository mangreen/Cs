/*
 * IdrPOISearch.h - Porting Layer for III Navi system
 *
 * (c)2010 Institute for Information Industry, All Rights Reserved
 *
 * Maintainer: mangreen@iii.org.tw
 *
 * CVS: $Id: Route_IdrAStar_Out.h, v1.5 2011/03/28 04:16:00 Chun-Jen Chen Exp $
 */
#ifndef _IDRPOISEARCH_H_
#define _IDRPOISEARCH_H_

#include <stdlib.h>
#include <string.h>
//#include <conio.h>
#include <math.h>
#include "prototype.h"
#include "III_Route_Manager.h"
/*
 * Constant
 */
#define TYPE_SIZE			(10)		//type size

/*
 * Error Codes
 */

#define SEARCH_INDOOR_POI_FAIL				(-1)		//search indoor poi fail
#define SEARCH_INDOOR_POI_SUCCESS			(0)		//search indoor poi success
#define OPEN_FILE_FAIL						(1)		//open indoor poi file fail

/*
 * Structure
 */
typedef struct _IndoorPOI { //for openlist
	int latitude; //indoor POI latitude
	int longitude; //indoor POI longitude
	int floor; //indoor POI floor
	char type[TYPE_SIZE]; //
}IndoorPOI;

/*
 * Route_IdrAStar out interface functions
 */

#if defined(__cplusplus)

extern "C" {
#endif

int IdrPOISearch(int *, int *,	int,	char *, char *);
int CountDistance(int, int, int, int);

#if defined(__cplusplus)
}

#endif

#endif/*_IDRPOISEARCH_H_*/
