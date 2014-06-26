/*
 * CJNIIndoor.c
 *
 *  Created on: 2010/8/30
 *      Author: ASUS
 */

#include "CJNIIndoor.h"

//call Indoor Routing Engine and make Indoor JSON
JNIEXPORT jstring JNICALL Java_bean_RouteManager_startIdrRoute(JNIEnv *env, jobject obj, jint sLat, jint sLon, jint sFloor, jint eLat, jint eLon, jint eFloor, jstring build)
{
	char* response="fail";//return jstring preparing

	//.dll preparing, initial .dll function point
	HINSTANCE hDLL;
	typedef int(*III_Indoor_SetRoutingParam)(III_RTPARAM *);
	III_Indoor_SetRoutingParam	_Indoor_SetRoutingParam;
	typedef int(*III_Indoor_Routing_Lock)(III_RTPOINT *, III_RTPOINT *, III_CHAR *);
	III_Indoor_Routing_Lock	_Indoor_Routing_Lock;
	typedef int(*III_Indoor_GetIdrRTResult)(III_RESULTINDOOR *);
	III_Indoor_GetIdrRTResult _Indoor_GetIdrRTResult;
	typedef int(*III_Indoor_FreeIdrAStar)();
	III_Indoor_FreeIdrAStar	_Indoor_FreeIdrAStar;

	//typedef int(*III_Indoor_GetIdrDir)(char *);
	//III_Indoor_GetIdrDir _Indoor_GetIdrDir;
	//char dir[100];//for get dir

	III_RTPARAM *paramRounting = NULL; //indoor routing parameter
	III_RTPOINT pntDeparture = {0}, pntArrival = {0}; //initial routing Start_Point and End_Point

	//converting jstring to char* for set Building_ID, Routing_Table and Locking_Table file name
	char* buildID = (*env)->GetStringUTFChars(env, build, NULL); //need ReleaseStringUTFChars
	char* lockFile = strcat((*env)->GetStringUTFChars(env, build, NULL),".lock"); //need ReleaseStringUTFChars
	char* indoorFile =  strcat((*env)->GetStringUTFChars(env, build, NULL),".idr"); //need ReleaseStringUTFChars
	//printf("lock:%s,\nidr:%s\n", lockFile, indoorFile);

	III_RESULTINDOOR *resultIdrRT = NULL; //initial routing result
	//III_CHAR s[1];//for test console pause
	III_INT i;//for loop

	json_object *steps = json_object_new_object(); //initial indoor Steps JSON object

	char *ans;
	char dir2[100];
	getcwd(dir2, sizeof(dir2));
	printf("\ncurrent dir:%s\n", dir2);

	//set indoor parameter
	paramRounting = (III_RTPARAM *)malloc(sizeof(III_RTPARAM));
	paramRounting->typeRouting = TYPE_ROUTE_INDOOR;
	paramRounting->modeRouting = MODE_PD_SHORTEST;
	paramRounting->numFloorWalkMax = 0;
	paramRounting->tableRT.nameRoutingTable = indoorFile;

	//set original start point to pntDeparture
	pntDeparture.pntOriginal.latitude = sLat;
	pntDeparture.pntOriginal.longitude = sLon;
	pntDeparture.pntOriginal.height = sFloor;

	//set original end point to pntArrival
	pntArrival.pntOriginal.latitude = eLat;
	pntArrival.pntOriginal.longitude = eLon;
	pntArrival.pntOriginal.height = eFloor;

	resultIdrRT = (III_RESULTINDOOR*)malloc(sizeof(III_RESULTINDOOR));   // 弄個新空間;

	//loading .dll
	hDLL = LoadLibrary("IdrRouteEngine.dll");
	if(hDLL==NULL){ // if loading .dll fail, return fail info
		printf("ERROR: Indoor DLL load fail");
		//return 1;
		response = "ERROR_Indoor_DLL_Load_Fail";
		return (*env)->NewStringUTF(env,response);
	}else{ //if success, start routing
		//printf("SUCCESS: DLL load success\n");

		//set Indoor Routing Parameter
		_Indoor_SetRoutingParam = (III_Indoor_SetRoutingParam)GetProcAddress(hDLL,"III_Indoor_SetRoutingParam");
		if (NULL == _Indoor_SetRoutingParam)
			printf("ERROR: _Indoor_SetRoutingParam null\n");
		if(0 != _Indoor_SetRoutingParam(paramRounting)){
			response = "ERROR_Indoor_Parameter_Set_Fail\n";
			return (*env)->NewStringUTF(env,response);
		}

		//lock road and routing
		_Indoor_Routing_Lock = (III_Indoor_Routing_Lock)GetProcAddress(hDLL,"III_Indoor_Routing_Lock");
		if (NULL == _Indoor_Routing_Lock)
			printf("ERROR: _Indoor_Routing_Lock null\n");
		if(0 != _Indoor_Routing_Lock(&pntDeparture, &pntArrival, lockFile)){
			//_Indoor_GetIdrDir = (III_Indoor_GetIdrDir)GetProcAddress(hDLL,"III_Indoor_GetIdrDir");
			//_Indoor_GetIdrDir(dir);
			//printf("current dir:%s\n", dir);
			ans = " is CurrentDir, ERROR_Indoor_Routing_Fail\n";
			response = strcat(dir2,ans);
			return (*env)->NewStringUTF(env,response);
		}

		//get result
		_Indoor_GetIdrRTResult = (III_Indoor_GetIdrRTResult)GetProcAddress(hDLL,"III_Indoor_GetIdrRTResult");
		if (NULL == _Indoor_GetIdrRTResult)
			printf("ERROR: _Indoor_GetIdrRTResult null\n");
		if(0 != _Indoor_GetIdrRTResult(resultIdrRT)){
			response = "ERROR_Get_Indoor_Result_Fail\n";
			return (*env)->NewStringUTF(env,response);
		}

		/*//print result
		printf("\n amount:%d  ", resultIdrRT->numNode);
		for(i=0; i<resultIdrRT->numNode; i++){
			printf("\nnodeID:%d, attr:%d", (resultIdrRT->resultNode + i)->nodeID, (resultIdrRT->resultNode + i)->attr);
			printf(" lon:%d, lat:%d, floor:%d", (resultIdrRT->resultNode + i)->cdnt.longitude, (resultIdrRT->resultNode + i)->cdnt.latitude, (resultIdrRT->resultNode + i)->cdnt.height);
			printf("->");
		}*/

		//indoor JSON encoder
		if(ERROR_IDRJSON_SUCCESS == getIdrStepJSON(resultIdrRT, buildID, steps)){
			response = json_object_to_json_string(steps);
			printf("%s", json_object_to_json_string(steps));
		}
		//scanf("%s", s);//for test console pause

		//release memory
		_Indoor_FreeIdrAStar = (III_Indoor_FreeIdrAStar)GetProcAddress(hDLL,"III_Indoor_FreeIdrAStar");
		if (NULL == _Indoor_FreeIdrAStar)
			printf("ERROR: _Indoor_FreeIdrAStar null\n");
		if(0 != _Indoor_FreeIdrAStar()){
			response = "ERROR_Free_IdrAstar_Fail";
			return (*env)->NewStringUTF(env,response);
		}
		//response="success";
	}

	//release memory
	free(paramRounting);
	free(resultIdrRT);
	(*env)->ReleaseStringUTFChars(env, build, lockFile);//用完需釋放
	(*env)->ReleaseStringUTFChars(env, build, indoorFile);//用完需釋放
	FreeLibrary(hDLL);

	return (*env)->NewStringUTF(env,response);
}

