/*
*	CopyRight : ESTI_資策會新興智慧所
*	Description : III_Pedestrian-Route圖資壓縮程式
*   Version & Date : v0.3  2010/5/24
*/

#include "dataProcess.h"
#include "setting.h"

/*
*	Function:
*	     Parse *.mid, *.mif content
*	parameter:
*	     name - [in] Area or Conutry name
*/
void Country::Parsing(char *name)
{
	char buf[512];			//mid file row data

	strcpy(cName, name);
	strcpy(midName, name); 
	strcat(midName, MIDNAME);

	strcpy(mifName, name); 
	strcat(mifName, MIFNAME);

	fileMid.open(midName, ios::_Nocreate); //開啟 *.mid檔案
	fileMif.open(mifName, ios::_Nocreate); //開啟 *.mif檔案

	if(!fileMid.is_open() || !fileMif.is_open()) 
	{
		printf("Can't open %s mid or mif file\n", cName);
		exit(0);
	}
	
	printf("parsing %s <mid> <mif> data...\n", cName);

	while(stricmp(buf, "DATA")!=0)
	{
		fileMif.getline(buf, 512); 
	}
	
	while(1) 
	{
		buf[0] = '\0';
		fileMid.getline(buf, 512);
		
		if(0 == strlen(buf)) 
		{
			break;
		}

		if(MAX == arr[edgeCount]->count)
		{
			arr[++edgeCount] = new EdgeInfo();
		}
		
		/*
		 *	Function:
		 *	     Parse *.mid, *.mif content
		 *	parameter:
		 *	     none
		*/
		MidAnalysis(buf, arr[edgeCount]->edge[arr[edgeCount]->count], ++maxEdge);
		arr[edgeCount]->count++;
	}

	if(maxNode>maxEdge) 
	{
		printf("Error003: %s Node Id(%d) too large!\n", midName, maxNode);
		exit(0);
	}

	if(maxNode>maxEdge) 
	{
		printf("Error003: %s Node Id(%d) too large!\n", midName, maxNode);
		exit(0);
	}
	fileMid.close();
	fileMif.close();
} //end of Parsing


/*
*	Function:
*	     計算每個Node與鄰近Node的連通性
*	parameter:
*	     none
*/
void Country::MakeBasicNodes()
{
	int i;				//迴圈count
	int j;				//迴圈count	
	Edge *cur;          
	//make basic nodes
	all = new Node[maxNode+1]; //skip 0

	//maxEdge : MID 檔案裡的row Data個數
	for(i=0, j=-1;i<maxEdge;i++)
	{
		if(0 == i%MAX) 
		{
			cur = &arr[++j]->edge[0];
		}
		else
		{
			cur = &arr[j]->edge[i%MAX];
		}

		all[cur->fnode].count++;
		all[cur->tnode].count++;
	}

	for(i=0;i<=maxNode;i++)
	{
		//如果同一個node上有一個edge以上
		if(all[i].count) 
		{
			//這個node的eid陣列大小為edge個數
			all[i].eid = new unsigned int[all[i].count];
			//將這個node count清空
			all[i].count = 0;
		}
	}

	//maxEdge : 每條edge (MID資料個數)
	for(i=0, j=-1;i<maxEdge;i++)
	{
		if(0 == i%MAX) 
		{
			cur = &arr[++j]->edge[0];
		}
		else
		{
			cur = &arr[j]->edge[i%MAX];
		}

		//加入 fnode or tnode edge
		if(all[cur->fnode].count<7 && all[cur->tnode].count<7)
		{
			/*
			*	Function:
			*	     記錄每個Node的edge ID並將連通數加1    
			*	parameter:
			*	     i - [in] edge ID
			*/
			all[cur->fnode].AddEdge(i*2+2); //2k, note that k = i+1
			all[cur->tnode].AddEdge(i*2+3); //2k+1
		}
		else
		{
			printf("Error007: Degree too large, line %d is discarded\n", i+1);
		}
	}

	for(i=0;i<=maxNode;i++)
	{
		//檢查是否有連通性大於8的Node
		if(all[i].count>=8)
		{
			for(j=0;j<all[i].count;j++)
			{
				cout<<"eid="<<all[i].eid[j]/2 <<endl;
			}
			exit(0);
		}
	}

} //end of MakeBasicNodes


