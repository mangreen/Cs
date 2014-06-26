#include <windows.h>       
#include "WinCEheader.h"
#include "III_Route_Manager.h"
#include "IdrLockRoad.h"
#include "III_Route_Indoor.h"


const TCHAR RouteTestName[] = TEXT ("RouteEngineTest");
HINSTANCE hInst;

#if defined(__cplusplus)
extern "C" {
#endif

extern III_FILE *performaceTestFile;

#if defined(__cplusplus)
}
#endif

const struct decodeUINT MainMessage[] = 
{
	WM_PAINT, DoRouteMain,
	WM_DESTROY, DoDestroyMain,
};

//======================================================================
//
// Program entry point
//
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPWSTR lpCmdLine, int nCmdShow) 
{
    MSG msg;
    HWND hwndMain;

    // Initialize this instance.
    hwndMain = InitInstance (hInstance, lpCmdLine, nCmdShow);
    if (0 == hwndMain)
	{
        return 0x10;
	}

    // Application message loop
    while (GetMessage (&msg, NULL, 0, 0)) 
	{
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    }
    // Instance cleanup
    return TermInstance (hInstance, msg.wParam);
}

//InitInstance
HWND InitInstance (HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;
	HWND hWnd;

	hInst = hInstance;
	
#if defined(WIN32_PLATFORM_PSPC)
	//If Pocket PC, allow only one instance of the application
	hWnd = FindWindow(RouteTestName, NULL);

	if(hWnd)
	{
		SetForegroundWindow ((HWND)(((DWORD)hWnd) | 0x01 ));
		return 0;
	}
#endif

	wc.style = 0; //windows style
	wc.lpfnWndProc = MainWndProc;  //callback function
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;   
	wc.hIcon = 0;
	wc.hCursor = LoadCursor (NULL, IDC_ARROW); //Default cursor
	wc.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = RouteTestName;

	if(0 == RegisterClass (&wc)) 
	{
		return 0;
	}

	//Create main window
	hWnd = CreateWindowEx (WS_EX_NODRAG, 
						   RouteTestName,                           //window class
						   TEXT("RouteEngineDEMO"),					//window title
						   WS_VISIBLE | WS_CAPTION | WS_SYSMENU,    //style flag
						   CW_USEDEFAULT,							//x position
						   CW_USEDEFAULT,							//y position
						   CW_USEDEFAULT,							//Initial width
						   CW_USEDEFAULT,							//Initial height
						   NULL,									//Parent
						   NULL,									//Menu
						   hInstance,								//Application instance
						   NULL);									//Pointer to create parameters

	//Return fail code if window not created
	if((!hWnd) || (!IsWindow (hWnd))) 
	{
		return 0;
	}

	//Standard show and update calls
	ShowWindow (hWnd, nCmdShow);
	UpdateWindow (hWnd);
	return hWnd;
}

int TermInstance (HINSTANCE hInstance, int nDefRC)
{
	return nDefRC;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	INT i;

	//Search message list to see if we need to handle this
	//message
	for (i = 0; i < dim(MainMessage); i++)
	{
		if( wMsg == MainMessage[i].Code)
		{
			return (*MainMessage[i].Fxn)(hWnd, wMsg, wParam, lParam);
		}
	}

	return DefWindowProc (hWnd, wMsg, wParam, lParam);
}

