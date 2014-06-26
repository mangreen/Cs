/*
*	CopyRight : ESTI_資策會新興智慧所
*	Description : III_Pedestrian-Route Route內部介面API
*	Version & Date : v0.3  2010/5/24
*   AUTHOR : 資策會 凌家強
*/
#include "Route_Astar.h"

III_INT Pow2[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};  //將edgeID分類成8種，以減少edgeID的儲存空間
III_INT RW[MAX_PD_TYPE];

/*
*	Function:
*	    記錄起點與終點的edge的左,右端經緯度, 國別or區域, 鎖路後的點
*	parameter:
*	     Misc *a - [in] Record Edge Info (see Route_Astar.h)
*		 PointX - [in] 鎖路後的經度
*		 PointY - [in] 鎖路後的緯度
*		 Fx - [in] edge的左端經度
*		 Fy - [in] edge的左端緯度
*		 Tx - [in] edge的右端經度
*		 Ty - [in] edge的右端緯度
*
*/
III_INT SaveEdgeInfo(Misc *m, III_INT PointX, III_INT PointY,  III_INT Fx, III_INT Fy, III_INT Tx, III_INT Ty)
{
	m->fx = Fx;
	m->fy = Fy;
	m->tx = Tx;
	m->ty = Ty;

	m->cid = 0;

	m->mx = PointX;
	m->my = PointY;

	return ERROR_SUCCESS;
}

/*
*	Function:
*	    III_Pedestrian_Route Astar 權重設定
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		  [out] see III_Route_Manager.h error definition
*	
*/
III_INT AstarSetWeight(Astar *a)
{
	//RW[SIDEWALK]:人行道 RW[SHELTER]:有遮避道路 RW[ACCESSABLE]:無障礙道路 RW[STAIR]:階梯 RW[SHELTER|ACCESSABLE]:有遮避和無障礙 
	//RW[SHELTER|STAIR]:無障礙和階梯 RW[ACCESSABLE|STAIR]:有遮避和階梯 RW[SHELTER|ACCESSABLE|STAIR]:皆有
	//mode=1:最短路徑  mode=2:無障礙空間  mode=3:避雨路徑 
	if(MODE_PD_SHORTEST == a->mode)
	{
		RW[SIDEWALK] = 15; 
		RW[SHELTER] = 15; 
		RW[ACCESSABLE] = 15; 
		RW[STAIR]= 20; 
		RW[SHELTER|ACCESSABLE] = 15; 
		RW[SHELTER|STAIR] = 15;
		RW[ACCESSABLE|STAIR] = 15; 
		RW[SHELTER|ACCESSABLE|STAIR] = 20;

		return ERROR_SUCCESS;
	}
	else if(MODE_PD_ACCESSABLE == a->mode)
	{
		RW[SIDEWALK] = 50; 
		RW[SHELTER] = 30; 
		RW[ACCESSABLE] = 10; 
		RW[STAIR] = 100; 
		RW[SHELTER|ACCESSABLE]  = 10;
		RW[SHELTER|STAIR]  = 100; 
		RW[ACCESSABLE|STAIR] = 10; 
		RW[SHELTER|ACCESSABLE|STAIR] = 10;

		return ERROR_SUCCESS;
	}
	else if(MODE_PD_RAIN_AVOID == a->mode)
	{
		RW[SIDEWALK] = 50; 
		RW[SHELTER] = 10;
		RW[ACCESSABLE] = 30; 
		RW[STAIR] = 100;
		RW[SHELTER|ACCESSABLE] = 10;
		RW[SHELTER|STAIR] = 50; 
		RW[ACCESSABLE|STAIR] = 40; 
		RW[SHELTER|ACCESSABLE|STAIR] = 40;

		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_SET_PD_ROUTE_WEIGHT;
	}
}