/*
*	Function:
*	     Parse *.mid, *.mif content
*	parameter:
*	     none
*/
void Country::MidAnalysis(char *buf, Edge &edge, int curLine)
{
	char tarr[64][128];				//存放著mid's row data
	int c = 1;						//array count
	int p = 0;						//array count
	int p2 = 0;						//array count
	int len = strlen(buf);			//mid each row data length
	int i;							//迴圈count
	int n = 0;                      //x,y (經緯度)的個數                          
	int *ax;                        //x (經度) 暫存陣列
	int *ay;                        //y (緯度) 暫存陣列
	int mapscale2 = MAPSCALE/2;     //依10^6 來縮放Map 比例
	double x = 0;                   //實際的x (經度)
	double y = 0;					//實際的y (緯度)
	double dist = 0;                //實際edge 長度距離
	char buf2[256];                 //存目前讀出的edge是屬於Line or PLine 

	//len 為 buf的長度
	while(p<len)
	{
		p2 = 0;

		//將, ,之間的字串記錄下來
		//tarr[c] => c 為每個, , 資料值個數
		while(buf[p]!=',' && p<len)
		{
			tarr[c][p2++] = buf[p++];
		}

		tarr[c][p2] = '\0';
		c++; 
		p++;
	}
	
	//將每個row data裡的fnode, tnode整數值取出來放
	edge.fnode = atoi(tarr[F_NODE_NUM]);
	edge.tnode = atoi(tarr[T_NODE_NUM]);

	//將最大的fnode 路段起始點編號, tnode 路段終止點編號 MaxNode 記錄下來(Default 為0)
	if(edge.fnode>maxNode)
	{
		maxNode = edge.fnode;
	}

	if(edge.tnode>maxNode) 
	{
		maxNode = edge.tnode;
	}

	edge.stair = atoi(tarr[IS_STAIR]);					//是否有樓梯
	edge.shelter = atoi(tarr[IS_SHELTER]);				//是否有遮敞物
	edge.accessable = atoi(tarr[IS_ACCESSABLE]);		//是否為無障礙空間
	//讀出Mif info.
	fileMif>>buf2;

	//當buf2 = PLINE時, 返回值為0
	if(0 == stricmp(buf2, "PLINE")) 
	{ 
		fileMif>>n; 
	}
	else if(0 == stricmp(buf2, "LINE")) 
	{ 
		n = 2; 
	}
	else 
	{ 
		printf("Error002: %s %d's pline invalid!\n", mifName, curLine); 
		exit(0); 
	}

	ax = new int[n]; 
	ay = new int[n];

	//將實際的x,y暫存至ax, ay陣列
	for(i=0;i<n;i++) 
	{
		fileMif>>x>>y;
		ax[i] = (int)(x*1000000+mapscale2)/MAPSCALE;
		ay[i] = (int)(y*1000000+mapscale2)/MAPSCALE;
	}
	
	//if <mid> doesn't support "distance"，由edge上的兩兩經緯度位置來計算距離
	for(dist=0.0f,i=1;i<n;i++) 
	{
		x = (ax[i]-ax[i-1]);
		y = (ay[i]-ay[i-1]);
		dist+=sqrt(x*x+y*y);
	}

	dist*=MAPSCALE;
	dist = (dist+5)/11; //suppose earth radis = 6378km

	if(dist >= 65535.0f) 
	{ 
		printf("Error001: distance overflow\n"); 
		exit(0); 
	}
	edge.dist = (unsigned short)dist;

	delete [] ax;
	delete [] ay;
} //end of MidAnalysis

/*
*	Function:
*	     將edge的屬性附在Node並計算所需的offset大小
*	parameter:
*	     none
*/
void Country::MakeFullData()
{
	int i;						//迴圈count
	int j;						//迴圈count
	int nodeId;					//Node ID
	int eEdgeId = 0;			//2k or 2k + 1後的 EdgeID		
	int dEdgeId = 0;			//GIS edgeID
	int offset = HEADERSIZE;	//headersize 先保留header所需大小
	char buf[256];              //輸出edge總個數及最大的NodeID

	for(i=0;i<=maxNode;i++) //prepare output data
	{
		//all[i].count => 每個node的edge個數
		if(all[i].count>0)
		{
			all[i].fout = new FullOutput[all[i].count];
		}

		for(j=0;j<all[i].count;j++)
		{
			eEdgeId = all[i].eid[j];
			dEdgeId = eEdgeId>>1;
	
			/*
			*	Function:
			*	     取得Edge結構裡的內容
			*	parameter:
			*	     eid - [in] edge id
			*/
			Edge *ptrEdge = GetEdge(dEdgeId);

			if(0 == eEdgeId%2) 
			{ 
				nodeId = ptrEdge->tnode; 
			}
			else 
			{ 
				nodeId = ptrEdge->fnode; 
			}

			all[i].fout[j].offset = (nodeId<<3);

			if(0 == eEdgeId%2) 
			{
				eEdgeId++;
			}
			else
			{
				eEdgeId--;
			}

			all[i].fout[j].eid = (eEdgeId<<8); 
			
		} //end of for(j) loop
	} //end of for(i) loop

	//記錄fullData的offset(size)
	for(i=0; i<=maxNode ;i++)
	{
		int flag = 1;

		if(1 == all[i].count) 
		{
			all[i].offset = 0;
			flag = 0;
			//else: though count=1, but we keep those nodes, bcz it may connect to other countries
		}

		//當all.count > 1
		if(flag)
		{
			all[i].offset = offset;

			for(j=0;j<all[i].count;j++)
			{
				offset+=FULLDATASIZE;
				all[i].rcount++;
			}
		}

	}

	if(offset>0x7ffffff) //0x7ffffff = 128MB
	{ 
		printf("Error008: full data offset is larger than 128MB\n");
		exit(0);
	}

	fullSize = offset;

	if(4 == ( cFullDataOffset = fullSize %8 )) //滿足4 Bytes格式
	{
		fullSize+=4;
	}
	
	//make real output offset
	for(i=0;i<=maxNode;i++)
	{
		for(j=0;j<all[i].count;j++)
		{
			nodeId = (all[i].fout[j].offset>>3); 
			//note: all[k].offset<<5 bcz offset is divided by 4, so <<7 becomes <<5
			all[i].fout[j].offset = (all[nodeId].offset<<5) + (all[nodeId].rcount<<4); 
		}
	}

	sprintf(buf, "edge(%d) node(%d)", maxEdge, maxNode);
	cout<<buf<<endl<<endl;
}//end of MakeFullData


