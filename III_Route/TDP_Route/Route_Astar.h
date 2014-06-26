/*
*	CopyRight : ESTI_�굦�|�s�����z��
*	Description : III_Pedestrian-Route Route���� & ��~����API �� ���c�w�q
*	Version & Date : v0.3  2010/5/24
*/
#ifndef __ROUTE_ASTAR_H__
#define __ROUTE_ASTAR_H__

#include <stdio.h>
#include <math.h>
#include "platform.h"
#include "prototype.h"
#include "Cache.h"
#include "III_Route_Manager.h"

#define MAXOPENLIST		(4096) 
#define LENGTHM			(256)	//number of 1D array of Node2 **list
#define LENGTHN			(1024)	//the length of each 1D array of Node2 **list
#define ADJSIZE			(16)

typedef struct _Edge { //for openlist
	III_UINT eid; //edgeID
	III_UINT pid; //parent edgeID
	III_INT Pg; //edge���Z��(PAPAGO g)
	III_INT g; //edge���Z��(�P���I���Z���t)
	III_UINT off; //direction
	III_INT x, y; //�g�n��
	III_INT f; //edge�Pedge�����������Z��
}Edge;

typedef struct _Edge2 { //for closelist
	III_UINT eid; //edgeID
	III_UINT pid; //parent edgeID
	III_UINT dist; //edge distance
}Edge2;

typedef struct _Area { //for ��O or �ϰ� 
	III_INT off1;   //��Full Data���_�l��m
	III_INT maxEdge; //����Oor�ϰ� edge���`��
	III_INT active; //�O�_��l�ƹL
	III_FILE *fp; //RouteTable I/O �ɮ׫���
}Area;

typedef struct _Misc {
	III_INT fx, fy; //fnode�g�n��
	III_INT tx, ty; //tnode�g�n��
	III_INT mx, my; //����᪺�g�n��
	III_UINT eid;   //edgeID
	III_UCHAR cid; //country id
}Misc;

typedef struct _AdjNode {
	III_UINT a; //a={nid off/4(25), num(3), reserved(4) }
	III_UINT b; //b={eid(24), reserved(8)}
	III_SHORT dx,dy; //dx, dy
	III_USHORT dist,type; //distance, type (�L��ê�Ŷ�, �׫B���|, �̵u���|)
}AdjNode;

typedef struct _OpenList {
	III_INT limit; //OpenList �̤j�e�ǭӼ�
	III_INT count; //OpenList���Ӽ�
	Edge *heap; //save Edge struct
}OpenList;

typedef struct _CloseList {
	III_INT count; //CloseList���Ӽ�
	III_CHAR **visitEdge; //�O�_�����X�L
	Edge2 **list; //save Edge2 struct 
}CloseList;

typedef struct _Astar {
	III_INT mode; //Route�Ҧ��]�w
	III_INT HW; //���@�T�w�B�I�ơA�[��Route���G
	III_INT distG; //Total distance 
	III_INT stopFlag; //Do~Loop flag until Route have Result
	III_UINT offs[2]; //offs[0],offs[1]:record edge direction
	Misc start, end, misc; //see Misc struct
	Area *area; //see Area struct
	OpenList open; //see OpenList struct
	CloseList close; //see CloseList struct
	Cache *c1; //see Cache struct (Cache.h)
}Astar;

//��H�ɯ�ʺAWeight�]�w
enum PD_ROUTE_TYPE
{
	SIDEWALK = 0x00,
	SHELTER = 0x01,
	ACCESSABLE = 0x02,
	STAIR = 0x04,
	MAX_PD_TYPE = 8,
};

