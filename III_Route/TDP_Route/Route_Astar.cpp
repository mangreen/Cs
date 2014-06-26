/*
*	CopyRight : ESTI_�굦�|�s�����z��
*	Description : III_Pedestrian-Route ��~����API
*	Version & Date : v0.3  2010/5/24
*   AUTHOR : �굦�| ��a�j
*/

#include "Route_Astar.h"

/*
*	Function:
*	    ����III_Pedestrian_Route�X�z�ʤ����ҵ��G�A�ñN���G��X�b path.txt
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		 III_RESULTPDRT *resultPDRT - [in] III_PD_RouteOutStruct
*	
*/

III_INT DXDYSCALE,  HWEIGHT1, HWEIGHT2, MAPSCALE;

III_INT Test(Astar *a, III_RESULTPDRT *resultPDRT)
{
	III_INT  i;                       //�j��Count
	III_CHAR buf[256] = "";           //RouteTable File pointer
	III_FILE *fp = NULL;

	if(NULL == fp) 
	{
		fp = III_fopen("path.txt", "w");
	}

	//�g�JPath �Ӽ�
	sprintf(buf, "from end to start, totally %d edges!\n", resultPDRT->numEdge);   
	III_fwrite(buf, 1, strlen(buf), fp);
	//�g�J�_, ���I�������g�n��
	sprintf(buf, "%d %d %d %d\n", a->start.mx, a->start.my, a->end.mx, a->end.my);
	III_fwrite(buf, 1, strlen(buf), fp);
	
	i = resultPDRT->numEdge -1;

	while( i >= 0 )
	{
		//�g�JEdgeID
		sprintf(buf, " EdgeID = %d " ,resultPDRT->resultEdge[i].idEdge);
		III_fwrite(buf, 1, strlen(buf), fp);
		//�g�JEdgeDistance
		sprintf(buf , "Distance = %d \n" , resultPDRT->resultEdge[i].distanceEdge);
		III_fwrite(buf, 1, strlen(buf), fp);
		i-- ;
	}

	sprintf(buf, "------------------\n\n");
	III_fwrite(buf, 1, strlen(buf), fp);
	fflush((FILE *)fp);
	III_fclose(fp);

	return ERROR_SUCCESS;
}