/*
*	Function:
*	     將RouteTable寫入到新開的資料夾裡
*	parameter:
*	     dir - [in] 資料夾
*		 fname - [in] area or country
*        mode - [in] wb or w or r ..  寫入或讀出
*        fp - [in] 檔案指標
*/
void OpenFile(const char *dir, char *fname, char *mode, FILE **fp)
{
	*fp = fopen(fname, mode);

	if(NULL == *fp) 
	{
		char buf2[64];
		sprintf(buf2, "mkdir %s", dir);
		system(buf2);
		*fp = fopen(fname, mode);

		if(NULL == *fp) 
		{
			printf("Error005: Can't make %s file!\n", fname);
			exit(0);
		}
	}
}//end of OpenFile


/*
*	Function:
*	     取得Edge結構裡的內容
*	parameter:
*	     eid - [in] edge id
*/
Edge* Country::GetEdge(int eid)
{
	//SHIFT : 12位元, 2的12位元為4096
	return &arr[(eid-1)>>SHIFT]->edge[(eid-1)%MAX];
} //end of GetEdge


/*
*	Function:
*	     產生EdgeID對應到Full data NodeID的Index(Full misc to Full data)
*	parameter:
*	     none
*/
void Country::MakeFullMisc()
{ 
	int i;				//迴圈count
	int j;				//迴圈count

	for(i=1;i<=maxEdge;i++)
	{
		/*
		 *	Function:
		 *	     取得Edge結構裡的內容
		 *	parameter:
		 *	     i - [in] edge id
		*/
		Edge *ptrEdge = GetEdge(i);

		Node *nf=&all[ptrEdge->fnode];
		Node *nt=&all[ptrEdge->tnode];

		for(j=0;j<nf->count;j++)
		{
			if((int)(nf->fout[j].eid>>9)==i)
			{
				if(nf->offset!=0)
				{
					ptrEdge->sp = ((nf->offset<<5)+(nf->rcount<<4));
					j = nf->count;
				}
			}
		}

		for(j=0;j<nt->count;j++)
		{
			if((int)(nt->fout[j].eid>>9)==i)
			{
				if(nt->offset!=0)
				{
					ptrEdge->sp = (nt->offset<<5) + (nt->rcount<<4);
					j = nt->count;
				}
			}
		}

		if(0 == (ptrEdge->sp>>4))
		{
			printf("Warning002: eid=%d is isolated edge or oneway-dead-end edge\n", i);
		}
	} 
}//end of MakeFullMisc