//jni get Java String test
JNIEXPORT jobject JNICALL Java_bean_RouteManager_setMessage1(JNIEnv *env, jobject this, jstring msg)
{
    jclass cls = (*env)->GetObjectClass(env, this);
	//jclass cls = (*env)->FindClass(env,"bean/GDATest");
    jfieldID fid; // store the field ID
    char* setmsg = (*env)->GetStringUTFChars(env, msg, NULL); //need ReleaseStringUTFChars

    //use javap -s -p to see data type signature.
    fid = (*env)->GetFieldID(env, cls, "message1", "Ljava/lang/String;");
    (*env)->SetObjectField(env, this, fid, msg);

    printf("setMessage1:%s", setmsg);
    (*env)->ReleaseStringUTFChars(env, msg, setmsg);//用完需釋放

    return this;
}

//Steps JSON Encoder
int getIdrStepJSON(III_RESULTINDOOR *resultIdrRT, char *build, json_object *steps){
	json_object *start_location, *end_location, *building_info, *polyline, *duration, *html_instructions, *distance, *sub_steps;
	int i;//for loop
	int tDist=0;//total distance

	/* add travel_mode into steps, "travel_mode": " VIRTUAL ", */
	json_object_object_add(steps, "travel_mode", json_object_new_string("VIRTUAL"));

	/* set start_location and add into steps */
	start_location = json_object_new_object();
	if(ERROR_IDRJSON_SUCCESS == getIdrLocationObj(resultIdrRT->resultNode->cdnt, resultIdrRT->resultNode->attr, start_location)){
		json_object_object_add(steps, "start_location", start_location);
	}else{
		return ERROR_GETIDRSUBSTEPSJSON_FAIL;
	}

	/* set end_location and add into steps */
	end_location = json_object_new_object();
	if(ERROR_IDRJSON_SUCCESS == getIdrLocationObj((resultIdrRT->resultNode + resultIdrRT->numNode - 1)->cdnt, (resultIdrRT->resultNode + resultIdrRT->numNode - 1)->attr, end_location)){
		json_object_object_add(steps, "end_location", end_location);
	}else{
		return ERROR_GETIDRSUBSTEPSJSON_FAIL;
	}

	/* set building_info and add into steps, "building_info": {"id": }, */
	building_info = json_object_new_object();
	json_object_object_add(building_info, "id", json_object_new_string(build));
	json_object_object_add(steps, "building_info", building_info);

	/* set polyline and add into steps, "polyline": {"points": "", "levels": ""}, */
	polyline = json_object_new_object();
	json_object_object_add(polyline, "points", json_object_new_string(""));
	json_object_object_add(polyline, "levels", json_object_new_string(""));
	json_object_object_add(steps, "polyline", polyline);

	/* set sub_steps */
	sub_steps = json_object_new_array();
	for(i=1; i<resultIdrRT->numNode; i++){

		//if((resultIdrRT->resultNode + i - 1)->cdnt.height == (resultIdrRT->resultNode + i)->cdnt.height){
			if(ERROR_IDRJSON_SUCCESS != getIdrSubstepJSON((resultIdrRT->resultNode + i - 1), (resultIdrRT->resultNode + i), sub_steps, &tDist)){
				return ERROR_GETIDRSTEPSJSON_FAIL;
			}
		//}else{
		//	tDist = tDist + 4 * abs((resultIdrRT->resultNode + i - 1)->cdnt.height - (resultIdrRT->resultNode + i)->cdnt.height);
		//}
	}

	//printf("%d", tDist);
	/* wait count distance to set duration and add into steps */
	duration = json_object_new_object();
	if(ERROR_IDRJSON_SUCCESS == getIdrDurationObj(tDist, duration)){
		json_object_object_add(steps, "duration", duration);
	}else{
		return ERROR_GETIDRSUBSTEPSJSON_FAIL;
	}

	/* set html_instructions(null string) and add into steps, "html_instructions": "", */
	html_instructions = json_object_new_string("");
	json_object_object_add(steps, "html_instructions", html_instructions);

	/* wait count distance to set duration and add into steps */
	distance = json_object_new_object();
	if(ERROR_IDRJSON_SUCCESS == getIdrDistanceObj(tDist, distance)){
		json_object_object_add(steps, "distance", distance);
	}else{
		return ERROR_GETIDRSUBSTEPSJSON_FAIL;
	}

	/* add sub_steps into steps*/
	//printf("%s", json_object_to_json_string(sub_steps));
	json_object_object_add(steps, "sub_steps", sub_steps);

	//json_object_put(subobj);
	//json_object_put(sub_steps);
	//json_object_put(steps);

	return ERROR_IDRJSON_SUCCESS;
}