/*
*	Function:
*	    III_Pedestrian_Route��l��(RouteTable���}�Ҥ�Ū��, ��l��Route�Ҧ��M�һݪ���Ƶ��c�M�֨��O����)
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		 III_RESULTPDRT *resultPDRT - [in] III_PD_RouteOutStruct
*		 [out] see III_Route_Manager.h error definition
*/
III_INT AstarInit(Astar *a, III_RTPARAM *paramRounting)
{
	III_INT i;              //�j��Count
	III_CHAR buf[256]="";   //�ȦsRouteTable�ɮצW��
	III_CHAR ch = '\0';         //�qheader�^���X�Ӫ����
	III_CHAR RTID[256];     //RouteTable �ѧO

	a->area = (Area*)malloc(sizeof(Area));
	
	//RouteTable���}�Ҥ�Ū��
	strcpy(buf, paramRounting->tableRT.nameRoutingTable);
	
	for(i=0; i < paramRounting->tableRT.numRoutingTable; i++)
	{
		a->area[i].fp = III_fopen(buf, "rb");

		if(a->area[i].fp==NULL)
		{
			a->area[i].active = 0;
			return ERROR_PD_ROUTE_OPENROUTEFILE;
		}
		else
		{
			III_fread(&a->area[i].maxEdge, 4, 1, a->area[i].fp);
			III_fread(&a->area[i].off1, 4, 1, a->area[i].fp);	
			III_fread(&ch, 1, 1, a->area[i].fp);

			DXDYSCALE = (III_INT)ch;

			III_fread(&ch, 1, 1, a->area[i].fp);
			MAPSCALE = (III_INT)ch;
	        
			III_fread(&RTID, 2, 1, a->area[i].fp);

			//"PD"�O�ΨӽT�{�O�_����H�ɯ�һݪ����(identifier)
			if(0 != strncmp(RTID, "PD", 2) )
			{
				a->area[i].active = 0;
				return ERROR_PD_ROUTE_ROUTETABLE_FORMAT;
			}

			a->area[i].active = 1;	
			//�w�]�v���OHWEIGHT1, ��Closelist�̪��ӼƤj��200��, �|�N�v������HWEIGHT2, �[�t�D�� & �վ��v��
			HWEIGHT1 = (III_INT)(4.0f*DXDYSCALE*MAPSCALE/10+0.5f);
			HWEIGHT2 = (III_INT)(24.0f*DXDYSCALE*MAPSCALE/10+0.5f);
		}
	}

	//��l��Route�Ҧ�
	a->mode = paramRounting->modeRouting; //mode=1:�̵u���|  mode=2:�L��ê�Ŷ�  mode=3:�׫B���| 

	//Init variables
	a->close.count = 0;

	//���t�_�I�P���I�һݤ���Ƶ��c��m
	memset(&a->start, 0, sizeof(Misc));
	memset(&a->end, 0, sizeof(Misc));

	a->open.heap = NULL;

	//Init close list
	a->close.visitEdge = (III_CHAR**)malloc(sizeof(void*));
	a->close.visitEdge[0] = NULL;
	a->close.list = (Edge2**)malloc(sizeof(void*)*LENGTHM);

	for(i=0;i<LENGTHM;i++)
	{
		a->close.list[i] = NULL;
	}

	//��l�Ƨ֨��O����
	a->c1 = (Cache*)malloc(sizeof(Cache));
	//�C�@��block�� 2 ^ 10 = 1024 Bytes, ���Cache��2 ^ 10 = 1024 block�A�ҥH�j�p��1MB
	if( ERROR_SUCCESS != CacheInit(a->c1, 10, 10) )
	{
		return ERROR_PD_ROUTE_CACHEINIT;
	}
	
	return ERROR_SUCCESS;
}

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
III_INT AstarSetStart(Astar *a, III_INT PointX, III_INT PointY, III_INT Fx, III_INT Fy, III_INT Tx, III_INT Ty)
{
	Area *A;
	III_INT  i;          //�j��Count 
	III_INT  offset = 0; //Node offset
	III_INT  offset2 = 0;//Node offset
	III_INT  num = 0;    //Node�ҳs���䥦edge�Ӽ�
	III_INT  num2 = 0;   //Node�ҳs���䥦edge�Ӽ�
	III_INT  flag = 0;	 //�T�{��V(F->T)(T->F)
	III_INT  flag2 = 0;  //�T�{��V(F->T)(T->F)
	AdjNode  adj[8];     //�F�䪺��T

	if (ERROR_SUCCESS != SaveEdgeInfo(&a->misc, PointX, PointY, Fx, Fy, Tx, Ty))
	{
		return ERROR_PD_ROUTE_SAVEEDGEINFO;
	}
	
	if(0 == a->area[a->misc.cid].active)
	{
		return ERROR_PD_ROUTE_OPENROUTEFILE;
	}

	if(ERROR_SUCCESS != AstarClean(a))
	{
		return ERROR_PD_ROUTE_CLEAN;
	}

	A = &a->area[a->misc.cid];

	//�첾RouteTable�ɮפ��e(�qFull Misc���^��Full data��offset index)
	III_fseek(A->fp, A->off1+(a->misc.eid-1)*4, SEEK_SET);
	III_fread(&a->offs[0], 4, 1, A->fp);
	num = (a->offs[0]>>4)&7;
	offset = (a->offs[0]>>7)<<2;

	if(0 == offset)
	{
		return ERROR_PD_ROUTE_ROUTEFILEDATA_STARTPOINT;
	}

	//���N�_�I�P�D��path�^����Cache memory
	CacheRead(a->c1, offset, ADJSIZE*num, a->misc.cid, adj, A->fp);
	
	//���ofull data�����
	for(i=0, a->offs[1]=0 ;i<num ;i++)
	{
		if((adj[i].b>>9) == a->misc.eid)
		{
			//adj[i].b �첾9�쪺��]�b ��source�� edge id * 2
			//a->misc.eid ���{�bedge ���s��
			//�� edge id �ӧ�۹�����Node
			flag = (adj[i].b>>8)%2;				//�T�{��V(F->T)(T->F)
			//Node id offset
			offset = (adj[i].a>>7)<<2;
			// &7 ��ܨ��o �̧C3��
			num2 = (adj[i].a>>4)&7;
			offset2 = adj[i].a;					//see spec. Full data part A

			//�A�qCache memory�^���_�Ipath�t�@�ݪ�full data(F or TNode)
			CacheRead(a->c1, offset, ADJSIZE*num2, a->misc.cid, adj, A->fp);
			
			for(i=0;i<num2;i++)
			{
				if((adj[i].b>>9)==a->misc.eid)
				{
					//�NNode�s�J a->offs
					a->offs[0] = adj[i].a;
					a->offs[1] = offset2;
					flag2 = 1;
					i = num2;
				}
			}
			i = num;
		}
	}
	
	//�T�{�@��Path��F,TNode����V�A�é�I�sRunAstar2�ɡA�i�H�NF,TNode���g�n��assign��F,TNode
	if(1 == flag)
	{
		offset2+=2;

		if(a->offs[1]!=0)
		{
			a->offs[1] = offset2;
		}
	}
	else
	{
		a->offs[0]+=2;
	}

	if(0 == a->offs[1] && (offset2>>4)>0)
	{
		a->offs[0] = offset2;
	}
	memcpy(&a->start, &a->misc, sizeof(Misc));

	return ERROR_SUCCESS;
}


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
III_INT AstarSetEnd(Astar *a, III_INT PointX, III_INT PointY, III_INT Fx, III_INT Fy, III_INT Tx, III_INT Ty)
{
	Area *A;
	III_INT offset = 0; //Node offset
	III_INT num = 0;    //Node connect number

	if (ERROR_SUCCESS != SaveEdgeInfo(&a->misc, PointX, PointY, Fx, Fy, Tx, Ty))
	{
		return ERROR_PD_ROUTE_SAVEEDGEINFO;
	}

	if(0 == a->area[a->misc.cid].active)
	{
		return ERROR_PD_ROUTE_OPENROUTEFILE;
	}

	if(ERROR_SUCCESS != AstarClean(a))
	{
		return ERROR_PD_ROUTE_CLEAN;
	}

	A = &a->area[a->misc.cid];

	III_fseek(A->fp, A->off1+(a->misc.eid-1)*4, SEEK_SET);
	III_fread(&offset, 4, 1, A->fp);
	num = (offset>>4)&7;
	offset = (offset>>7)<<2;

	if(0 == offset)
	{
		return ERROR_PD_ROUTE_ROUTEFILEDATA_ENDPOINT;
	}

	memcpy(&a->end, &a->misc, sizeof(Misc));

	return ERROR_SUCCESS;
}

