/*
*	CopyRight : ESTI_�굦�|�s�����z��
*	Description : III_Pedestrian-Route Funtion�w�q
*	Version & Date : v0.3  2010/5/24
*/
#ifndef BASICDS_H
#define BASICDS_H

#include "setting.h"

const int MAX = 4096;               //max allowed edges
const int SHIFT = 12;               //�ϥ�shift �٭�^�쥻��EdgeID
const int FULLDATASIZE = 16;        //each entry 16 bytes
const int DXDYSCALE = 64;           //apply on full data & x,y encoder
const int MAPSCALE = 1;             //base scale: 10^6
const int HEADERSIZE = 12;          //define routetable header size

struct Edge 
{	
	//basic edge struct
	int tnode;					//tnode : tnodeID
	int fnode;					//fnode : fnodeID
	int stair;					//stair : �O�_���ӱ�
	int shelter;				//shelter : �O�_���B�ת�
	int accessable;				//accessable : �O�_���L��ê�Ŷ�
	unsigned short dist;		//���qedge���Z�� (���:����)
	unsigned int sp;			//nid offset(25 bits) + num(3 bits) + reserved (4 bits)

	//��l��
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
	int count;		//count�Gfull Data�̪�edge�Ӽ�
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
	unsigned int offset;  //�O���C��Node��offset
	unsigned int eid;     //�O���C��Node��EdgeID

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
	int x;					//�O��Node���g��
	int y;					//�O��Node���n��
	unsigned int *eid;		//�O���C��Node��EdgeID
	unsigned int offset;	//�O���C��Node��offset
	char count;				//�O���C��Node�P�F��Node���s�q�Ӽ�
	char rcount;			//�O���C��Node�P�F��Node���s�q�Ӽ�
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
	 *	     �O���C��Node��edge ID�ñN�s�q�ƥ[1    
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
	int maxNode;		 //�O���̤j��NodeID
	int maxEdge;		 //�O��Edge���`�Ӽ�
	int edgeCount;		 //edge count
	int cFullDataOffset; //�ˬd�O�_��16 bytes �j�p
	int fullSize;		 //Full Data size

	char cName[128];     //RouteTable�W��
	char midName[128];   //RouteTable.mid �ɮצW��
	char mifName[128];   //RouteTable.mif �ɮצW��

	ifstream fileMid;    //I/O Input stream (for mid file)
	ifstream fileMif;    //I/O Input stream (for mif file)
	EdgeInfo **arr;       //�x�s�C��edge��info
	Node *all;           //��Node���x�sedge info 	
	Country *next;       //�O�_���U�@�ӹϸ�

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
	 *	     �p��C��Node�P�F��Node���s�q��
	 *	parameter:
	 *	     none
	*/
	void MakeBasicNodes();

	/*
	 *	Function:
	 *	     �Nedge���ݩʪ��bNode�íp��һݪ�offset�j�p
	 *	parameter:
	 *	     none
	*/
	void MakeFullData();

	/*
	 *	Function:
	 *	     ����EdgeID������NodeID��Index
	 *	parameter:
	 *	     none
	*/
	void MakeFullMisc();

	/*
	 *	Function:	
	 *	     �̷ӨC��Node�ҰO������T�A����RouteTable
	 *	parameter:
	 *	     none
	*/
	void WriteFullData();

	/*
	 *	Function:
	 *	     ���oEdge���c�̪����e
	 *	parameter:
	 *	     eid - [in] edge id
	*/
	Edge* GetEdge(int);
};

/*
*	Function:
*	     �NRouteTable�g�J��s�}����Ƨ���
*	parameter:
*	     dir - [in] ��Ƨ�
*		 fname - [in] area or country
*        mode - [in] wb or w or r ..  �g�J��Ū�X
*        fp - [in] �ɮ׫���
*/
void OpenFile(const char*, char*, char*, FILE**);

#endif