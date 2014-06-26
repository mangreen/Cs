/*
*	CopyRight : ESTI_資策會新興智慧所
*	Description : III_Pedestrian-Route 對外介面API
*	Version & Date : v0.3  2010/5/24
*   AUTHOR : 資策會 凌家強
*/

#include "Route_Astar.h"

/*
*	Function:
*	    測試III_Pedestrian_Route合理性及驗證結果，並將結果輸出在 path.txt
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		 III_RESULTPDRT *resultPDRT - [in] III_PD_RouteOutStruct
*	
*/

III_INT DXDYSCALE,  HWEIGHT1, HWEIGHT2, MAPSCALE;

III_INT Test(Astar *a, III_RESULTPDRT *resultPDRT)
{
	III_INT  i;                       //迴圈Count
	III_CHAR buf[256] = "";           //RouteTable File pointer
	III_FILE *fp = NULL;

	if(NULL == fp) 
	{
		fp = III_fopen("path.txt", "w");
	}

	//寫入Path 個數
	sprintf(buf, "from end to start, totally %d edges!\n", resultPDRT->numEdge);   
	III_fwrite(buf, 1, strlen(buf), fp);
	//寫入起, 終點的鎖路後經緯度
	sprintf(buf, "%d %d %d %d\n", a->start.mx, a->start.my, a->end.mx, a->end.my);
	III_fwrite(buf, 1, strlen(buf), fp);
	
	i = resultPDRT->numEdge -1;

	while( i >= 0 )
	{
		//寫入EdgeID
		sprintf(buf, " EdgeID = %d " ,resultPDRT->resultEdge[i].idEdge);
		III_fwrite(buf, 1, strlen(buf), fp);
		//寫入EdgeDistance
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
*	    III_Pedestrian_Route初始化(RouteTable的開啟及讀檔, 初始化Route模式和所需的資料結構和快取記憶體)
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		 III_RESULTPDRT *resultPDRT - [in] III_PD_RouteOutStruct
*		 [out] see III_Route_Manager.h error definition
*/
III_INT AstarInit(Astar *a, III_RTPARAM *paramRounting)
{
	III_INT i;              //迴圈Count
	III_CHAR buf[256]="";   //暫存RouteTable檔案名稱
	III_CHAR ch = '\0';         //從header擷取出來的資料
	III_CHAR RTID[256];     //RouteTable 識別

	a->area = (Area*)malloc(sizeof(Area));
	
	//RouteTable的開啟及讀檔
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

			//"PD"是用來確認是否為行人導航所需的資料(identifier)
			if(0 != strncmp(RTID, "PD", 2) )
			{
				a->area[i].active = 0;
				return ERROR_PD_ROUTE_ROUTETABLE_FORMAT;
			}

			a->area[i].active = 1;	
			//預設權重是HWEIGHT1, 當Closelist裡的個數大於200時, 會將權重換成HWEIGHT2, 加速挑選 & 調整權重
			HWEIGHT1 = (III_INT)(4.0f*DXDYSCALE*MAPSCALE/10+0.5f);
			HWEIGHT2 = (III_INT)(24.0f*DXDYSCALE*MAPSCALE/10+0.5f);
		}
	}

	//初始化Route模式
	a->mode = paramRounting->modeRouting; //mode=1:最短路徑  mode=2:無障礙空間  mode=3:避雨路徑 

	//Init variables
	a->close.count = 0;

	//分配起點與終點所需之資料結構位置
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

	//初始化快取記憶體
	a->c1 = (Cache*)malloc(sizeof(Cache));
	//每一個block為 2 ^ 10 = 1024 Bytes, 整個Cache有2 ^ 10 = 1024 block，所以大小為1MB
	if( ERROR_SUCCESS != CacheInit(a->c1, 10, 10) )
	{
		return ERROR_PD_ROUTE_CACHEINIT;
	}
	
	return ERROR_SUCCESS;
}