LRESULT DoRouteMain (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	
	III_RTPARAM *paramRounting = NULL;
	III_RTPOINT pntDeparture = {0}, pntArrival = {0};
	III_RESULTPDRT *resultPDRT = NULL;
	III_RESULTINDOOR *resultIdrRT = NULL;
	III_RESULTMTRT *resultMTRT = NULL;

	PAINTSTRUCT ps;
	HDC hdc;
	HBRUSH hbrOld;
	RECT rectCli;

	GetClientRect(hWnd, &rectCli);
	hdc = BeginPaint(hWnd, &ps);

	//MRT_TCH
	pntDeparture.pntOriginal.longitude = 121566713;//121566480;//121566857; 
	pntDeparture.pntOriginal.latitude = 25041025;//25041096;//25041267;
	pntDeparture.pntOriginal.height = -2;
	
	pntArrival.pntOriginal.longitude = 121565443;//121567194;
	pntArrival.pntOriginal.latitude = 25039294;//25041275;
	pntArrival.pntOriginal.height = -1;

	if((START_LOCK_ROAD_SUCCESS == startLockRoad(&pntDeparture, "\\Storage Card\\III_RouteTable\\INDOOR\\MRT_TCH_lock.bin")) &&
	   (START_LOCK_ROAD_SUCCESS == startLockRoad(&pntArrival,   "\\Storage Card\\III_RouteTable\\INDOOR\\MRT_TCH_lock.bin")))
	{
		paramRounting = (III_RTPARAM *)III_malloc(sizeof(III_RTPARAM));
		paramRounting->typeRouting = TYPE_ROUTE_INDOOR;
		paramRounting->modeRouting = MODE_INDOOR_SHORTEST;
		paramRounting->numFloorWalkMax = 0;
		paramRounting->tableRT.nameRoutingTable = "\\Storage Card\\III_RouteTable\\INDOOR\\MRT_TCH_indoor.bin";

		
		III_Indoor_SetRoutingParam(paramRounting);
		III_Indoor_Routing(pntDeparture, pntArrival);
		
		resultIdrRT = (III_RESULTINDOOR*)III_malloc(sizeof(III_RESULTINDOOR));   // �˭ӷs�Ŷ�;
		III_Indoor_GetIdrRTResult(resultIdrRT);

		III_INT i=0;//for loop

		printf("\n amount:%d  ", resultIdrRT->numNode);
		for(i=0; i<resultIdrRT->numNode; i++){
			printf("\nnodeID:%d, attr:%d", (resultIdrRT->resultNode + i)->nodeID, (resultIdrRT->resultNode + i)->attr);
			printf(" lon:%d, lat:%d, floor:%d", (resultIdrRT->resultNode + i)->cdnt.longitude, (resultIdrRT->resultNode + i)->cdnt.latitude, (resultIdrRT->resultNode + i)->cdnt.height);
			printf("->");
		}
		
		SetTextColor(hdc, 0x00000000);
		DrawText(hdc, TEXT("Indoor_Route_Test_File_Write_OK !"), -1, &rectCli, DT_CENTER | DT_SINGLELINE);
		SelectObject(hdc, hbrOld);
	}
	
	/*
	III_INT STARTTIME = 0; //FOR PERFORMANCE TEST
	III_INT ENDTIME = 0;   //FOR PERFORMANCE TEST
	III_CHAR buf[256] = ""; //FOR PERFORMANCE TEST

	PAINTSTRUCT ps;
	HDC hdc;
	HBRUSH hbrOld;
	RECT rectCli;
	
	GetClientRect(hWnd, &rectCli);

	hdc = BeginPaint(hWnd, &ps);
	
	III_RTPARAM *paramRounting = NULL;
	III_RTPOINT pntDeparture = {0}, pntArrival = {0};

	//InitResultPDRT
	III_RESULTPDRT *resultPDRT = NULL;
	resultPDRT = (III_RESULTPDRT *)III_malloc(sizeof(III_RESULTPDRT));
	paramRounting = (III_RTPARAM *)III_malloc(sizeof(III_RTPARAM));
	paramRounting->typeRouting = TYPE_ROUTE_PD;      //TYPE�]�w[PD, MASS, Indoor]
	paramRounting->modeRouting = MODE_PD_SHORTEST; //MODE�]�w[AVOID, ACCESSABLE, SHORTEST]
	//paramRounting->modeRouting = MODE_PD_RAIN_AVOID;
	//paramRounting->modeRouting = MODE_PD_ACCESSABLE;
	paramRounting->tableRT.nameRoutingTable = "\\Storage Card\\III_RouteTable\\routeTable.dat";  //RouteTable NAME[absolute path]
	paramRounting->tableRT.numRoutingTable = 1;                  //RouteTable number
	performaceTestFile = III_fopen("PerformanceListFile.txt", "w");
	
	//sprintf(buf, "III_PD_ROUTING_AstarInit_Phase - ��l���q Start !\n");   
	III_fwrite(buf, 1, strlen(buf), performaceTestFile);

	//
	//*		Function:
	//*			III_SetRoutingParam
	//*                   �̷�TYPE_ROUTE_PD�����P[See Route_Manager define]�I�s��H, �j��, �Ǥ��ɯ誺RouteEngine��l��
	//*		parameter:
	//*			III_RTPARAM *paramRouting - [in] Route �һݪ���T(ex. routetable name and number , typeRouting, modeRouting )
	//*			[out] see III_Route_Manager.h error definition
	//
	STARTTIME = GetTickCount();
	III_SetRoutingParam(paramRounting);
	ENDTIME = GetTickCount() - STARTTIME;
	//sprintf(buf, "API_III_SetRoutingParam_SPEND_TIME = %d \n\n", ENDTIME);   
	III_fwrite(buf, 1, strlen(buf), performaceTestFile);
	
	sprintf(buf, "��l���q�Ҫ�ɶ� = %d �@��\n\n", ENDTIME);   
	III_fwrite(buf, 1, strlen(buf), performaceTestFile);
	 
	GetNearEdge(  121616835,  25055833, &pntDeparture);

	GetNearEdge( 121566863 ,  25033045,   &pntArrival);
	

	//sprintf(buf, "III_PD_ROUTING_AstarSetStart&AstarSetEnd_Phase - �]�w���|�W���_,���I���q Start ! \n");   
	III_fwrite(buf, 1, strlen(buf), performaceTestFile);

	//
	//*		Function:
	//*			III_Routing
	//*                   �̷�TYPE_ROUTE_PD�����P[See Route_Manager define]�I�s��H, �j��, �Ǥ��ɯ誺�_,���I�]�w
	//*		parameter:
	//*			III_RTPOINT pntDeparture - [in] ��J�_�I�һݸ�� (����᪺edgeID, F,TNode�g�n��)
	//*			III_RTPOINT pntArrival - [in] ��J���I�I�һݸ�� (����᪺edgeID, F,TNode�g�n��)
	//*			[out] see III_Route_Manager.h error definition
	//
	STARTTIME = GetTickCount();
	III_Routing(pntDeparture, pntArrival);
	ENDTIME = GetTickCount() - STARTTIME;
	sprintf(buf, "API_III_Routing_SPEND_TIME = %d \n\n", ENDTIME);   
	III_fwrite(buf, 1, strlen(buf), performaceTestFile);
	sprintf(buf, "�]�w���|�W���_,���I���q�Ҫ�ɶ� = %d �@�� \n\n", ENDTIME);   
	III_fwrite(buf, 1, strlen(buf), performaceTestFile);

	//
 	//*		Function:
	//*			III_Routing
	//*                   �̷�TYPE_ROUTE_PD�����P[See Route_Manager define]�I�s��H, �j��, �Ǥ��ɯ誺A*�t��k�ñN���G(PathID & Path distance)�x�s�bIII_RESULTPDRT���c��
	//*		parameter:
	//*			III_RESULTPDRT *resultPDRT - [in] routing���G(pathID & path distance)�x�s�bIII_RESULTPDRT���c��
	//*			[out] see III_Route_Manager.h error definition
	//

	//sprintf(buf, "III_PD_ROUTING_AstarRouting_Phase - ����A* �t��k Start ! \n");   
	III_fwrite(buf, 1, strlen(buf), performaceTestFile);

	III_GetPdRTResult(resultPDRT);

	SetTextColor(hdc, 0x00000000);

	DrawText(hdc, TEXT("Pedestrian_Route_Test_File_Write_OK !"), -1, &rectCli, 
		     DT_CENTER | DT_SINGLELINE);

	SelectObject(hdc, hbrOld);
	//�̷�edge���Ӽ�, Output���G[path ID & path distance]
	int t = resultPDRT->numEdge - 1;
	int TotalDistance = 0;
	
	while (t >= 0 )
	{
		printf(" [RoadID] = %d OR", resultPDRT->resultEdge[t].idEdge - 1);
		t--;
		TotalDistance += resultPDRT->resultEdge[t].distanceEdge;
		//printf("TotalDistance = %d", resultPDRT->resultEdge[t].distanceEdge);
	}
	printf("\n TotalDistance = %d \n ", TotalDistance);
	EndPaint(hWnd, &ps);

	III_fclose(performaceTestFile);
	*/
	return 0;
}

LRESULT DoDestroyMain (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return 0;
}