//SubSteps Array JSON Encoder, "sub_steps":[{},{},{}...]
int getIdrSubstepJSON(III_RESULTNODE *startNode, III_RESULTNODE *endNode, json_object *sub_steps, int *total_dist){

	json_object *substepobj, *sub_start_location, *sub_end_location, *sub_polyline, *sub_duration, *sub_html_instructions, *sub_distance;
	int sub_dist;

	substepobj = json_object_new_object();

	sub_start_location = json_object_new_object();
	if(ERROR_IDRJSON_SUCCESS == getIdrLocationObj(startNode->cdnt, startNode->attr, sub_start_location)){
		json_object_object_add(substepobj, "start_location", sub_start_location);
	}else{
		return ERROR_GETIDRSUBSTEPSJSON_FAIL;
	}

	sub_end_location = json_object_new_object();
	if(ERROR_IDRJSON_SUCCESS == getIdrLocationObj(endNode->cdnt, endNode->attr, sub_end_location)){
		json_object_object_add(substepobj, "end_location", sub_end_location);
	}else{
		return ERROR_GETIDRSUBSTEPSJSON_FAIL;
	}

	sub_polyline = json_object_new_object();
	json_object_object_add(sub_polyline, "points", json_object_new_string(""));
	json_object_object_add(sub_polyline, "levels", json_object_new_string(""));
	json_object_object_add(substepobj, "polyline", sub_polyline);

	if(startNode->cdnt.height == endNode->cdnt.height){
		sub_dist = getDistance(startNode->cdnt.latitude, startNode->cdnt.longitude, endNode->cdnt.latitude, endNode->cdnt.longitude);
	}else{
		sub_dist = 4 * abs(startNode->cdnt.height - endNode->cdnt.height);
	}

	*total_dist = *total_dist + sub_dist;//counting distance
	//printf("%d\n", *total_dist);

	sub_duration = json_object_new_object();
	if(ERROR_IDRJSON_SUCCESS == getIdrDurationObj(sub_dist, sub_duration)){
		json_object_object_add(substepobj, "duration", sub_duration);
	}else{
		return ERROR_GETIDRSUBSTEPSJSON_FAIL;
	}

	sub_html_instructions = json_object_new_string("");
	json_object_object_add(substepobj, "html_instructions", sub_html_instructions);

	sub_distance = json_object_new_object();
	if(ERROR_IDRJSON_SUCCESS == getIdrDistanceObj(sub_dist, sub_distance)){
		json_object_object_add(substepobj, "distance", sub_distance);
	}else{
		return ERROR_GETIDRSUBSTEPSJSON_FAIL;
	}

	//printf("%s", json_object_to_json_string(substepobj));
	//json_object_put(sub_start_location);
	//json_object_put(sub_end_location);
	//json_object_put(sub_polyline);
	//json_object_put(sub_duration);
	//json_object_put(sub_html_instructions);
	json_object_array_add(sub_steps, substepobj);

	return ERROR_IDRJSON_SUCCESS;
}

