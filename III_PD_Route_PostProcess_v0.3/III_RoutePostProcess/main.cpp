/*
*	CopyRight : ESTI_資策會新興智慧所
*	Description : III_Pedestrian-Route圖資壓縮主程式
*	Version & Date : v0.3  2010/5/24
*/
#include "setting.h"
#include "dataProcess.h"

int main(int argc, char *argv[])
{
	char buf[512];
	//目前所要處理的國家 or 圖資數為 1
	int count = 1;

	cout<<endl<<"Create RouteTable Start..."<<endl;

	ifstream clist("countrylist.txt", ios::_Nocreate);

	if(!clist.is_open()) //在檔案位置裡沒有擷取到檔案
	{
		printf("Can't open countrylist.txt\n");
		exit(0);
	}
	
	Country *cHead = NULL;

	buf[0] = '\0';
		
	clist>>buf;  //讀出國家名稱 ex : tw

	if(NULL == cHead)
	{
		cHead = new Country(count);
	}

	/*
	 *	Function:
	 *	     Parse *.mid, *.mif content
	 *	parameter:
	 *	     name - [in] Area or Conutry name
	*/
	cHead->Parsing(buf);
	
	/*
	 *	Function:
	 *	     計算每個Node與鄰近Node的連通性
	 *	parameter:
	 *	     none
	*/
	cHead->MakeBasicNodes();

	/*
	 *	Function:
	 *	     將edge的屬性附在Node並計算所需的offset大小
	 *	parameter:
	 *	     none
	*/
	cHead->MakeFullData();
		
	/*
	 *	Function:
	 *	     產生EdgeID對應到NodeID的Index
	 *	parameter:
	 *	     none
	*/
	cHead->MakeFullMisc();

	/*
	 *	Function:	
	 *	     依照每個Node所記錄的資訊，產生RouteTable
	 *	parameter:
	 *	     none
	*/
	cHead->WriteFullData();

	delete cHead;
	
	cout<<endl<<"Create RouteTable End..."<<endl;
	return 0;
}