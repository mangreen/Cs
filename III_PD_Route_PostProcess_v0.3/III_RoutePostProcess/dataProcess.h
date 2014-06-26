/*
*	CopyRight : ESTI_資策會新興智慧所
*	Description : III_Pedestrian-Route Funtion定義
*	Version & Date : v0.3  2010/5/24
*/
#ifndef BASICDS_H
#define BASICDS_H

#include "setting.h"

const int MAX = 4096;               //max allowed edges
const int SHIFT = 12;               //使用shift 還原回原本的EdgeID
const int FULLDATASIZE = 16;        //each entry 16 bytes
const int DXDYSCALE = 64;           //apply on full data & x,y encoder
const int MAPSCALE = 1;             //base scale: 10^6
const int HEADERSIZE = 12;          //define routetable header size

struct Edge 
{	
	//basic edge struct
	int tnode;					//tnode : tnodeID
	int fnode;					//fnode : fnodeID
	int stair;					//stair : 是否為樓梯
	int shelter;				//shelter : 是否為遮避物
	int accessable;				//accessable : 是否為無障礙空間
	unsigned short dist;		//此段edge的距離 (單位:公尺)
	unsigned int sp;			//nid offset(25 bits) + num(3 bits) + reserved (4 bits)

	//初始化
	Edge() 
	{ 
		tnode = 0;
		fnode = 0;
		stair = 0;
		shelter = 0;
		accessable = 0;
		sp = 0;
		dist = 0;
	}
	~Edge() 
	{ 

	}

};

struct EdgeInfo 
{
	int count;		//count：full Data裡的edge個數
	Edge *edge;

	EdgeInfo() 
	{ 
		count = 0; 
		edge = new Edge[MAX]; 
	}
	~EdgeInfo() 
	{ 
		delete [] edge; 
	}
};

struct FullOutput 
{
	unsigned int offset;  //記錄每個Node的offset
	unsigned int eid;     //記錄每個Node的EdgeID

	FullOutput() 
	{ 
		offset = 0; 
		eid = 0;
	}
	~FullOutput()
	{ 
		 
	}
	
};

struct Node 
{ 
	int x;					//記錄Node的經度
	int y;					//記錄Node的緯度
	unsigned int *eid;		//記錄每個Node的EdgeID
	unsigned int offset;	//記錄每個Node的offset
	char count;				//記錄每個Node與鄰近Node的連通個數
	char rcount;			//記錄每個Node與鄰近Node的連通個數
	FullOutput *fout;

	//basic node struct
	Node() 
	{
		x = 0; 
		rcount = 0;
        count = 0; 
		offset = 0;
		eid = NULL; 
		fout = NULL;
	}
	~Node() 
	{
		delete [] eid;
	}
	
	/*
	 *	Function:
	 *	     記錄每個Node的edge ID並將連通數加1    
	 *	parameter:
	 *	     e - [in] edge ID
	*/
	void AddEdge(int e) 
	{ 
		eid[count++] = (unsigned int)e; 
	}
	
};

struct Country 
{ 
	int maxNode;		 //記錄最大的NodeID
	int maxEdge;		 //記錄Edge的總個數
	int edgeCount;		 //edge count
	int cFullDataOffset; //檢查是否為16 bytes 大小
	int fullSize;		 //Full Data size

	char cName[128];     //RouteTable名稱
	char midName[128];   //RouteTable.mid 檔案名稱
	char mifName[128];   //RouteTable.mif 檔案名稱

	ifstream fileMid;    //I/O Input stream (for mid file)
	ifstream fileMif;    //I/O Input stream (for mif file)
	EdgeInfo **arr;       //儲存每條edge的info
	Node *all;           //用Node來儲存edge info 	
	Country *next;       //是否有下一個圖資

	//basic country struct
	Country(int cid) 
	{
		arr = new EdgeInfo*[MAX]; //MAX*MAX = max allowed edges
		arr[0] = new EdgeInfo();
		edgeCount = 0;
		maxNode = 0; 
		maxEdge =0;
		all = NULL;  
		next = NULL;
		cid = 0;
		fullSize = 0; 
	}

	~Country() 
	{

	}

	/*
	 *	Function:
	 *	     Parse *.mid, *.mif content
	 *	parameter:
	 *	     name - [in] Area or Conutry name
	*/
	void Parsing(char*);
		
	/*
	 *	Function:
	 *	     Parse *.mid, *.mif content
	 *	parameter:
	 *	     none
	*/
	void MidAnalysis(char*, Edge&, int);

	/*
	 *	Function:
	 *	     計算每個Node與鄰近Node的連通性
	 *	parameter:
	 *	     none
	*/
	void MakeBasicNodes();

	/*
	 *	Function:
	 *	     將edge的屬性附在Node並計算所需的offset大小
	 *	parameter:
	 *	     none
	*/
	void MakeFullData();

	/*
	 *	Function:
	 *	     產生EdgeID對應到NodeID的Index
	 *	parameter:
	 *	     none
	*/
	void MakeFullMisc();

	/*
	 *	Function:	
	 *	     依照每個Node所記錄的資訊，產生RouteTable
	 *	parameter:
	 *	     none
	*/
	void WriteFullData();

	/*
	 *	Function:
	 *	     取得Edge結構裡的內容
	 *	parameter:
	 *	     eid - [in] edge id
	*/
	Edge* GetEdge(int);
};

/*
*	Function:
*	     將RouteTable寫入到新開的資料夾裡
*	parameter:
*	     dir - [in] 資料夾
*		 fname - [in] area or country
*        mode - [in] wb or w or r ..  寫入或讀出
*        fp - [in] 檔案指標
*/
void OpenFile(const char*, char*, char*, FILE**);

#endif