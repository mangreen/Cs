/*
*	CopyRight : ESTI_�굦�|�s�����z��
*	Description : III_Pedestrian-Fusion Routing ��~����API
*	Version & Date : v0.3  2011/4/8
*/

#include "III_RouteAPI.h"
#include "III_Fusion_RouteAPI.h"
#include "IdrPOISearch.h"
#include "QueryBuilding.h"
#include "IdrLockRoad.h"
#include "III_Route_Indoor.h" 

III_FusionRouteAPI::III_FusionRouteAPI()
{
    IIIRoute[0] = IIIRoute[1] = IIIRoute[2] = NULL; 
    resultFusionRT = NULL;
}

III_FusionRouteAPI::~III_FusionRouteAPI(void)
{
    DeleteIIIRouteObject();   
}

III_INT III_FusionRouteAPI::III_SetFusionRoutingParam(III_RTPARAM *paramRounting)
{
	GlobalparamRounting = paramRounting;

	return ERROR_SUCCESS;
}

/*
*		Function:
*			III_Routing
*                   �̷�TYPE_ROUTE_PD�����P[See Route_Manager define]�I�s��H, �j��, �Ǥ��ɯ誺�_,���I�]�w
*		parameter:
*			III_RTPOINT pntDeparture - [in] ��J�_�I�һݸ�� (����᪺edgeID, F,TNode�g�n��)
*			III_RTPOINT pntArrival - [in] ��J���I�I�һݸ�� (����᪺edgeID, F,TNode�g�n��)
*			[out] see III_Route_Manager.h error definition
*/
III_INT III_FusionRouteAPI::III_FusionRouting(III_POINT Departure, III_POINT Arrival)
{
    char sDepartureBuilding[20]; 
    char sArrivalBuilding[20]; 
    III_INT result_process;
    
    bool IsDepartureIndoor = QueryBuildingString(Departure.longitude, Departure.latitude, sDepartureBuilding);
    bool IsArrivalIndoor = QueryBuildingString(Arrival.longitude, Arrival.latitude, sArrivalBuilding);
    
    DeleteIIIRouteObject(); 
    NewResultFusionRT(); 
        
    if(IIIRoute[0] == NULL)
        IIIRoute[0] = new III_RouteAPI; 
       
    if(IsDepartureIndoor == true && IsArrivalIndoor == true) { // Indoor              
        if(strcmp(sDepartureBuilding, sArrivalBuilding) == 0) { // Same Building    
            result_process = III_IndoorRoutingFunction(IIIRoute[0], Departure, Arrival, sDepartureBuilding);   
        } else { // Difference Building
            III_POINT pntMiddle, pntMiddle2;
            char sMiddleBuilding[20]; 
            pntMiddle.longitude = (Departure.longitude + Arrival.longitude) / 2;
            pntMiddle.latitude = (Departure.latitude + Arrival.latitude) / 2;      
            bool IsMiddleIndoor = QueryBuildingString(pntMiddle.longitude, pntMiddle.latitude, sMiddleBuilding);
            if(IsMiddleIndoor == true) { // Indoor Both  
                pntMiddle = Arrival;
                pntMiddle.height = 1;         
                IdrPOISearch(&pntMiddle.longitude, &pntMiddle.latitude, pntMiddle.height, "exit", sDepartureBuilding);
                result_process = III_IndoorRoutingFunction(IIIRoute[0], Departure, pntMiddle, sDepartureBuilding); 
                if(IIIRoute[1] == NULL)
                    IIIRoute[1] = new III_RouteAPI;  
                
                pntMiddle2 = Departure;
                pntMiddle2.height = 1;         
                IdrPOISearch(&pntMiddle2.longitude, &pntMiddle2.latitude, pntMiddle2.height, "exit", sArrivalBuilding);   
                result_process = result_process | III_IndoorRoutingFunction(IIIRoute[1], pntMiddle2, Arrival, sArrivalBuilding);   
            } else { // Indoor_Outdoor_Indoor
                pntMiddle = Arrival; // Indoor
                pntMiddle.height = 1;         
                IdrPOISearch(&pntMiddle.longitude, &pntMiddle.latitude, pntMiddle.height, "exit", sDepartureBuilding);
                result_process = III_IndoorRoutingFunction(IIIRoute[0], Departure, pntMiddle, sDepartureBuilding); 
                if(IIIRoute[1] == NULL) // Outdoor
                    IIIRoute[1] = new III_RouteAPI;  
                
                pntMiddle2 = Departure;
                pntMiddle2.height = 1;         
                IdrPOISearch(&pntMiddle2.longitude, &pntMiddle2.latitude, pntMiddle2.height, "exit", sArrivalBuilding);    
                result_process = result_process | III_OutdoorRoutingFunction(IIIRoute[1], pntMiddle, pntMiddle2);               
                
                if(IIIRoute[2] == NULL) // Indoor
                    IIIRoute[2] = new III_RouteAPI;  
                                  
                result_process = result_process | III_IndoorRoutingFunction(IIIRoute[2], pntMiddle2, Arrival, sArrivalBuilding);      
            } 
        }   
    } else if(IsDepartureIndoor == true && IsArrivalIndoor == false) { // Indoor First
        III_POINT pntMiddle = Arrival;
        pntMiddle.height = 1;
        IdrPOISearch(&pntMiddle.longitude, &pntMiddle.latitude, pntMiddle.height, "exit", sDepartureBuilding);
        result_process = III_IndoorRoutingFunction(IIIRoute[0], Departure, pntMiddle, sDepartureBuilding);  
        if(IIIRoute[1] == NULL)
            IIIRoute[1] = new III_RouteAPI;  
                   
        result_process = result_process | III_OutdoorRoutingFunction(IIIRoute[1], pntMiddle, Arrival);        
    } else if(IsDepartureIndoor == false && IsArrivalIndoor == true) { // Outdoor First
        III_POINT pntMiddle = Departure;
        pntMiddle.height = 1;
        IdrPOISearch(&pntMiddle.longitude, &pntMiddle.latitude, pntMiddle.height, "exit", sArrivalBuilding);
        result_process = III_OutdoorRoutingFunction(IIIRoute[0], Departure, pntMiddle); 
        if(IIIRoute[1] == NULL)
            IIIRoute[1] = new III_RouteAPI;  
            
        result_process = result_process | III_IndoorRoutingFunction(IIIRoute[1], pntMiddle, Arrival, sArrivalBuilding);                  
    } else { // Outdoor
	    result_process = III_OutdoorRoutingFunction(IIIRoute[0], Departure, Arrival);
    }
	
	return result_process;
}