/*
*	Function:
*	    III_Pedestrian_Route Astar 演算法主程式
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*		 III_RESULTPDRT *resultPDRT - [in] III_PD_RouteOutStruct
*		 [out] see III_Route_Manager.h error definition
*	
*/

extern III_INT DXDYSCALE,  HWEIGHT1, HWEIGHT2, MAPSCALE;

III_INT RunAstar2(Astar *a, III_RESULTPDRT*  resultPDRT)
{
	Edge edge;
	Edge edge2;
	AdjNode adj;
	Area *A = a->area;

	III_INT i, j;                                   //i, j 是for迴圈的初始值
	III_INT tempMax = 0;                            //tempMax : start edge國家裡的最大edge數
	III_INT maxCloseCount=LENGTHM*LENGTHN;          //CloseList最大的個數(可容忍的CloseList個數)
	III_INT endX=(a->end.mx+DXDYSCALE/2)/DXDYSCALE; //終點的經度
	III_INT endY=(a->end.my+DXDYSCALE/2)/DXDYSCALE; //終點的緯度 
	III_INT offset = 0;                             //在Full Data offset位置
	III_INT num = 0;                                //現在Node的相鄰Node個數
	III_INT adjnId = 0;                             //鄰邊的edgeID(2K or 2K + 1)
	III_INT adjnId2 = 0;                            //鄰邊的edgeID(adjnId >> 1)
	III_INT newG = 0;                               //f(newG) = g + h
	III_INT gap = 0;                                //在OpenList裡的edge和newG的值差
	III_INT p = 0;                                  //在OpenList裡哪個位置
	III_INT SLD = 0;                                //直線距離
	III_UCHAR curId = a->start.cid;                 //現在國家 or 區域的ID
	III_UCHAR buf[256];                             //從Cache擷取出來的Node資料暫存
	III_UCHAR *ptr;                                 //現在Node指標

	if(ERROR_SUCCESS != AstarClean(a))
	{
		return ERROR_PD_ROUTE_CLEAN;
	}
	a->HW = HWEIGHT1;

	if((III_INT)a->start.eid > A[a->start.cid].maxEdge || (III_INT)a->end.eid > A[a->end.cid].maxEdge || a->start.eid==a->end.eid)
	{
		return ERROR_PD_ROUTE_ROUTEFILEDATA;
	}

	tempMax = A[a->start.cid].maxEdge; //start edge國家裡的最大edge數

	SLD = abs(a->start.mx-a->end.mx) + abs(a->start.my-a->end.my); 	//直線距離

	tempMax = tempMax*2+1; 
	a->open.heap = (Edge*)malloc(sizeof(Edge)*(a->open.limit = MAXOPENLIST/2));
	a->close.visitEdge[0] = (III_CHAR*)malloc(tempMax/8+1);
	memset(a->close.visitEdge[0], 0, tempMax/8+1);

	memset(&edge, 0, sizeof(Edge));

	//先把start edge 存入OpenList
	for(i=0;i<2;i++)
	{
		if(0 == (a->offs[i]>>4))
		{
			continue;
		}

		j = a->offs[i]&2;

		edge.eid = (a->start.eid<<1) + (j>>1);

		if(0 == j)
		{
			edge.x = a->start.fx;
			edge.y = a->start.fy;
		}
		else
		{
			edge.x = a->start.tx;
			edge.y = a->start.ty;
		}
		
		//計算鎖路的起點與起點的節點(F, TNode)的距離
		edge.Pg = sqrt((float)(abs(a->start.mx - edge.x) * abs(a->start.mx - edge.x) + abs(a->start.my - edge.y) * abs(a->start.my - edge.y))) / 11;
		edge.x = (edge.x+DXDYSCALE/2)/DXDYSCALE;
		edge.y = (edge.y+DXDYSCALE/2)/DXDYSCALE;
		edge.f = (abs(edge.x-endX)+abs(edge.y-endY))*a->HW; //計算edge與終點之間的直角距離
		edge.off = a->offs[i]-j;

		if(ERROR_SUCCESS != OpenlistPush(&a->open, edge))
		{
			if(ERROR_SUCCESS != AstarClean(a))  //heap full
			{
				return ERROR_PD_ROUTE_CLEAN;
			}
			else
			{
				return ERROR_PD_ROUTE_OPENLISTFULL;
			}
		}
	}

	//A-star Algorithm Start !
	while(0 == a->stopFlag)
	{
		edge = OpenlistPop(&a->open);
	
		if(0 == edge.eid)
		{
			if(0 != AstarClean(a))
			{
				return ERROR_PD_ROUTE_CLEAN;
			}
			else
			{
				return ERROR_PD_ROUTE_EDGEIDBYZERO;
			}
		}

		if(ERROR_SUCCESS != SetVisit(&a->close, &edge))
		{
			return ERROR_PD_ROUTE_SETVISIT;
		}

		//如果目前的edge為end edge
		if((edge.eid>>1) == a->end.eid)
		{
			//計算終點與節點的距離
			if(0 == edge.eid %2)
			{
				edge.Pg =  sqrt((float)(abs(a->end.mx - a->end.fx) * abs(a->end.mx - a->end.fx) + abs(a->end.my - a->end.fy) * abs(a->end.my - a->end.fy))) /11;
			}
			else
			{
				edge.Pg =  sqrt((float)(abs(a->end.mx - a->end.tx) * abs(a->end.mx - a->end.tx) + abs(a->end.my - a->end.ty) * abs(a->end.my - a->end.ty))) /11;
			}

			//將距離重算後，再輸入至原本的位置
			if(ERROR_SUCCESS != SetVisit(&a->close, &edge))
			{
				return ERROR_PD_ROUTE_SETVISIT;
			}
			break;
		}

		if(a->close.count == maxCloseCount)
		{
			if(ERROR_SUCCESS != AstarClean(a))
			{
				return ERROR_PD_ROUTE_CLEAN;
			}
			else
			{
				return ERROR_PD_ROUTE_CLOSELISTFULL;
			}
		}

		if(edge.off<0x10)
		{
			continue;
		}

		// 再一次Push & Pop Min-heap tree 重建
		if((0 == a->close.count%200 && a->HW!=HWEIGHT2) && a->close.count>=2400)
		{
			if ( ERROR_SUCCESS != RearrangeOpen(&a->open, a->HW, HWEIGHT2))
			{
				return ERROR_PD_ROUTE_REARRANGEOPENLIST;
			}
			a->HW = HWEIGHT2;
		}

		//Node's Info
		offset = (edge.off>>7)<<2;
		num = (edge.off>>4)&7;
		
		CacheRead(a->c1, offset, ADJSIZE*num, curId, buf, A[curId].fp);

		//printf("a->c1 = %d, offset = %d, ADJSIZE*num = %d, curId = %d, buf = %d",a->c1, offset, ADJSIZE*num, curId,buf);
		j = edge.eid>>1; //edge.eid 右移一位，即為原來的edge id 編號
	
		//ptr為現在的Node指標,Buf包括start's Node 周圍Node ptr += 16, 因為一個Node用 16 BYTE 儲存
		for(ptr=buf, i=0; i<num; i++, ptr+=ADJSIZE)
		{
			memcpy(&adj, ptr, ADJSIZE);
			adjnId = adj.b>>8;
			adjnId2 = adjnId>>1;
			if(!IsVisit(&a->close, 0, adjnId))
			{
				//這個鄰邊是否在OpenList當中，如果有，就將邊取出(misc)，沒有就加入到OpenList中
				//p 回傳鄰邊在那一個位置裡
				p = InOpenlist(&a->open, adjnId);
				
				newG = edge.g + adj.dist*RW[adj.type];

				if(p>0)
				{
					//放置到openlist裡的Edge的g值比adj邊來的大
					gap = a->open.heap[p].g-newG;

					if(gap>0)
					{ //new gValue is better than the old one
						a->open.heap[p].f-=gap; //modify f value
						a->open.heap[p].g = newG; //modify g value
						a->open.heap[p].pid = edge.eid; //reset parent
					}
				}
				else
				{
					edge2.pid = edge.eid;
					edge2.eid = adjnId;
					//將edge的距離暫存至edge2.Pg
					edge2.Pg = adj.dist;
					edge2.g = newG;

					if(0 == adjnId%2 )
					{
						adj.dx =-adj.dx;
						adj.dy =-adj.dy;
					}

					edge2.x = edge.x + adj.dx;
					edge2.y = edge.y + adj.dy;
					//f = g(真實距離) + h
					edge2.f = (abs(edge2.x-endX)+abs(edge2.y-endY))*a->HW + edge2.g; //計算鄰邊與終點之間的直角距離
					edge2.off = adj.a;

					if(ERROR_SUCCESS != OpenlistPush(&a->open, edge2))
					{
						if(ERROR_SUCCESS != AstarClean(a))  //heap full
						{
							return ERROR_PD_ROUTE_CLEAN;
						}
						else
						{
							return ERROR_PD_ROUTE_OPENLISTFULL;
						}
					}
				}
			}
		} //end of for(ptr=buf)

	} //end of main while loop

	free(a->open.heap);
	a->open.heap = NULL;
	
	//將CloseList所拜訪的節點從終點一直Link到起點，並儲存在resultPDRT裡
	if(ERROR_SUCCESS != PGetPath(&a->close, resultPDRT)) 
	{
		return ERROR_PD_ROUTE_GETPATH;
	}

	if(ERROR_SUCCESS != FreeCloselist(&a->close, NULL))
	{
		return ERROR_PD_ROUTE_FREECLOSELIST;
	}

	return ERROR_SUCCESS;
}
/*
*	Function:
*	    III_Pedestrian_Route 清除Route所需資料結構內容
*	parameter:
*	     Astar *a - [in] Astar struct (see Route_Astar.h)
*	
*/
III_INT AstarClean(Astar *a)
{
	a->open.count = 0;
	a->stopFlag = 0;

	free(a->open.heap);
	a->open.heap = NULL;

	if(ERROR_SUCCESS != FreeCloselist(&a->close, NULL))
	{
		return ERROR_PD_ROUTE_FREECLOSELIST;
	}
	return ERROR_SUCCESS;
}