/*Location JSON Encoder,
 * "start_location": {"lat": , "lng": , "z_index": "", "attr": ""// EXIT, ELEVATOR, ESCALATOR, ...},
 * "end_location": {"lat": , "lng": , "z_index": "", "attr": ""// EXIT, ELEVATOR, ESCALATOR, ...},
 */
int getIdrLocationObj(III_POINT point, III_INT attr, json_object *locationObj){

	json_object_object_add(locationObj, "lat", json_object_new_int(point.latitude));
	json_object_object_add(locationObj, "lng", json_object_new_int(point.longitude));
	json_object_object_add(locationObj, "z_index", json_object_new_int(point.height));
	json_object_object_add(locationObj, "attr", json_object_new_int(attr));

	return ERROR_IDRJSON_SUCCESS;
}

//Duration JSON Encoder, "duration": {"value": ,"text": ""},
int getIdrDurationObj(int dist, json_object *durationObj){

	int durat = (dist/1.1)+1;

	json_object_object_add(durationObj, "value", json_object_new_int(durat));
	json_object_object_add(durationObj, "text", json_object_new_string(strcat(json_object_to_json_string(json_object_new_int(durat)),"秒")));

	return ERROR_IDRJSON_SUCCESS;
}

//Distance JSON Encoder, "distance": {"value": , "text": "" },
int getIdrDistanceObj(int dist, json_object *distanceObj){
	json_object_object_add(distanceObj, "value", json_object_new_int(dist));
	json_object_object_add(distanceObj, "text", json_object_new_string(strcat(json_object_to_json_string(json_object_new_int(dist)),"公尺")));

	return ERROR_IDRJSON_SUCCESS;
}