#ifdef __cplusplus
extern "C" {
#endif

/*
*	Function:
*	    ����III_Pedestrian_Route�X�z�ʤ����ҵ��G�A�ñN���G��X�b path.txt
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		 III_RESULTPDRT *resultPDRT - [in] III_PD_RouteOutStruct
*	
*/	
III_INT Test(Astar* , III_RESULTPDRT *); //for testing

/*
*	Function:
*	    III_Pedestrian_Route��l��(RouteTable���}�Ҥ�Ū��, ��l��Route�Ҧ��M�һݪ���Ƶ��c�M�֨��O����)
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		[out] see III_Route_Manager.h error definition
*/
III_INT AstarInit(Astar*, III_RTPARAM*);

/*
*	Function:
*		���s���|�W���ɡA�i�H�I�s��API�M��Astar���c
*	Parameter:
*		Astar *a - [in] Astar struct (see Route_Astar.h)
*		[out] see III_Route_Manager.h error definition
*/
III_INT AstarClear(Astar*); 

/*
*	Function:
*		�t�ε����ɡA�i�H�I�s��API�M��Astar���c
*	Parameter:
*		Astar *a - [in] Astar struct (see Route_Astar.h)
*		 [out] see III_Route_Manager.h error definition
*/
III_INT AstarFinish(Astar*); 

/*
*	Function:
*	    III_Pedestrian_Route Astar �t��k���v���]�w
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		 III_RESULTPDRT *resultPDRT - [in] III_PD_RouteOutStruct
*		 [out] see III_Route_Manager.h error definition
*	
*/
III_INT AstarRouting(Astar* , III_RESULTPDRT*); 

/*
*	Function:
*	    III_Pedestrian_Route �_�I�]�w
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
III_INT AstarSetStart(Astar *, III_INT , III_INT , III_INT , III_INT , III_INT , III_INT ); 

/*
*	Function:
*	    III_Pedestrian_Route ���I�]�w
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
III_INT AstarSetEnd(Astar *, III_INT , III_INT , III_INT , III_INT , III_INT , III_INT ); 

/*
*	Function:
*	    III_Pedestrian_Route Astar �v���]�w
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		  [out] see III_Route_Manager.h error definition
*	
*/
III_INT AstarSetWeight(Astar *);


//inner API

/*
*	Function:
*	    �O���_�I�P���I��edge����,�k�ݸg�n��, ��Oor�ϰ�, ����᪺�I
*	parameter:
*	     Misc *a - [in] Record Edge Info (see Route_Astar.h)
*		 PointX - [in] ����᪺�g��
*		 PointY - [in] ����᪺�n��
*		 Fx - [in] edge�����ݸg��
*		 Fy - [in] edge�����ݽn��
*		 Tx - [in] edge���k�ݸg��
*		 Ty - [in] edge���k�ݽn��
*		 [out] see III_Route_Manager.h error definition
*/
III_INT SaveEdgeInfo(Misc*, III_INT, III_INT, III_INT, III_INT, III_INT, III_INT);

/*
*	Function:
*	    III_Pedestrian_Route �M��Route�һݸ�Ƶ��c���e
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		 [out] see III_Route_Manager.h error definition
*/
III_INT AstarClean(Astar*);

/*
*
*	Function:
*		�[�J��CloseList�̡A�ñNFlag�]�w�����X�L
*	parameter:
*		CloseList - [in] CloseList struct (see Route_Astar.h)
*		e - [in] EdgeID
*        [out] see III_Route_Manager.h error definition
*/
III_INT SetVisit(CloseList*, Edge*);

/*
*
*	Function:
*		�ˬd�O�_�w�bCloseList��
*	parameter:
*		CloseList - [in] CloseList struct (see Route_Astar.h)
*		cId - [in] ��O or �ϰ� ID
*		e - [in] EdgeID
*       [out] True : �w�bCloseList��, False : ���bCloseList��
*/
III_INT IsVisit(CloseList*, III_UCHAR, III_INT);

/*
*
*	Function:
*		Pop�bOpenList�̪�Edge (�v���̻����b����)
*	parameter:
*		OpenList - [in] OpenList struct (see Route_Astar.h)
*		[out] �p�G�bOpenList�̡A�h�^��Edge struct�A�����\�h�^��NULL
*/
Edge OpenlistPop(OpenList*);

/*
*
*	Function:
*		start of OpenList's member function - Stack push
*		Min-Heap tree algorithm
*		Push���t��k���Nedge push�istack�ɡA
*		�|�Nstack�̪�edge��f��(f=g+h)������A�p�Gf�ȶV�j�������A�h�V�a��stack�����ݡC
*	parameter:
*		OpenList - [in] OpenList struct (see Route_Astar.h)
*		Edge - [in] Edge struct (see Route_Astar.h)
*		[out] see III_Route_Manager.h error definition
*/
III_INT OpenlistPush(OpenList*, Edge);

/*
*	Function:
*	    III_Pedestrian_Route Astar �t��k�D�{��
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		 III_RESULTPDRT *resultPDRT - [in] III_PD_RouteOutStruct
*		 [out] see III_Route_Manager.h error definition
*	
*/
III_INT RunAstar2(Astar* , III_RESULTPDRT* );

/*
*
*	Function:
*		�ˬd�bOpenList�̬O�_����Edge
*	parameter:
*		OpenList - [in] OpenList struct (see Route_Astar.h)
*		e - [in] EdgeID
*		[out] �p�G�bOpenList�̡A�h�^��Edge struct�A�����\�h�^��NULL
*/
III_INT InOpenlist(OpenList*, III_UINT);

/*
*	Function:
*	    III_Pedestrian_Route �M��CloseList���e
*	parameter:
*	    CloseList - [in] CloseList struct (see Route_Astar.h)
*		Area - [in] Area struct (see Route_Astar.h)
*       [out] see III_Route_Manager.h error definition
*/
III_INT FreeCloselist(CloseList*, Area*);

/*
*
*	Function:
*		��Route�����ɡA���oCloseList�̪����e
*	parameter:
*		CloseList - [in] CloseList struct (see Route_Astar.h)
*        III_RESULTPDRT *resultPDRT - [in] III_PD_RouteOutStruct
*		[out] see III_Route_Manager.h error definition
*/
III_INT PGetPath(CloseList*, III_RESULTPDRT*);

/*
*
*	Function:
*		���s�ƦCOpenList�̪�����
*	parameter:
*		OpenList - [in] OpenList struct (see Route_Astar.h)
*		H1 - [in] weight 1
*		H2 - [in] weight 2
*		[out] see III_Route_Manager.h error definition
*/
III_INT RearrangeOpen(OpenList*, III_INT, III_INT);


#ifdef __cplusplus
}
#endif


#endif