/*
*	Function:
*	    III_Pedestrian_Route 清除CloseList內容
*	parameter:
*	    CloseList - [in] CloseList struct (see Route_Astar.h)
*		Area - [in] Area struct (see Route_Astar.h)
*		[out] see III_Route_Manager.h error definition
*/
III_INT FreeCloselist(CloseList *close, Area *A)
{
	free(close->visitEdge[0]);
	close->visitEdge[0] = NULL;
	close->count = 0;
	return ERROR_SUCCESS;
}

/*
*
*	Function:
*		start of OpenList's member function - Stack push
*		Min-Heap tree algorithm
*		Push的演算法為將edge push進stack時，
*		會將stack裡的edge做f值(f=g+h)的比較，如果f值越大的元素，則越靠近stack的頂端。
*	parameter:
*		OpenList - [in] OpenList struct (see Route_Astar.h)
*		Edge - [in] Edge struct (see Route_Astar.h)
*		[out] see III_Route_Manager.h error definition
*/
III_INT OpenlistPush(OpenList *open, Edge edge)
{
	// count : open list 元素個數
	Edge temp;                      //for swap
	III_INT child;                  //child edge ID
	III_INT parent;                 //parent edge ID
	child = ++open->count;
	parent = child>>1;

	if(open->count>=open->limit)
	{
		return ERROR_PD_ROUTE_OPENLISTFULL;
	}
	// push至openlist 結構裡的Edge 結構
	open->heap[open->count] = edge;

	while(parent>=1)
	{
		 //當在Stack裡的元素比剛push進stack的元素還大時，就進行交換
		if(open->heap[parent].f>open->heap[child].f)
		{
			temp = open->heap[parent];
			open->heap[parent] = open->heap[child];
			open->heap[child] = temp;
		}
		child = parent;
		parent = child>>1;
	}
	return ERROR_SUCCESS;
}