//get distance between a node and b node
int getDistance(int ax, int ay, int bx, int by){
	double distance = sqrt((pow((ax - bx), 2) + pow((ay - by), 2))*pow((60*1.852*1000), 2)) * (0.000001);

	return (int)distance;
}
/*
int main(void){

	HINSTANCE hDLL;
	typedef int(*III_Indoor_SetRoutingParam)(III_RTPARAM *);
	III_Indoor_SetRoutingParam	_Indoor_SetRoutingParam;
	typedef int(*III_Indoor_Routing_Lock)(III_RTPOINT *, III_RTPOINT *, III_CHAR *);
	III_Indoor_Routing_Lock	_Indoor_Routing_Lock;
	typedef int(*III_Indoor_GetIdrRTResult)(III_RESULTINDOOR *);
	III_Indoor_GetIdrRTResult _Indoor_GetIdrRTResult;
	typedef int(*III_Indoor_FreeIdrAStar)();
	III_Indoor_FreeIdrAStar	_Indoor_FreeIdrAStar;

	III_RTPARAM *paramRounting = NULL;

	III_RTPOINT pntDeparture = {0}, pntArrival = {0};
	III_CHAR* lockFile = "lock.bin";

	III_RESULTINDOOR *resultIdrRT = NULL;
	III_CHAR s[1];//for test console pause
	III_INT i;//for loop

	paramRounting = (III_RTPARAM *)malloc(sizeof(III_RTPARAM));
	paramRounting->typeRouting = TYPE_ROUTE_INDOOR;
	paramRounting->modeRouting = MODE_PD_SHORTEST;
	paramRounting->numFloorWalkMax = 0;
	paramRounting->tableRT.nameRoutingTable = "indoor.bin";

	//set original start point to pntDeparture
	pntDeparture.pntOriginal.longitude = 121554880;
	pntDeparture.pntOriginal.latitude = 25058657;
	pntDeparture.pntOriginal.height = 1;

	//set original end point to pntArrival
	pntArrival.pntOriginal.longitude = 121554735;
	pntArrival.pntOriginal.latitude = 25058721;
	pntArrival.pntOriginal.height = 13;

	resultIdrRT = (III_RESULTINDOOR*)malloc(sizeof(III_RESULTINDOOR));   // 弄個新空間;

	hDLL = LoadLibrary("CJNIIndoor.dll");
	if(hDLL==NULL){
		printf("DLL Load fail");
		return 1;
	}else{

		_Indoor_SetRoutingParam = (III_Indoor_SetRoutingParam)GetProcAddress(hDLL,"III_Indoor_SetRoutingParam");
		_Indoor_SetRoutingParam(paramRounting);

		_Indoor_Routing_Lock = (III_Indoor_Routing_Lock)GetProcAddress(hDLL,"III_Indoor_Routing_Lock");
		_Indoor_Routing_Lock(&pntDeparture, &pntArrival, lockFile);


		_Indoor_GetIdrRTResult = (III_Indoor_GetIdrRTResult)GetProcAddress(hDLL,"III_Indoor_GetIdrRTResult");
		_Indoor_GetIdrRTResult(resultIdrRT);

		printf("\n amount:%d  ", resultIdrRT->numNode);
		for(i=0; i<resultIdrRT->numNode; i++){
			printf("\nnodeID:%d, attr:%d", (resultIdrRT->resultNode + i)->nodeID, (resultIdrRT->resultNode + i)->attr);
			printf(" lon:%d, lat:%d, floor:%d", (resultIdrRT->resultNode + i)->cdnt.longitude, (resultIdrRT->resultNode + i)->cdnt.latitude, (resultIdrRT->resultNode + i)->cdnt.height);
			printf("->");
		}

		scanf("%s", s);//for test console pause

		_Indoor_FreeIdrAStar = (III_Indoor_FreeIdrAStar)GetProcAddress(hDLL,"III_Indoor_FreeIdrAStar");
		_Indoor_FreeIdrAStar();

	}

	free(paramRounting);
	free(resultIdrRT);
	FreeLibrary("CJNIIndoor.dll");
	*********************************************
	if(0 == TEST_setRouteParameter(MODE_PD_SHORTEST, 0, "indoor.bin")){

		//TEST_startRouting(121554880, 25058657, 1, 121554735, 25058721, 13, "lock.bin");

		III_RTPOINT pntDeparture = {0}, pntArrival = {0};
			//set original start point to pntDeparture
			pntDeparture.pntOriginal.longitude = 121554880;
			pntDeparture.pntOriginal.latitude = 25058657;
			pntDeparture.pntOriginal.height = 1;

			//set original end point to pntArrival
			pntArrival.pntOriginal.longitude = 121554735;
			pntArrival.pntOriginal.latitude = 25058721;
			pntArrival.pntOriginal.height = 13;

		III_Indoor_Routing_Lock(pntDeparture, pntArrival, "lock.bin");


		TEST_getResult();
	}
	TEST_freeIndoor();
	*********************************************

	return 0;
}*/


