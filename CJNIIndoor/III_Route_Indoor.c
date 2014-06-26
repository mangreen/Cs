#include "III_Route_Manager.h"
#include "III_Route_Indoor.h"

IndoorAStar *idras = NULL;

III_INT III_Indoor_SetRoutingParam(III_RTPARAM *paramRounting)
{
	//Norman add begin 0609
	printf("\nIII_Indoor_SetRoutingParam paramRounting: %x\n", paramRounting);

	if (NULL == idras){
		idras = (IndoorAStar*)III_malloc(sizeof(IndoorAStar));   //get memory space;
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

III_INT III_Indoor_Routing_Lock(III_RTPOINT *pntDeparture, III_RTPOINT *pntArrival, III_CHAR *lockFile)
{
	III_INT errorRoute = 0;

	printf("III_Indoor_Routing_Lock parameter:%x, %x, %x\n", pntDeparture, pntArrival, lockFile);
	if((ERROR_IDR_SUCCESS == startLockRoad(pntDeparture, lockFile)) &&
				(ERROR_IDR_SUCCESS == startLockRoad(pntArrival, lockFile)))
	{
		errorRoute = III_Indoor_Routing(*pntDeparture, *pntArrival);
		if(ERROR_IDR_SUCCESS != errorRoute){
			printf("ERROR: ROUTING fail - %d", errorRoute);
			return errorRoute;
		}
	}else{
		printf("ERROR: LOCK ROAD fail");
		return ERROR_IDR_LOCK_FAIL;
	}
	return ERROR_IDR_SUCCESS;
}

III_INT III_Indoor_Routing(III_RTPOINT pntDeparture, III_RTPOINT pntArrival)
{
	III_INT errorRoute = 0;
	printf("\nIII_Indoor_Routing parameter:%x, %x\n", pntDeparture, pntArrival);

	//Norman add begin 0609
	if (NULL == idras)
		return ERROR_IDR_ASTAR_NULL;
	//Norman add end 0609
	//III_CHAR s[1];//for test console pause

	idras->resultIdr.pntDeparture = pntDeparture;
	idras->resultIdr.pntArrival = pntArrival;

	idras->result.pntDeparture = pntDeparture;
	idras->result.pntArrival = pntArrival;
	// Norman ��papago�nfunction�ˬdfixpoint�O�_�bedge ID�W

	errorRoute = initialAStar(idras);
	if(ERROR_IDR_SUCCESS == errorRoute){

		//debugNodeInfo(idras);
		errorRoute = setIdrAStarStart(idras);
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

		errorRoute == startIdrAStar(idras);
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
	int i;
	printf("\nIII_Indoor_GetIdrRTResult parameter:%x\n", resultIdrRT);

	if(NULL != resultIdrRT){
		//if(NULL != resultPDRT->resultEdge){
			//freeIdrResult(idras);
		//}
		memcpy(resultIdrRT, &(idras->result), sizeof(III_RESULTINDOOR));    //copy memory content
	}else{
		return ERROR_IDR_RESULTPDRT_NULL;
	}

	/*
	printf("\n amount:%d  ", idras->result.numNode);
	for(i=0; i<idras->result.numNode; i++){
		printf("%d(%d)->", (idras->result.resultNode + i)->nodeID, (idras->result.resultNode + i)->attr);
	}*/


	return ERROR_IDR_SUCCESS;
	//Norman add end 0609
}

//mangree modify
III_INT III_Indoor_FreeIdrAStar()
{
	printf("\nIII_Indoor_FreeIdrAStar parameter:\n");

	freeIdrResult(idras);
	finishAStar(idras);
	III_free(idras);//release IndoorAStar

	return ERROR_IDR_SUCCESS;
}

III_INT III_Indoor_GetIdrDir(char *dir){
	char dir1[100];
	getcwd(dir1, sizeof(dir1));
	getcwd(dir, sizeof(dir));
	printf("\ncurrent dir:%s, %s\n", dir, dir1);

	return ERROR_IDR_SUCCESS;
}
