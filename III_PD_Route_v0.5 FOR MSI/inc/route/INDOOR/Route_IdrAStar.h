/*
 * Route_IdrAStar_Out.h - Porting Layer for III Navi system
 *
 * (c)2010 Institute for Information Industry, All Rights Reserved
 *
 * Maintainer: mangreen@iii.org.tw
 *
 * CVS: $Id: Route_IdrAStar.h, v1.5 2010/05/05 04:16:00 MikeLin Exp $
 */
#ifndef _ROUTE_IDRASTAR_H_
#define _ROUTE_IDRASTAR_H_

#include "prototype.h"
//#include "III_win32.c"
#include "III_Route_Manager.h"

#include <stdio.h>
#include <stdlib.h>
//#include <conio.h>
#include <string.h>
#include <math.h>

#define SWAP(x,y) {int t; t = x; x = y; y = t;}
#define MAXHASH						(256)	//hash size
#define MAXHEAPTREE					(256)	//heap size
#define MAXHEADER					(16)	//header size
#define MAXNEIGHBOR					(20)	//every neighbor size
#define MAXEDGE						(4)	//every neighbor size
/*
typedef struct _Coordinate {
	III_INT x, y, z;
}Coordinate;
*/
typedef struct _Node {
	III_UINT nodeID;
	//Coordinate cdnt;
	III_POINT cdnt;//Coordinate
	III_INT f, g, h;
	
	III_INT offset;
	III_INT amount;	//the neighbor amount of node 
	
	III_INT dist;	//distance
	III_INT attr;	//attribute
	III_INT pflag;	//passage flag
	
	III_UINT edgeID;
	III_INT parentID;
}Node;

typedef struct _Heaptree {
	Node heap[MAXHEAPTREE];
	III_INT count;
}Heaptree;

typedef struct _BitMap {
	III_CHAR *list;
	III_INT num;
}BitMap;

typedef struct _HashNode {
	III_INT hashID;
	Node hnode;
	struct _HashNode *pre;
	struct _HashNode *next;
}HashNode;

typedef struct _IdrHeader {
	III_INT maxN; 
	III_INT	maxE; 
	III_INT fullsize; 
	III_INT dxdyScale; 
	III_INT distScale; 
	III_INT mapScale; 
	III_INT buildingID;
}IdrHeader;

typedef struct _IdrWeight {
	III_INT maxWalkFloor;
	III_INT routingMod;
}IdrWeight;

/*
typedef struct _ResultNode     //Routing node result information for indoor routing
{
	III_INT nodeID;
    III_POINT cdnt;					//Coordinate         
    III_INT attr;                   // The edge attribute
} ResultNode;

typedef struct _IdrResult     //Routing result information for pedestrian and indoor routing
{
    III_RTPOINT pntDeparture;			// The departure point information
    III_RTPOINT pntArrival;				// The arrival point information
    III_INT numNode;					// The total node number of routing plan
    ResultNode *resultNode;           // The edge information by link list
} IdrResult;
*/
typedef struct _IndoorAStar {
	IdrHeader header;
	IdrWeight weight;
	Heaptree opList;//open list
	HashNode *nodeCache;//hash node cache
	BitMap clList;//close list
	III_CHAR *binName;//.bin file name
	FILE *binfile;
	Node start, end;
	III_INT fEndID, tEndID;//fnod and tnode ID
	III_INT fEndEdgeDist, tEndEdgeDist;//fnode and tnode distance
	III_RESULTPDRT resultIdr;//by edge id
	III_RESULTINDOOR result;//by node coordinate
	III_INT stopFlag;
}IndoorAStar;

#if defined __cplusplus
extern "C" {
#endif


III_INT addCloseList(IndoorAStar *, III_INT);
III_INT checkCloseList(IndoorAStar *, III_INT);


III_INT addHash(HashNode[], Node, III_INT);
III_INT getHash(HashNode[], Node *, III_INT);
III_INT getHashGValue(HashNode[], III_INT);
III_INT freeHash(HashNode hash[]);


III_INT insertHeaptree(Heaptree *, Node *);
III_INT deleteHeaptreeRoot(Heaptree *);
III_INT printHeaptree(Heaptree);
III_INT swapHeap(Node[], III_INT, III_INT);
III_INT getDistance(III_INT, III_INT, III_INT, III_INT);


III_INT startIdrAStar(IndoorAStar *);
III_INT getIdrNeighbor(IndoorAStar *, Node *, III_INT, III_INT);
III_INT setIdrResult(IndoorAStar *, Node *, III_INT);
III_INT setIdrResultEdge(IndoorAStar *, Node *, III_INT);
III_INT setIdrResultNode(IndoorAStar *, Node *);

III_INT setIdrWeighting(IndoorAStar *, Node );
III_INT setIdrWeight(IndoorAStar *);


III_INT debugNodeInfo(IndoorAStar*);

#ifdef __cplusplus
  }
#endif

#endif/* INDOORASTAR_H_ */