/*
*
*	Function:
*		Pop在OpenList裡的Edge (權重最輕的在頂端)
*	parameter:
*		OpenList - [in] OpenList struct (see Route_Astar.h)
*		[out] 如果在OpenList裡，則回傳Edge struct，不成功則回傳NULL
*/
Edge OpenlistPop(OpenList *open)
{
	III_INT parent;                //parent edge ID
	III_INT child;                 //child edge ID
	Edge popItem = open->heap[1];
	Edge temp;

	if(0 == open->count)
	{
		popItem.eid = 0;
		return popItem;
	}

	open->heap[1] = open->heap[open->count--];
	parent = 1;
	child = 2;

	while(child <= open->count)
	{
		if(open->heap[child].f > open->heap[child+1].f)
		{
			child = child+1;
		}

		if(open->heap[child].f < open->heap[parent].f)
		{
			temp = open->heap[parent];
			open->heap[parent] = open->heap[child];
			open->heap[child] = temp;
			parent = child;
			child = child*2;
		}
		else
		{
			child = open->count+1; //stop the loop
		}
	} //end of while loop
	return popItem;
}


/*
*
*	Function:
*		檢查在OpenList裡是否有此Edge
*	parameter:
*		OpenList - [in] OpenList struct (see Route_Astar.h)
*		e - [in] EdgeID
*		[out] 如果在OpenList裡，則回傳Edge struct，不成功則回傳NULL
*/
III_INT InOpenlist(OpenList *open, III_UINT e)
{
	III_INT i; //迴圈Count

	for(i=1;i<=open->count;i++)
	{
		if(open->heap[i].eid==e)
		{
			return i;
		}
	}
	return ERROR_SUCCESS;
}


