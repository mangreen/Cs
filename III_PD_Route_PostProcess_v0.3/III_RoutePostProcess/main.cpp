/*
*	CopyRight : ESTI_�굦�|�s�����z��
*	Description : III_Pedestrian-Route�ϸ����Y�D�{��
*	Version & Date : v0.3  2010/5/24
*/
#include "setting.h"
#include "dataProcess.h"

int main(int argc, char *argv[])
{
	char buf[512];
	//�ثe�ҭn�B�z����a or �ϸ�Ƭ� 1
	int count = 1;

	cout<<endl<<"Create RouteTable Start..."<<endl;

	ifstream clist("countrylist.txt", ios::_Nocreate);

	if(!clist.is_open()) //�b�ɮצ�m�̨S���^�����ɮ�
	{
		printf("Can't open countrylist.txt\n");
		exit(0);
	}
	
	Country *cHead = NULL;

	buf[0] = '\0';
		
	clist>>buf;  //Ū�X��a�W�� ex : tw

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
	 *	     �p��C��Node�P�F��Node���s�q��
	 *	parameter:
	 *	     none
	*/
	cHead->MakeBasicNodes();

	/*
	 *	Function:
	 *	     �Nedge���ݩʪ��bNode�íp��һݪ�offset�j�p
	 *	parameter:
	 *	     none
	*/
	cHead->MakeFullData();
		
	/*
	 *	Function:
	 *	     ����EdgeID������NodeID��Index
	 *	parameter:
	 *	     none
	*/
	cHead->MakeFullMisc();

	/*
	 *	Function:	
	 *	     �̷ӨC��Node�ҰO������T�A����RouteTable
	 *	parameter:
	 *	     none
	*/
	cHead->WriteFullData();

	delete cHead;
	
	cout<<endl<<"Create RouteTable End..."<<endl;
	return 0;
}