/*
*	Function:
*	    III_Pedestrian_Route Astar �t��k���v���]�w
*	parameter:
*	    Astar *a - [in] Astar struct (see Route_Astar.h)
*		III_RESULTPDRT *resultPDRT - [in] III_PD_RouteOutStruct
*		[out] see III_Route_Manager.h error definition
*	
*/
III_INT AstarRouting(Astar *a,  III_RESULTPDRT * resultPDRT)
{
	III_INT checkRoutingFlag = 0; //ERROR Flag

	//�}�ɤ����\�����A
	if(0 == a->area[a->misc.cid].active)
	{
		return ERROR_PD_ROUTE_OPENROUTEFILE;
	}

	//�ˬd�_�I�P���Iedgeid�O�_�p��1
	if(a->start.eid<1 || a->end.eid<1)
	{
		return ERROR_PD_ROUTE_OPENROUTEFILE;
	}

	//�]�w�Ҧ��v����
	if(ERROR_SET_PD_ROUTE_WEIGHT == AstarSetWeight(a))
	{
		return ERROR_SET_PD_ROUTE_WEIGHT;
	}

	//�i�JAstarAlgorithm
	checkRoutingFlag = RunAstar2(a, resultPDRT);

	if(ERROR_SUCCESS != checkRoutingFlag)
	{
		return checkRoutingFlag;
	}
	else
	{
		return ERROR_SUCCESS;
	}
}

/*
*	Function:
*		�t�ε����ɡA�i�H�I�s��API�M��Astar���c
*	Parameter:
*		Astar *a - [in] Astar struct (see Route_Astar.h)
*		[out] see III_Route_Manager.h error definition
*/
III_INT AstarFinish(Astar *a)
{ 
	if(ERROR_SUCCESS != AstarClean(a))
	{
		return ERROR_PD_ROUTE_CLEAN;
	}

	if(ERROR_SUCCESS != CacheClean(a->c1))
	{
		return ERROR_PD_ROUTE_CACHECLEAN;
	}

	free(a->c1);
	free(a->area);
	free(a->close.visitEdge);

	return ERROR_SUCCESS;
}


/*
*	Function:
*		���s���|�W���ɡA�i�H�I�s��API�M��Astar���c
*	Parameter:
*		Astar *a - [in] Astar struct (see Route_Astar.h)
*		[out] see III_Route_Manager.h error definition
*/
III_INT AstarClear(Astar *a)
{
	if(ERROR_SUCCESS != AstarClean(a))
	{
		return ERROR_PD_ROUTE_CLEAN;
	}
	a->start.eid = 0;
	a->end.eid = 0;

	return ERROR_SUCCESS;
}