/*
*
*	Function:
*		加入到CloseList裡，並將Flag設定為拜訪過
*	parameter:
*		CloseList - [in] CloseList struct (see Route_Astar.h)
*		e - [in] EdgeID
*/
III_INT SetVisit(CloseList *close, Edge *e)
{
	III_INT quotient;                   //CloseList two dimension row
	III_INT remainder;                  //CloseList two dimension column
	III_INT eid = e->eid&0xffffff;      //edge ID
	III_INT cId = e->eid>>24;           //Country ID

	//unvisited Set
	//將2~7 Edge ID 為一組 0 可參考 7組記錄是否拜訪過, 8~15 Edge ID 為一組, 16~23 Edge ID 為一組
	close->visitEdge[cId][eid/8]|=Pow2[eid%8];
	quotient = close->count/LENGTHN;
	remainder = close->count%LENGTHN;
	
	if(0 == remainder)
	{
		close->list[quotient] = (Edge2*)malloc(sizeof(Edge2)*LENGTHN);
	}
	memcpy(&close->list[quotient][remainder], e, sizeof(Edge2));
	close->count++;
	return ERROR_SUCCESS;
}


/*
*
*	Function:
*		檢查是否已在CloseList裡
*	parameter:
*		CloseList - [in] CloseList struct (see Route_Astar.h)
*		cId - [in] 國別 or 區域 ID
*		e - [in] EdgeID
*       [out] True : 已在CloseList裡, False : 不在CloseList裡
*/
III_INT IsVisit(CloseList *close, III_UCHAR cId, III_INT eId)
{
	//visited Set
	//將2~7 Edge ID 為一組 0 可參考 7組記錄是否拜訪過, 8~15 Edge ID 為一組, 16~23 Edge ID 為一組
	return close->visitEdge[cId][eId/8]&Pow2[eId%8];
}