/*
III_INT TEST_setRouteParameter(int mode, int maxWalk, char *routeTable){

	III_RTPARAM *paramRounting = NULL;
	paramRounting = (III_RTPARAM *)III_malloc(sizeof(III_RTPARAM));
	paramRounting->typeRouting = TYPE_ROUTE_INDOOR;
	paramRounting->modeRouting = mode;
	paramRounting->numFloorWalkMax = maxWalk;
	paramRounting->tableRT.nameRoutingTable = routeTable;

	if(ERROR_IDR_SUCCESS != III_Indoor_SetRoutingParam(paramRounting)){
		printf("ERROR: set Route Parameter");
		III_free(paramRounting);
		return 1;
	}

	III_free(paramRounting);
	III_free(resultIdrRT);
	return 0;
}

III_INT TEST_startRouting(int osLon, int osLat, int osFloor, int oeLon, int oeLat, int oeFloor, char *lockFile){

	III_RTPOINT pntDeparture = {0}, pntArrival = {0};
	//set original start point to pntDeparture
	pntDeparture.pntOriginal.longitude = osLon;
	pntDeparture.pntOriginal.latitude = osLat;
	pntDeparture.pntOriginal.height = osFloor;

	//set original end point to pntArrival
	pntArrival.pntOriginal.longitude = oeLon;
	pntArrival.pntOriginal.latitude = oeLat;
	pntArrival.pntOriginal.height = oeFloor;

	if((START_LOCK_ROAD_SUCCESS == startLockRoad(&pntDeparture, lockFile)) &&
			(START_LOCK_ROAD_SUCCESS == startLockRoad(&pntArrival,   lockFile)))
	{
		III_Indoor_Routing(pntDeparture, pntArrival);
	}else{
		printf("ERROR: LOCK ROAD fail");
		return 1;
	}

	return 0;
}

III_INT TEST_getResult(){
	III_RESULTINDOOR *resultIdrRT = NULL;
	III_CHAR s[1];//for test console pause
	III_INT i;//for loop

	resultIdrRT = (III_RESULTINDOOR*)III_malloc(sizeof(III_RESULTINDOOR));   // 弄個新空間;
	III_Indoor_GetIdrRTResult(resultIdrRT);

	printf("\n amount:%d  ", resultIdrRT->numNode);
	for(i=0; i<resultIdrRT->numNode; i++){
		printf("\nnodeID:%d, attr:%d", (resultIdrRT->resultNode + i)->nodeID, (resultIdrRT->resultNode + i)->attr);
		printf(" lon:%d, lat:%d, floor:%d", (resultIdrRT->resultNode + i)->cdnt.longitude, (resultIdrRT->resultNode + i)->cdnt.latitude, (resultIdrRT->resultNode + i)->cdnt.height);
		printf("->");
	}

	scanf("%s", s);//for test console pause
	return 0;
}

III_INT TEST_freeIndoor(){
	III_Indoor_FreeIdrAStar();
	return 0;
}
*/