/*
*	Function:
*	    III_Pedestrian_Route 起點設定
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		 PointX - [in] 鎖路後的經度
*		 PointY - [in] 鎖路後的緯度
*		 Fx - [in] 鎖路後的FNode經度
*		 Fy - [in] 鎖路後的FNode緯度
*		 Tx - [in] 鎖路後的TNode經度
*		 Tx - [in] 鎖路後的TNode緯度
*		 [out] see III_Route_Manager.h error definition
*	
*/
III_INT AstarSetStart(Astar *a, III_INT PointX, III_INT PointY, III_INT Fx, III_INT Fy, III_INT Tx, III_INT Ty)
{
	Area *A;
	III_INT  i;          //迴圈Count 
	III_INT  offset = 0; //Node offset
	III_INT  offset2 = 0;//Node offset
	III_INT  num = 0;    //Node所連接其它edge個數
	III_INT  num2 = 0;   //Node所連接其它edge個數
	III_INT  flag = 0;	 //確認方向(F->T)(T->F)
	III_INT  flag2 = 0;  //確認方向(F->T)(T->F)
	AdjNode  adj[8];     //鄰邊的資訊

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

	//位移RouteTable檔案內容(從Full Misc裡擷取Full data的offset index)
	III_fseek(A->fp, A->off1+(a->misc.eid-1)*4, SEEK_SET);
	III_fread(&a->offs[0], 4, 1, A->fp);
	num = (a->offs[0]>>4)&7;
	offset = (a->offs[0]>>7)<<2;

	if(0 == offset)
	{
		return ERROR_PD_ROUTE_ROUTEFILEDATA_STARTPOINT;
	}

	//先將起點周遭的path擷取至Cache memory
	CacheRead(a->c1, offset, ADJSIZE*num, a->misc.cid, adj, A->fp);
	
	//取得full data的資料
	for(i=0, a->offs[1]=0 ;i<num ;i++)
	{
		if((adj[i].b>>9) == a->misc.eid)
		{
			//adj[i].b 位移9位的原因在 原source為 edge id * 2
			//a->misc.eid 為現在edge 的編號
			//依 edge id 來找相對應的Node
			flag = (adj[i].b>>8)%2;				//確認方向(F->T)(T->F)
			//Node id offset
			offset = (adj[i].a>>7)<<2;
			// &7 表示取得 最低3位
			num2 = (adj[i].a>>4)&7;
			offset2 = adj[i].a;					//see spec. Full data part A

			//再從Cache memory擷取起點path另一端的full data(F or TNode)
			CacheRead(a->c1, offset, ADJSIZE*num2, a->misc.cid, adj, A->fp);
			
			for(i=0;i<num2;i++)
			{
				if((adj[i].b>>9)==a->misc.eid)
				{
					//將Node存入 a->offs
					a->offs[0] = adj[i].a;
					a->offs[1] = offset2;
					flag2 = 1;
					i = num2;
				}
			}
			i = num;
		}
	}
	
	//確認一條Path的F,TNode的方向，並於呼叫RunAstar2時，可以將F,TNode的經緯度assign到F,TNode
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
*	    III_Pedestrian_Route 終點設定
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		 PointX - [in] 鎖路後的經度
*		 PointY - [in] 鎖路後的緯度
*		 Fx - [in] 鎖路後的FNode經度
*		 Fy - [in] 鎖路後的FNode緯度
*		 Tx - [in] 鎖路後的TNode經度
*		 Tx - [in] 鎖路後的TNode緯度
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
*	    III_Pedestrian_Route Astar 演算法及權重設定
*	parameter:
*	    Astar *a - [in] Astar struct (see Route_Astar.h)
*		III_RESULTPDRT *resultPDRT - [in] III_PD_RouteOutStruct
*		[out] see III_Route_Manager.h error definition
*	
*/
III_INT AstarRouting(Astar *a,  III_RESULTPDRT * resultPDRT)
{
	III_INT checkRoutingFlag = 0; //ERROR Flag

	//開檔不成功的狀態
	if(0 == a->area[a->misc.cid].active)
	{
		return ERROR_PD_ROUTE_OPENROUTEFILE;
	}

	//檢查起點與終點edgeid是否小於1
	if(a->start.eid<1 || a->end.eid<1)
	{
		return ERROR_PD_ROUTE_OPENROUTEFILE;
	}

	//設定模式權重值
	if(ERROR_SET_PD_ROUTE_WEIGHT == AstarSetWeight(a))
	{
		return ERROR_SET_PD_ROUTE_WEIGHT;
	}

	//進入AstarAlgorithm
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
*		系統結束時，可以呼叫此API清除Astar結構
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
*		重新路徑規劃時，可以呼叫此API清除Astar結構
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