/*
*	Function:
*	     依照每個Node所記錄的資訊，產生RouteTable
*	parameter:
*	     none
*/
void Country::WriteFullData()
{
	int i;					//迴圈count
	int j;					//迴圈count	
	int gap = 0;			//edge的長度距離
	int offset = 0;         //各edge的offset
	char buf[256];          //輸出的RouteTable名稱
	char ch;				//寫入 header資料(DXDYSCALE, MAPSCALE)
	int eEdgeId = 0;	    //2k or 2k + 1後的 EdgeID		
	int dEdgeId = 0;        //GIS edgeID
	FILE *write = NULL;     //RouteTable file pointer
	FullOutput *out;        //每一個Node所記錄的資訊Pointer
	Node *nf, *nt;          //Node pointer
	short delta = 0;        //variable gap other type		
	short PD_Attr = 0;		//pedestrian attribute

	sprintf(buf, "./%s/%s%s", OUTPUTDIR, cName, OUTPUTNAME);
	OpenFile(OUTPUTDIR, buf, "wb", &write);

	offset = HEADERSIZE; 				//offset從header之後開始算起
	fwrite(&maxEdge, 4, 1, write); 		//寫入Edge的總數
	fwrite(&fullSize, 4, 1, write);		//fullSize 為header + full data 的總和
	ch = (char)DXDYSCALE;				//經緯度encoder比例
	fwrite(&ch,1, 1, write);
	ch = (char)MAPSCALE;                //經緯度encoder比例
	fwrite(&ch,1, 1, write);
	fwrite("PD", 2 ,1, write);			//寫入檢查碼
		
	//依每個node來處理資料
	for(i=0;i<=maxNode;i++)
	{
		for(j=0;j<all[i].count;j++)
		{
			if(all[i].offset>0)
			{
				out = &all[i].fout[j];
				fwrite(&out->offset, 4, 1, write); //See spec. Full Data part A. 
				fwrite(&out->eid, 4, 1, write);    //See spec. Full Data part B.
				eEdgeId = all[i].eid[j];
				dEdgeId = eEdgeId>>1;

				/*
				 *	Function:
				 *	     取得Edge結構裡的內容
				 *	parameter:
				 *	     eid - [in] edge id
				*/
				Edge *ptrEdge = GetEdge(dEdgeId);
				nf=&all[ptrEdge->fnode]; 
				nt=&all[ptrEdge->tnode];
				//See spec. Full Data part C. 寫入dx, dy, dist
				gap = (nt->x-nf->x);

				if(gap>=0) 
				{
					gap+=DXDYSCALE/2;
				}
				else
				{
					gap-=DXDYSCALE/2;
				}
				gap/=DXDYSCALE;

				if(gap>32767 || gap<-32768) 
				{
					printf("Error006: node %d to %d (eid = %d), dx overflow!\n", ptrEdge->fnode, ptrEdge->tnode, i);
					exit(0);
				}
				else 
				{
					delta = (short)gap;
					fwrite(&delta, 2, 1, write);
				}

				gap = (nt->y-nf->y);

				if(gap>=0) 
				{
					gap+=DXDYSCALE/2;
				}
				else
				{
					gap-=DXDYSCALE/2;
				}
				gap/=DXDYSCALE;

				if(gap>32767 || gap<-32768) 
				{
					printf("Error006: node %d to %d (eid = %d), dy overflow!\n", ptrEdge->fnode, ptrEdge->tnode, i);
					exit(0);
				}
				else 
				{
					delta = (short)gap;
					fwrite(&delta, 2, 1, write);
				}
				fwrite(&ptrEdge->dist, 2, 1, write);
				
				PD_Attr = -1;
				//開始寫入並判斷edge's pedestrian attribute
				if(1 == ptrEdge->shelter)
				{
					PD_Attr = 1;
				}

				if(1 == ptrEdge->accessable)
				{
					PD_Attr = 2;
				}

				if(1 == ptrEdge->stair)
				{
					PD_Attr = 4;
				}

				if(1 == ptrEdge->shelter && 1 == ptrEdge->accessable)
				{
					PD_Attr = 3;
				}

				if(1 == ptrEdge->shelter && 1 == ptrEdge->stair)
				{
					PD_Attr = 5;
				}

				if(1 == ptrEdge->accessable && 1 == ptrEdge->stair)
				{
					PD_Attr = 6;
				}

				if(1 == ptrEdge->accessable && 1 == ptrEdge->stair && 1 == ptrEdge->shelter)
				{
					PD_Attr = 7;
				}

				//SIDEWALK
				if(0 == ptrEdge->accessable && 0 == ptrEdge->stair && 0 == ptrEdge->shelter)
				{
					PD_Attr = 0;
				}
				
				if(-1 == PD_Attr)
				{
					printf("ERROR_PEDESTRIAN_ATTRIBUTE");
					exit(0);
				}
				fwrite(&PD_Attr, 2 , 1 ,write);

				offset+=FULLDATASIZE; 	//Every Full Data 16 Bytes
			}
		}
	} //end of for(i)
	
	if(4 == cFullDataOffset) 
	{
		offset+=4;
		i = 0;
		fwrite(&i, 4, 1, write);
	}

	if(offset!=fullSize) 
	{
		printf("Error: Unknown fetal error!\n");
		exit(0);
	}
	
	//寫入full misc 資料
	for(i=1;i<=maxEdge;i++)
	{
		/*
		 *	Function:
		 *	     取得Edge結構裡的內容
		 *	parameter:
		 *	     i - [in] edge id
		*/
		Edge *ptrEdge = GetEdge(i);

		//寫入 edgeID對應到nodeID的Index
		fwrite(&ptrEdge->sp, 4, 1, write);
	}

	fclose(write);
}//end of WriteFullData