extern char g_szMapFolder[256];

III_INT III_FusionRouteAPI::III_OutdoorRoutingFunction(III_RouteAPI *IIIRoute, III_POINT Departure, III_POINT Arrival)
{
    III_RTPOINT pntLockDeparture = {0}, pntLockArrival = {0};
    III_INT result_process = ERROR_PD_ROUTE_OPENROUTEFILE;
  
	char pRtTablePath[256];

	strcpy(pRtTablePath, g_szMapFolder);
	strcat(pRtTablePath, "III_RouteTable\\routeTable.dat");           

    GlobalparamRounting->typeRouting = TYPE_ROUTE_PD;      //TYPE�]�w[PD, MASS, Indoor]
	GlobalparamRounting->tableRT.nameRoutingTable = pRtTablePath;  //RouteTable NAME[absolute path]
	GlobalparamRounting->tableRT.numRoutingTable = 1;                  //RouteTable number	
	IIIRoute->III_SetRoutingParam(GlobalparamRounting);
	//�]�w�_�I
	IIIRoute->GetNearEdge(Departure.longitude, Departure.latitude, &pntLockDeparture);
	//�]�w���I
	IIIRoute->GetNearEdge(Arrival.longitude, Arrival.latitude, &pntLockArrival);	
	result_process = IIIRoute->III_Routing(pntLockDeparture, pntLockArrival);
   	result_process = result_process | IIIRoute->III_GetPdRTResult(&resultFusionRT->resultPDRT);
   	
	return result_process;  
}

III_INT III_FusionRouteAPI::III_IndoorRoutingFunction(III_RouteAPI *IIIRoute, III_POINT Departure, III_POINT Arrival, char *sBuildingString)
{
    III_RTPOINT pntDeparture = {0}, pntArrival = {0};
    char sTempBuilding[256]; 
    III_INT result_process = ERROR_PD_ROUTE_OPENROUTEFILE;
        
    pntDeparture.pntOriginal = Departure;
    pntArrival.pntOriginal = Arrival;

	strcpy(sTempBuilding, g_szMapFolder);
	strcat(sTempBuilding, sBuildingString);
	strcat(sTempBuilding, "_lock.bin");           
    if(START_LOCK_ROAD_SUCCESS == startLockRoad(&pntDeparture, sTempBuilding) && START_LOCK_ROAD_SUCCESS == startLockRoad(&pntArrival, sTempBuilding)) {
	    GlobalparamRounting->typeRouting = TYPE_ROUTE_INDOOR;
		GlobalparamRounting->numFloorWalkMax = 0;
		strcpy(sTempBuilding, g_szMapFolder);
		strcat(sTempBuilding, sBuildingString);
        strcat(sTempBuilding, "_indoor.bin"); 
        GlobalparamRounting->tableRT.nameRoutingTable = sTempBuilding;
		if(0 != IIIRoute->III_SetRoutingParam(GlobalparamRounting)){
		    printf("\nError: SetRoutingParam fail");
		}
		
		result_process = IIIRoute->III_Routing(pntDeparture, pntArrival);
    } 

    if(resultFusionRT->resultIdrRT.numNode == 0) {
        III_RESULTINDOOR TempResultIdrRT;
        result_process = result_process | IIIRoute->III_GetIdrRTResult(&TempResultIdrRT);
        CopyResultIdrRT(TempResultIdrRT);        
    } else {
        result_process = result_process | IIIRoute->III_GetIdrRTResult(&resultFusionRT->resultIdrRT2);
    }
        
	return result_process; 
}