/*
*
*	Function:
*		重新排列OpenList裡的順序
*	parameter:
*		OpenList - [in] OpenList struct (see Route_Astar.h)
*		H1 - [in] weight 1
*		H2 - [in] weight 2
*       [out] True : 已在CloseList裡, False : 不在CloseList裡
*/
III_INT RearrangeOpen(OpenList *open, III_INT H1, III_INT H2)
{ //from h1 to h2
	III_INT i;                   //迴圈Count
	III_INT tc = open->count;
	Edge *temp;
	Edge *p;

	if(H1==H2) 
	{
		return ERROR_SUCCESS;
	}
	temp = (Edge*)malloc(sizeof(Edge)*open->count);

	for(i=1;i<=tc;i++)
	{
		p = &open->heap[i];
		p->f+=((p->f-p->g)/H1*H2);
	}

	for(i=0;i<tc;i++)
	{
		temp[i] = OpenlistPop(open);
	}
	for(i=0;i<tc;i++)
	{
		if(ERROR_SUCCESS != OpenlistPush(open, temp[i]))
		{
			return ERROR_PD_ROUTE_OPENLISTFULL;
		}
	}
	free(temp);

	return ERROR_SUCCESS;
}


/*
*
*	Function:
*		當Route結束時，取得CloseList裡的內容
*	parameter:
*		CloseList - [in] CloseList struct (see Route_Astar.h)
*		 III_RESULTPDRT *resultPDRT - [in] III_PD_RouteOutStruct
*      [out] see III_Route_Manager.h error definition
*/
III_INT PGetPath(CloseList *close, III_RESULTPDRT* resultPDRT)
{
	III_INT quotient = 0;       //CloseList two dimension row
	III_INT remainder = 0;		//CloseList two dimension column			
	III_INT qCount = 0;			//row count
	III_INT rCount = -1;        //column count
	III_INT edgeTotalCount=0;   //edge count
	III_UINT parent;            //parent edge
	III_INT eCount = 0;         //edge count

	if(0 == close->count)
	{
		return ERROR_PD_ROUTE_CLOSELISTEMPTY;
	}
	
	quotient = (close->count-1)/LENGTHN; //quotient
	remainder = (close->count-1)%LENGTHN; //remainder
	parent = close->list[quotient][remainder].pid;

	edgeTotalCount++;

	//先計算實際Path的個數
	while(quotient!=qCount || remainder!=rCount)
	{
		if(-1 == remainder)
		{
			quotient--;
			remainder = LENGTHN-1;
		}
		
		if(close->list[quotient][remainder].eid==parent)
		{
			parent = close->list[quotient][remainder].pid;
			edgeTotalCount++;
		}
		remainder--;
	}

	//依實際Path個數產生memory
	resultPDRT->numEdge = edgeTotalCount;
	resultPDRT->resultEdge = (III_PDRTEDGE *)malloc(sizeof(III_PDRTEDGE) * edgeTotalCount);

	quotient = (close->count-1)/LENGTHN; //quotient
	remainder = (close->count-1)%LENGTHN; //remainder
	parent = close->list[quotient][remainder].pid;

	//將CloseList所拜訪的節點從終點一直Link到起點，並儲存在PathInfo	
	while(quotient!=qCount || remainder!=rCount)
	{
		if(-1 == remainder)
		{
			quotient--;
			remainder = LENGTHN-1;
		}

		if(close->list[quotient][remainder].eid==parent || 0 == eCount )
		{
			parent = close->list[quotient][remainder].pid;
			resultPDRT->resultEdge[eCount].distanceEdge = close->list[quotient][remainder].dist;
			resultPDRT->resultEdge[eCount].idEdge = close->list[quotient][remainder].eid >> 1;
			eCount++;
		}
		remainder--;
	}

	return ERROR_SUCCESS;
}
