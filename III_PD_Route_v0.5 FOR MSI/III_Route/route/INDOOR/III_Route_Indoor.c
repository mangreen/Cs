#include "III_Route_Manager.h"
#include "III_Route_Indoor.h"   

IndoorAStar *idras = NULL;

III_INT III_Indoor_SetRoutingParam(III_RTPARAM *paramRounting)
{
	//Norman add begin 0609
	if (NULL == idras){
		idras = (IndoorAStar*)III_malloc(sizeof(IndoorAStar));   // 弄個新空間;
	}else{
		freeIdrResult(idras);
		finishAStar(idras);
	}

	if (NULL == idras)//judge idras malloc success or fail
		return ERROR_IDR_ASTAR_NULL;

	//Norman add end 0609
	idras->weight.maxWalkFloor = paramRounting->numFloorWalkMax;
	idras->weight.routingMod = paramRounting->modeRouting;
	idras->binName = paramRounting->tableRT.nameRoutingTable;
	return ERROR_IDR_SUCCESS;
}

III_INT III_Indoor_Routing(III_RTPOINT pntDeparture, III_RTPOINT pntArrival)
{
	III_INT errorRoute = 0;

	//Norman add begin 0609
	if (NULL == idras)
		return ERROR_IDR_ASTAR_NULL;
	//Norman add end 0609
	//III_CHAR s[1];//for test console pause

	idras->resultIdr.pntDeparture = pntDeparture;
	idras->resultIdr.pntArrival = pntArrival;

	idras->result.pntDeparture = pntDeparture;
	idras->result.pntArrival = pntArrival;
	// Norman 跟papago要function檢查fixpoint是否在edge ID上

	errorRoute = initialAStar(idras);//Wally modify
	if(ERROR_IDR_SUCCESS == errorRoute){

		//debugNodeInfo(idras);
		errorRoute = setIdrAStarStart(idras);//Wally modify
		if (ERROR_SUCCESS != errorRoute){
			printf("\nError %d: setIdrAStarStart fail", errorRoute);
			return errorRoute;
		}

		errorRoute = setIdrAStarEnd(idras);
		if (ERROR_SUCCESS != errorRoute){
			printf("\nError %d:  setIdrAStarEnd fail", errorRoute);
			return errorRoute;
		}
		//if (ERROR_IDR_SUCCESS != setIdrAStarEnd(idras)){
		//	return ERROR_INDOOR_4;
		//}

		errorRoute = startIdrAStar(idras);
		if(ERROR_IDR_SUCCESS != errorRoute){
			printf("ERROR:%d", errorRoute);
			return errorRoute;
		}

		//finishAStar(idras);
	}
	else
	{
		return errorRoute;
	}

	//scanf("%s", s);//for test console pause
	return ERROR_IDR_SUCCESS;
}

//mangreen modify
III_INT III_Indoor_GetIdrRTResult(III_RESULTINDOOR *resultIdrRT)
{
//	int i;
//	printf("\nIII_Indoor_GetIdrRTResult parameter:%x\n", resultIdrRT);

	if(NULL != resultIdrRT && idras->result.resultNode != NULL){	// Tony ADDED "idras->result.resultNode != NULL" @ 20110126
		//if(NULL != resultPDRT->resultEdge){
			//freeIdrResult(idras);
		//}
		memcpy(resultIdrRT, &(idras->result), sizeof(III_RESULTINDOOR));    // 將該資料拷至指定位置
	}else{
		return ERROR_IDR_RESULTPDRT_NULL;
	}

//	printf("\n amount:%d  ", idras->result.numNode);
/*	for(i=0; i<idras->result.numNode; i++){
		printf("%d(%d)->", (idras->result.resultNode + i)->nodeID, (idras->result.resultNode + i)->attr);
	}*/
	

	return ERROR_IDR_SUCCESS;
	//Norman add end 0609
}

//mangree modify
III_INT III_Indoor_FreeIdrAStar(void)
{
//	printf("\nIII_Indoor_FreeIdrAStar parameter:\n");

	freeIdrResult(idras);
	finishAStar(idras);
	III_free(idras);//release IndoorAStar

    idras = III_NULL;   // [Tony added @ 20101210]

	return ERROR_IDR_SUCCESS;
}