void III_FusionRouteAPI::CopyResultIdrRT(III_RESULTINDOOR TempResultIdrRT)
{
    resultFusionRT->resultIdrRT = TempResultIdrRT;
    resultFusionRT->resultIdrRT.resultNode = (III_RESULTNODE*)malloc(sizeof(III_RESULTNODE) * TempResultIdrRT.numNode);
    memcpy(resultFusionRT->resultIdrRT.resultNode, TempResultIdrRT.resultNode, sizeof(III_RESULTNODE) * TempResultIdrRT.numNode);  
    if(TempResultIdrRT.numNode != 0 && TempResultIdrRT.resultNode) {  
	    free(TempResultIdrRT.resultNode);
        TempResultIdrRT.resultNode = NULL;
        TempResultIdrRT.numNode = 0;
    }
}
/*
 *	Function:
 *	     III_PD_GetPdRTResult 
 *			�}�l����A* �t��k�ñNrouting���G(pathID & path distance)�x�s�bIII_RESULTPDRT���c��
 *	Parameter:
 *		 III_RESULTPDRT *resultPDRT - [in] routing���G(pathID & path distance)�x�s�bIII_RESULTPDRT���c��
 *		 [out] see III_Route_Manager.h error definition
*/
III_INT III_FusionRouteAPI::III_GetFusionRTResult(III_RESULT_FUSION_RT **resultRT)
{
    III_INT result_process = ERROR_SUCCESS;

    *resultRT = resultFusionRT;
	
	if(IIIRoute[0] != NULL) {
	    if(TYPE_ROUTE_PD == IIIRoute[0]->typetoroute) { //Outdoor
		    resultFusionRT->resultSequence = RESULT_OUTDOOR_FIRST;
	    } else if(TYPE_ROUTE_INDOOR == IIIRoute[0]->typetoroute) { //Indoor
		    resultFusionRT->resultSequence = RESULT_INDOOR_FIRST;
	    }
	}
	
	if(IIIRoute[1] != NULL) {
	    if(TYPE_ROUTE_INDOOR == IIIRoute[1]->typetoroute) { //Indoor		    
		    if(TYPE_ROUTE_INDOOR == IIIRoute[0]->typetoroute) { //Indoor
        		resultFusionRT->resultSequence = RESULT_INDOOR_BOTH;
	        }
		}
	}
	
	if(IIIRoute[2] != NULL) { //Indoor
        resultFusionRT->resultSequence = RESULT_INDOOR_OUTDOOR_INDOOR;	      
	}
	
	return result_process;
}

void III_FusionRouteAPI::DeleteIIIRouteObject(void)
{
    if(IIIRoute[0]) {
        delete IIIRoute[0]; 
        IIIRoute[0] = NULL;
    }   
    
    if(IIIRoute[1]) {
        delete IIIRoute[1];
        IIIRoute[1] = NULL;
    }     
        
    if(IIIRoute[2]) {
        delete IIIRoute[2]; 
        IIIRoute[2] = NULL;
    }  
    
    if(resultFusionRT) {
        if(resultFusionRT->resultPDRT.numEdge != 0 && resultFusionRT->resultPDRT.resultEdge) {
            free(resultFusionRT->resultPDRT.resultEdge);
            resultFusionRT->resultPDRT.resultEdge = NULL;
            resultFusionRT->resultPDRT.numEdge = 0;
        }
        
        if(resultFusionRT->resultIdrRT.numNode != 0 && resultFusionRT->resultIdrRT.resultNode) {  
		    free(resultFusionRT->resultIdrRT.resultNode);
            resultFusionRT->resultIdrRT.resultNode = NULL;
            resultFusionRT->resultIdrRT.numNode = 0;
        }
        
        if(resultFusionRT->resultIdrRT2.numNode != 0 && resultFusionRT->resultIdrRT2.resultNode) {  
            free(resultFusionRT->resultIdrRT2.resultNode);
            resultFusionRT->resultIdrRT2.resultNode = NULL;
            resultFusionRT->resultIdrRT2.numNode = 0;
        }
        
        free(resultFusionRT); 
        resultFusionRT = NULL;
    }
}

void III_FusionRouteAPI::NewResultFusionRT(void)
{
    if(resultFusionRT == NULL) {
        resultFusionRT = (III_RESULT_FUSION_RT *)malloc(sizeof(III_RESULT_FUSION_RT));
        resultFusionRT->resultPDRT.resultEdge = NULL; 
        resultFusionRT->resultPDRT.numEdge = 0;
        
        resultFusionRT->resultIdrRT.resultNode = NULL; 
        resultFusionRT->resultIdrRT.numNode = 0;
           
        resultFusionRT->resultIdrRT2.resultNode = NULL;
        resultFusionRT->resultIdrRT2.numNode = 0;
    }
}       
     