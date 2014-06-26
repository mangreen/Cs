#include "Route_IdrAStar.h"
#include "Route_IdrAStar_Out.h"

const III_CHAR BITMASK[8] = {1, 2, 4, 8, 16, 32, 64, 128};// 用來幫助設定bit array: {00000001, 00000010, 00000100, 00001000, 00010000, 00100000, 01000000, 10000000}
III_INT WEIGHT[5];//weighting

//testing code
III_INT debugNodeInfo(IndoorAStar* idras){
	Node node;//front node
	III_INT i,j;//for loop
	
	for(i=16; i<1456; i=i+20){
		//if (i > 1255)
			//j = i;
		getIdrNeighbor(idras, &node, i, 0);
		getch();
	}

	return 0;
}

//set Start of indoor A*
III_INT setIdrAStarStart(IndoorAStar* idras){
	III_UCHAR egbuf[MAXNEIGHBOR]; //read file buffer(edge)
	Node fnode;//front node
	Node tnode;//tail node
	//III_INT dx;
	//III_INT dy;
	III_INT i;//for loop

	// Norman modify
	if(idras->header.maxE < idras->resultIdr.pntDeparture.nLineSegIdxFix){
		return ERROR_EDGE_NO_EXSIT;
	}

	// 在前面檢查file size時要確定binfile size超過, 否則會抓到亂七八糟的東東
	III_fseek(idras->binfile, 0, SEEK_END);

	if(III_ftell(idras->binfile) > (MAXHEADER+MAXNEIGHBOR*2+MAXEDGE)){
		III_fseek(idras->binfile, idras->header.fullsize+(idras->resultIdr.pntDeparture.nLineSegIdxFix*MAXEDGE), 0); //shift off1(fullsize) + MAZEDGE to read edge info
	//}

	//if(!feof(idras->binfile)){	
		
		III_fread(egbuf, sizeof(III_CHAR), MAXNEIGHBOR, idras->binfile);//file read

		/*
		//get dx
		if(egbuf[1] > 0x80){
			dx = ((0xffffff00 | egbuf[1]) << 8) | (0x000000ff & egbuf[0]);
		}else{
			dx = ((0x000000ff & egbuf[1]) << 8) | (0x000000ff & egbuf[0]);
		}
		//printf("\n dx:%d", dx);

		//get dy
		if(egbuf[3] > 0x80){
			dy = ((0xffffff00 | egbuf[3]) << 8) | (0x000000ff & egbuf[2]);
		}else{
			dy = ((0x000000ff & egbuf[3]) << 8) | (0x000000ff & egbuf[2]);
		}
		//printf("\n dy:%d", dy);
		*/

		//get offset
		fnode.offset = ((0x000000ff & egbuf[3]) << 20) | ((0x000000ff & egbuf[2]) << 12) | ((0x000000ff & egbuf[1]) << 4) | ((0x000000ff & egbuf[0]) >> 4);
		//printf("\n offset:%d", fnode.offset);

		//get amount
		fnode.amount = 0x0000000f & egbuf[0];
		//printf("\n amount:%d", fnode.amount);
	}else{
		return ERROR_SET_START_EDGE_NULL;
	}

	for(i=0; i<fnode.amount; i++){
		getIdrNeighbor(idras, &tnode, fnode.offset, i);
		if(tnode.edgeID == idras->resultIdr.pntDeparture.nLineSegIdxFix){
			tnode.g = 0;
			tnode.h = 0;
			tnode.f = tnode.g + tnode.h;
			tnode.dist = getDistance(idras->resultIdr.pntDeparture.pntOriginal.longitude , idras->resultIdr.pntDeparture.pntOriginal.latitude, tnode.cdnt.longitude, tnode.cdnt.latitude);
			//printf("\nstarttdistance:%d  (%d, %d) (%d, %d) ", tnode.dist, sPoint.originalX, sPoint.originalY, tnode.x, tnode.y);
			tnode.parentID = NODE_HAVE_NO_PARENT;
			insertHeaptree(&(idras->opList), &tnode);//put into open list
			addHash(idras->nodeCache, tnode, tnode.nodeID);//put into hash
			break;
		}
	}

	for(i=0; i<tnode.amount; i++){
		getIdrNeighbor(idras, &fnode, tnode.offset, i);
		if(fnode.edgeID == idras->resultIdr.pntDeparture.nLineSegIdxFix){
			fnode.g = 0;
			fnode.h = 0;
			fnode.f = fnode.g + fnode.h;
			fnode.dist = getDistance(idras->resultIdr.pntDeparture.pntOriginal.longitude , idras->resultIdr.pntDeparture.pntOriginal.latitude, fnode.cdnt.longitude, fnode.cdnt.latitude);
			//printf("\nstartfdistance:%d  (%d, %d) (%d, %d) ", fnode.dist, sPoint.originalX, sPoint.originalY, fnode.x, fnode.y);
			fnode.parentID = NODE_HAVE_NO_PARENT;
			insertHeaptree(&(idras->opList), &fnode);//put into open list
			addHash(idras->nodeCache, fnode, fnode.nodeID);//put into hash
			break;
		}
	}

	idras->start = tnode;
	return EROOR_SET_START_SUCCESS;
}

//set End of indoor A*
III_INT setIdrAStarEnd(IndoorAStar* idras){
	III_UCHAR egbuf[MAXNEIGHBOR]; //read file buffer(edge)
	Node fnode;//front node
	Node tnode;//tail node
	//III_INT dx;
	//III_INT dy;
	III_INT i;//for loop

	fnode.nodeID = -1;
	tnode.nodeID = -1;

	// Norman 不懂判斷NULL意義
	if(idras->header.maxE < idras->resultIdr.pntArrival.nLineSegIdxFix){
		return ERROR_EDGE_NO_EXSIT;
	}

	// 在前面檢查file size時要確定binfile size超過, 否則會抓到亂七八糟的東東
	III_fseek(idras->binfile, 0, SEEK_END);

	if(III_ftell(idras->binfile) > (MAXHEADER+MAXNEIGHBOR*2+MAXEDGE)){
		III_fseek(idras->binfile, idras->header.fullsize+(idras->resultIdr.pntArrival.nLineSegIdxFix*MAXEDGE), 0); //shift off1(fullsize) + MAZEDGE to read edge info
	//}
	//if(!feof(idras->binfile)){	
		
		III_fread(egbuf, sizeof(III_CHAR), MAXNEIGHBOR, idras->binfile);//file read

		/*
		//get dx
		if(egbuf[1] > 0x80){
			dx = ((0xffffff00 | egbuf[1]) << 8) | (0x000000ff & egbuf[0]);
		}else{
			dx = ((0x000000ff & egbuf[1]) << 8) | (0x000000ff & egbuf[0]);
		}
		//printf("\n dx:%d", dx);

		//get dy
		if(egbuf[3] > 0x80){
			dy = ((0xffffff00 | egbuf[3]) << 8) | (0x000000ff & egbuf[2]);
		}else{
			dy = ((0x000000ff & egbuf[3]) << 8) | (0x000000ff & egbuf[2]);
		}
		//printf("\n dy:%d", dy);
		*/

		//get offset
		fnode.offset = ((0x000000ff & egbuf[3]) << 20) | ((0x000000ff & egbuf[2]) << 12) | ((0x000000ff & egbuf[1]) << 4) | ((0x000000ff & egbuf[0]) >> 4);
		//printf("\n offset:%d", fnode.offset);

		//get amount
		fnode.amount = 0x0000000f & egbuf[0];
		//printf("\n amount:%d", fnode.amount);
	}else{
		return ERROR_SET_END_EDGE_NULL;
	}

	for(i=0; i<fnode.amount; i++){
		getIdrNeighbor(idras, &tnode, fnode.offset, i);
		if(tnode.edgeID == idras->resultIdr.pntArrival.nLineSegIdxFix){
			//tnode.g = 0;
			//tnode.h = 0;
			//tnode.f = tnode.g + tnode.h;
			//tnode.dist = getDistance(ePoint.originalX, ePoint.originalY, tnode.x, tnode.y);
			//printf("\nendtdistance:%d  (%d, %d) (%d, %d) ", tnode.dist, ePoint.originalX, ePoint.originalY, tnode.x, tnode.y);
			idras->tEndID = tnode.nodeID;
			idras->tEndEdgeDist = getDistance(idras->resultIdr.pntArrival.pntOriginal.longitude , idras->resultIdr.pntArrival.pntOriginal.latitude, tnode.cdnt.longitude, tnode.cdnt.latitude);
			break;
		}
	}

	for(i=0; i<tnode.amount; i++){
		getIdrNeighbor(idras, &fnode, tnode.offset, i);
		if(fnode.edgeID == idras->resultIdr.pntArrival.nLineSegIdxFix){
			//fnode.g = 0;
			//fnode.h = 0;
			//fnode.f = fnode.g + fnode.h;
			//fnode.dist = getDistance(ePoint.originalX, ePoint.originalY, fnode.x, fnode.y);
			//printf("\nendfdistance:%d  (%d, %d) (%d, %d) ", fnode.dist, ePoint.originalX, ePoint.originalY, fnode.x, fnode.y);
			idras->fEndID = fnode.nodeID;
			idras->fEndEdgeDist = getDistance(idras->resultIdr.pntArrival.pntOriginal.longitude , idras->resultIdr.pntArrival.pntOriginal.latitude, fnode.cdnt.longitude, fnode.cdnt.latitude);
			break;
		}
	}

	idras->end.cdnt.longitude = idras->resultIdr.pntArrival.pntOriginal.longitude;
	idras->end.cdnt.latitude = idras->resultIdr.pntArrival.pntOriginal.latitude;
	idras->end.cdnt.height = idras->resultIdr.pntArrival.pntOriginal.height;
	idras->end.edgeID = idras->resultIdr.pntArrival.nLineSegIdxFix;

	return EROOR_SET_END_SUCCESS;
}

//start indoor A* routing
III_INT startIdrAStar(IndoorAStar* idras){
	
	III_INT i;//for loop
	III_INT count = 0;
	III_INT totalDist = 0;//total distance
	Node neighbor;
	Node parent;
	Node stay;
	III_PDRTEDGE oneedge;

	setIdrWeight(idras);//set weighting

	if(idras->resultIdr.pntDeparture.nLineSegIdxFix != idras->resultIdr.pntArrival.nLineSegIdxFix){
		while(0 == idras->stopFlag){
			
			if(0 >= idras->opList.count){
				printf("\n%d -- ERROR: Can't Find Road ");
				return ERROR_ASTAR_CANT_FIND_ROAD;
			}
			stay = idras->opList.heap[1];
			printHeaptree(idras->opList);
			deleteHeaptreeRoot(&(idras->opList));
			
			printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ %d \\\\\\\\\\\\\\\\\\\\\\\\\\", stay.nodeID);
			printHeaptree(idras->opList);
			
			for(i = 0; i < stay.amount; i++){
				
				getIdrNeighbor(idras, &neighbor, stay.offset, i);
			
				if(CLLIST_HAS_NOT_ADDED == checkCloseList(idras, neighbor.nodeID)){
				
					neighbor.g = stay.g + neighbor.dist;//set G value
					//neighbor.h = setIdrWeighting(idras, neighbor);//count H value by Manhattan method
					neighbor.h = abs(idras->end.cdnt.longitude - neighbor.cdnt.longitude) + abs(idras->end.cdnt.latitude - neighbor.cdnt.latitude) + WEIGHT[neighbor.attr]*70*abs(idras->end.cdnt.height - neighbor.cdnt.height);//使用曼哈頓方法（Manhattan method）計算H值
					neighbor.f = neighbor.g + neighbor.h; //F = G + H
				
					//處理更改parent, 搜尋neighbor的鄰居, 以G值查詢是否在hash中, 如果新G值比較小或是不在hash中就加入open list 
					if(HASH_GET_GVALUE_FAIL == getHashGValue(idras->nodeCache, neighbor.nodeID) || getHashGValue(idras->nodeCache, neighbor.nodeID) > neighbor.g){
						neighbor.parentID = stay.nodeID;//set parent
						//getHash(idras->nodeCache, neighbor.parent, next->nodeID);
						insertHeaptree(&(idras->opList), &neighbor);//put into open list
						addHash(idras->nodeCache, neighbor, neighbor.nodeID);//put into hash
					}
				}
			}
			
			//getch();
			addCloseList(idras, stay.nodeID);//add Stay it to Close List
			if(CLLIST_HAS_ADDED == checkCloseList(idras, idras->fEndID) || CLLIST_HAS_ADDED == checkCloseList(idras, idras->tEndID)){
				idras->stopFlag = 1;
			}
		}

		//check End has been to or not, if not,  startIdrAStar carry on
		if(CLLIST_HAS_ADDED == checkCloseList(idras, idras->fEndID)){
			/*
			printf("\npath: %d <-(%d)-", stay->nodeID, stay->edgeID);
			while(-1 != stay->parentID){
				getHash(idras->nodeCache, &parent, stay->parentID);
				
				printf(" %d <-(%d)- ", parent.nodeID, parent.edgeID);
				stay->parentID = parent.parentID;
			}*/
			setIdrResult(idras, &stay, 0);
			//setIdrResultEdge(idras, &stay, 0);
			//setIdrResultNode(idras, &stay);
			
		}else if(CLLIST_HAS_ADDED == checkCloseList(idras, idras->tEndID)){
			/*
			printf("\npath: %d <-(%d)-", stay->nodeID, stay->edgeID);
			while(-1 != stay->parentID){
				getHash(idras->nodeCache, &parent, stay->parentID);
				
				printf(" %d <-(%d)- ", parent.nodeID, parent.edgeID);
				stay->parentID = parent.parentID;
			}*/
			setIdrResult(idras, &stay, 1);
			//setIdrResultEdge(idras, &stay, 0);
			//setIdrResultNode(idras, &stay);
		}

	}else{//when Start and End are in the same edge
		idras->resultIdr.numEdge = 1;
		idras->resultIdr.resultEdge = (III_PDRTEDGE*)III_malloc(sizeof(III_PDRTEDGE));   // 弄個新空間;
		oneedge.idEdge = idras->resultIdr.pntDeparture.nLineSegIdxFix;
		oneedge.distanceEdge = getDistance(idras->resultIdr.pntArrival.pntOriginal.longitude , idras->resultIdr.pntArrival.pntOriginal.latitude, idras->resultIdr.pntDeparture.pntOriginal.longitude , idras->resultIdr.pntDeparture.pntOriginal.latitude);
		memcpy(idras->resultIdr.resultEdge, &oneedge, sizeof(III_PDRTEDGE));    // 將該資料拷至指定位置
	}

	return ERROR_ASTAR_SUCCESS;
}

/*
//start indoor A* routing, recursive
III_INT startIdrAStar(IndoorAStar* idras, Node* stay){
	
	III_INT i;//for loop
	III_INT count = 0;
	III_INT totalDist = 0;//total distance
	Node neighbor;

	for(i = 0; i < stay->amount; i++){
		getIdrNeighbor(idras, &neighbor, stay->offset, i);
		
		if(0 == checkCloseList(idras, neighbor.nodeID)){
			
			neighbor.g = stay->g + neighbor.dist;//set G value
			neighbor.h = setIdrWeighting(idras, neighbor);//count H value by Manhattan method
			neighbor.f = neighbor.g + neighbor.h; //F = G + H
			
			//處理更改parent, 搜尋neighbor的鄰居, 以G值查詢是否在hash中, 如果新G值比較小或是不在hash中就加入open list 
			if(-1 == getHashGValue(idras->nodeCache, neighbor.nodeID) || getHashGValue(idras->nodeCache, neighbor.nodeID) > neighbor.g){
				neighbor.parent = stay;//set parent
				insertHeaptree(&(idras->opList), &neighbor);//put into open list
				addHash(idras->nodeCache, &neighbor, neighbor.nodeID);//put into hash
			}
		}
	}

	addCloseList(idras, stay->nodeID);//add Stay it to Close List

	//check End has been to or not, if not,  startIdrAStar carry on
	if(1 == checkCloseList(idras, idras->fEndID)){
		setIdrResult(idras, stay, 0);
		/*testing code, print routing result*/
		/*
		printf("\npath: %d <-(%d:%d)-", stay->nodeID, stay->edgeID, stay->dist);
		idras->result.amount = 2;//fix edges of start and end
		idras->result.totalDist = stay->dist + idras->fEndEdgeDist;
		while(NULL != stay->parent){
			idras->result.amount ++;
			idras->result.totalDist += stay->parent->dist;
			printf(" %d <-(%d:%d)- ", stay->parent->nodeID, stay->parent->edgeID, stay->parent->dist);
			stay = stay->parent;
		}
		printf("\ncount:%d dist:%d", idras->result.amount, idras->result.totalDist);
		
	}else if(1 == checkCloseList(idras, idras->tEndID)){
		setIdrResult(idras, stay, 1);
		/*testing code, print routing result*/
		/*
		printf("\npath: %d <-(%d:%d)-", stay->nodeID, stay->edgeID, stay->dist);
		idras->result.amount = 2;//fix edges of start and end
		idras->result.totalDist = stay->dist + idras->tEndEdgeDist;
		while(NULL != stay->parent){
			idras->result.amount ++;
			idras->result.totalDist += stay->parent->dist;
			printf(" %d <-(%d:%d)- ", stay->parent->nodeID, stay->parent->edgeID, stay->parent->dist);
			stay = stay->parent;
		}
		printf("\ncount:%d dist:%d", idras->result.amount, idras->result.totalDist);
		
	}else{
		//get most small G value Node from open list
		Node next = idras->opList.heap[1];
		deleteHeaptreeRoot(&(idras->opList));

		startIdrAStar(idras, &next);//recursive call startIdrAStar
	}
	
	return 0;
}*/

//adjust H value
III_INT setIdrWeight(IndoorAStar* idras){
	//WEIGHT[0]:走道 WEIGHT[1]:階梯  WEIGHT[2]:電扶梯  WEIGHT[3]:階梯+電扶梯  WEIGHT[4]:電梯
	//mode=1:最短路徑  mode=2:無障礙空間
	if(MODE_ACCESSABLE == idras->weight.routingMod){

		WEIGHT[PATH_PASSAGE] = 1;
		WEIGHT[PATH_STAIR] = 15;
		WEIGHT[PATH_ESCALATOR] = 10;
		WEIGHT[PATH_STAIR|PATH_ESCALATOR] = 15;
		WEIGHT[PATH_ELEVATOR] = 1;

	}else if(MODE_SHORTEST == idras->weight.routingMod || MODE_RAIN_AVOID  == idras->weight.routingMod){

		if(abs(idras->end.cdnt.height - idras->start.cdnt.height) > idras->weight.maxWalkFloor || idras->weight.maxWalkFloor == 0){//if difference of goal and start is bigger than maxWalkFloor 

			WEIGHT[PATH_PASSAGE] = 1;
			WEIGHT[PATH_STAIR] = 15;
			WEIGHT[PATH_ESCALATOR] = 1;
			WEIGHT[PATH_STAIR|PATH_ESCALATOR] = 15;
			WEIGHT[PATH_ELEVATOR] = 2;
		}else{//if difference of goal and start is smaller than maxWalkFloor 

			WEIGHT[PATH_PASSAGE] = 1;
			WEIGHT[PATH_STAIR] = 1;
			WEIGHT[PATH_ESCALATOR] = 10;
			WEIGHT[PATH_STAIR|PATH_ESCALATOR] = 10;
			WEIGHT[PATH_ELEVATOR] = 15;
		}
	}
	
	return 0;
}

//adjust H value
III_INT setIdrWeighting(IndoorAStar* idras, Node neighbor){
	//printf("(%d,%d,%d)-attr->%d", neighbor.cdnt.longitude, neighbor.cdnt.latitude, neighbor.cdnt.height, neighbor.attr);
	
	neighbor.h = abs(idras->end.cdnt.longitude - neighbor.cdnt.longitude) + abs(idras->end.cdnt.latitude - neighbor.cdnt.latitude) + 70*abs(idras->end.cdnt.height - neighbor.cdnt.height);//使用曼哈頓方法（Manhattan method）計算H值
	
	if(MODE_ACCESSABLE == idras->weight.routingMod){//accessable mode, ELEVATOR first, ESCALATOR second, STAIR third
		
		/*if(PATH_ELEVATOR == neighbor.attr){	
			neighbor.h = neighbor.h;
		}else*/ if(PATH_ESCALATOR == neighbor.attr){
			neighbor.h = 10*neighbor.h;
		}else if(PATH_STAIR == neighbor.attr){
			neighbor.h = 15*neighbor.h;
		}else if(PATH_STAIR_AND_ESCALATOR == neighbor.attr){
			neighbor.h = 15*neighbor.h;
		}

	}else{//shotest mode or other mode
		//if difference of goal and start is bigger than maxWalkFloor, ESCALATOR first, ELEVATOR second, STAIR third 
		if(abs(idras->end.cdnt.height - idras->start.cdnt.height) > idras->weight.maxWalkFloor || idras->weight.maxWalkFloor == 0){
			if(PATH_ELEVATOR == neighbor.attr){	
				neighbor.h = 12*neighbor.h;
			}/*else if(PATH_ESCALATOR == neighbor.attr){
				neighbor.h = neighbor.h;
			}*/else if(PATH_STAIR == neighbor.attr){
				neighbor.h = 15*neighbor.h;
			}else if(PATH_STAIR_AND_ESCALATOR == neighbor.attr){
				neighbor.h = 10*neighbor.h;
			}

		}else{//if difference of goal and start is smaller than maxWalkFloor, STAIR first, ESCALATOR second, ELEVATOR third 
			if(PATH_ELEVATOR == neighbor.attr){	
				neighbor.h = 15*neighbor.h;
			}else if(PATH_ESCALATOR == neighbor.attr){
				neighbor.h = 10*neighbor.h;
			}/*else if(PATH_STAIR == neighbor.attr){
				neighbor.h = neighbor.h;
			}else if(PATH_STAIR_AND_ESCALATOR == neighbor.attr){
				neighbor.h = neighbor.h;
			}*/
		}
	}
	
	return neighbor.h;
}

//set indoor routing result by node(coordinate and attribute)
III_INT setIdrResultNode(IndoorAStar* idras, Node* stay){
	III_INT i;//for loop
	Node temp;
	III_INT pID = stay->parentID;
	III_RESULTNODE rNode;

	idras->result.numNode = 2;//number of node include fix edges of Start and End
	temp = *stay;

	//counting resultIdr.numEdge
	while(NODE_HAVE_NO_PARENT != pID){
		
		if(HASH_GET_SUCCESS == getHash(idras->nodeCache, &temp, pID)){
			idras->result.numNode ++;
			pID = temp.parentID;
		}else{
			return	ERROR_SET_RESULT_NODE_SUCCESS;
		}
	}

	//printf("\ncount:%d dist:%d", idras->result.amount, idras->result.totalDist);

	idras->result.resultNode = (III_RESULTINDOOR*)III_malloc(sizeof(III_RESULTINDOOR)*(idras->result.numNode));   // 弄個新空間;

	//add fix edges of End to result
	rNode.nodeID = -1;
	rNode.cdnt.longitude = idras->result.pntArrival.pntFix.longitude;
	rNode.cdnt.latitude = idras->result.pntArrival.pntFix.latitude;
	rNode.cdnt.height = idras->result.pntArrival.pntFix.height;
	rNode.attr = 0;
	memcpy((idras->result.resultNode + (idras->result.numNode-1)), &rNode, sizeof(III_RESULTNODE));    // 將該資料拷至指定位置
	
	temp = *stay;

	for(i = idras->result.numNode - 2; i>0; i--){
		
		//*(idras->result.edgeID + i) = stay->edgeID;
		//printf("\n%d->", *(idras->result.edgeID + i));
		rNode.nodeID = temp.nodeID;
		rNode.cdnt.longitude = temp.cdnt.longitude;
		rNode.cdnt.latitude = temp.cdnt.latitude;
		rNode.cdnt.height = temp.cdnt.height;
		rNode.attr = temp.attr;
		//printf("stay->parentID:%d",stay->parentID);
		memcpy((idras->result.resultNode)+i, &rNode, sizeof(III_RESULTNODE));    // 將該資料拷至指定位置
		if(HASH_GET_FAIL == getHash(idras->nodeCache, &temp, stay->parentID)){
			
			return ERROR_SET_RESULT_NODE_FAIL;
		}
		stay->parentID = temp.parentID;
	}

	//add fix edges of Start to result
	rNode.nodeID = -1;
	rNode.cdnt.longitude = idras->result.pntDeparture.pntFix.longitude;
	rNode.cdnt.latitude = idras->result.pntDeparture.pntFix.latitude;
	rNode.cdnt.height = idras->result.pntDeparture.pntFix.height;
	rNode.attr = 0;
	memcpy(idras->result.resultNode, &rNode, sizeof(III_RESULTNODE));    // 將該資料拷至指定位置

	return ERROR_SET_RESULT_NODE_SUCCESS;
}

//set indoor routing result
III_INT setIdrResultEdge(IndoorAStar* idras, Node* stay, III_INT flag){
	III_INT i;//for loop
	Node temp;
	III_PDRTEDGE edge;
	III_INT pID = stay->parentID;

	idras->resultIdr.numEdge = 2;//number of edge include fix edges of Start and End
	temp = *stay;

	if(0 == flag){//set edge distance by tnode or fnode
		edge.distanceEdge = idras->fEndEdgeDist;
	}else{
		edge.distanceEdge = idras->tEndEdgeDist;
	}

	//counting resultIdr.numEdge
	while(NODE_HAVE_NO_PARENT != pID){
		
		if(HASH_GET_SUCCESS == getHash(idras->nodeCache, &temp, pID)){
			idras->resultIdr.numEdge ++;
			pID = temp.parentID;
		}else{
			return	ERROR_SET_RESULT_SUCCESS;
		}
	}
	//printf("\ncount:%d dist:%d", idras->result.amount, idras->result.totalDist);

	idras->resultIdr.resultEdge = (III_PDRTEDGE*)III_malloc(sizeof(III_PDRTEDGE)*(idras->resultIdr.numEdge));   // 弄個新空間;

	//add fix edges of End to result
	edge.idEdge = idras->resultIdr.pntArrival.nLineSegIdxFix;
	memcpy((idras->resultIdr.resultEdge + (idras->resultIdr.numEdge-1)), &edge, sizeof(III_PDRTEDGE));    // 將該資料拷至指定位置
	
	temp = *stay;

	for(i = idras->resultIdr.numEdge - 2; i>0; i--){
		
		//*(idras->result.edgeID + i) = stay->edgeID;
		//printf("\n%d->", *(idras->result.edgeID + i));
		edge.idEdge = temp.edgeID;
		edge.distanceEdge = temp.dist;
		//printf("stay->parentID:%d",stay->parentID);
		memcpy((idras->resultIdr.resultEdge)+i, &edge, sizeof(III_PDRTEDGE));    // 將該資料拷至指定位置
		if(HASH_GET_FAIL == getHash(idras->nodeCache, &temp, stay->parentID)){
			
			return ERROR_SET_RESULT_FAIL;
		}
		stay->parentID = temp.parentID;
	}

	//add fix edges of Start to result
	edge.idEdge = idras->resultIdr.pntDeparture.nLineSegIdxFix;
	edge.distanceEdge = temp.dist;//because upon while loop, temp is fnode or tnode of start
	memcpy(idras->resultIdr.resultEdge, &edge, sizeof(III_PDRTEDGE));    // 將該資料拷至指定位置

	/*	
	III_INT i;//for loop
	Node* temp = stay;
	IdrEdge edge;
	
	//testing code, print routing result
	//printf("\npath: %d <-(%d:%d)-", temp->nodeID, temp->edgeID, temp->dist);
	idras->result.amount = 2;//fix edges of start and end
	
	if(0 == flag){
		idras->result.totalDist = temp->dist + idras->fEndEdgeDist;
		edge.dist = idras->fEndEdgeDist;
	}else{
		idras->result.totalDist = temp->dist + idras->tEndEdgeDist;
		edge.dist = idras->fEndEdgeDist;
	}
	
	while(0 != temp->parentID){
		idras->result.amount ++;
		idras->result.totalDist += temp->parent->dist;
		//printf(" %d <-(%d:%d)- ", temp->parent->nodeID, temp->parent->edgeID, temp->parent->dist);
		temp = temp->parent;
	}
	//printf("\ncount:%d dist:%d", idras->result.amount, idras->result.totalDist);

	idras->result.edge = (IdrEdge*)III_malloc(sizeof(IdrEdge)*(idras->result.amount));   // 弄個新空間;
	edge.edgeID = idras->end.edgeID;
	memcpy((idras->result.edge + (idras->result.amount-1)), &edge, sizeof(IdrEdge));    // 將該資料拷至指定位置
	
	for(i = idras->result.amount - 2; i>=0; i--){
		
		//*(idras->result.edgeID + i) = stay->edgeID;
		//printf("\n%d->", *(idras->result.edgeID + i));
		edge.edgeID = stay->edgeID;
		edge.dist = stay->dist;
		memcpy((idras->result.edge)+i, &edge, sizeof(IdrEdge));    // 將該資料拷至指定位置
		stay = stay->parent;
	}
	*/
	return ERROR_SET_RESULT_SUCCESS;
}

//set indoor routing result
III_INT setIdrResult(IndoorAStar* idras, Node* stay, III_INT flag){
	III_INT i;//for loop
	Node temp;
	III_PDRTEDGE edge;
	III_RESULTNODE rNode;
	III_INT pID = stay->parentID;

	idras->resultIdr.numEdge = 2;//number of edge include fix edges of Start and End
	idras->result.numNode = 2;//number of node include fix edges of Start and End
	temp = *stay;

	if(0 == flag){//set edge distance by tnode or fnode
		edge.distanceEdge = idras->fEndEdgeDist;
	}else{
		edge.distanceEdge = idras->tEndEdgeDist;
	}

	//counting resultIdr.numEdge
	while(NODE_HAVE_NO_PARENT != pID){
		
		if(HASH_GET_SUCCESS == getHash(idras->nodeCache, &temp, pID)){
			idras->resultIdr.numEdge ++;
			idras->result.numNode ++;
			pID = temp.parentID;
		}else{
			return	ERROR_SET_RESULT_SUCCESS;
		}
	}
	//printf("\ncount:%d dist:%d", idras->result.amount, idras->result.totalDist);

	idras->resultIdr.resultEdge = (III_PDRTEDGE*)III_malloc(sizeof(III_PDRTEDGE)*(idras->resultIdr.numEdge));   // 弄個新空間;
	idras->result.resultNode = (III_RESULTINDOOR*)III_malloc(sizeof(III_RESULTINDOOR)*(idras->result.numNode));   // 弄個新空間;

	//add fix edges of End to result
	edge.idEdge = idras->resultIdr.pntArrival.nLineSegIdxFix;
	memcpy((idras->resultIdr.resultEdge + (idras->resultIdr.numEdge-1)), &edge, sizeof(III_PDRTEDGE));    // 將該資料拷至指定位置

	//add fix edges of End to result
	rNode.nodeID = -1;
	rNode.cdnt.longitude = idras->result.pntArrival.pntFix.longitude;
	rNode.cdnt.latitude = idras->result.pntArrival.pntFix.latitude;
	rNode.cdnt.height = idras->result.pntArrival.pntFix.height;
	rNode.attr = 0;
	memcpy((idras->result.resultNode + (idras->result.numNode-1)), &rNode, sizeof(III_RESULTNODE));    // 將該資料拷至指定位置
	
	temp = *stay;

	for(i = idras->resultIdr.numEdge - 2; i>0; i--){
		
		//*(idras->result.edgeID + i) = stay->edgeID;
		//printf("\n%d->", *(idras->result.edgeID + i));
		edge.idEdge = temp.edgeID;
		edge.distanceEdge = temp.dist;
		//printf("stay->parentID:%d",stay->parentID);
		memcpy((idras->resultIdr.resultEdge)+i, &edge, sizeof(III_PDRTEDGE));    // 將該資料拷至指定位置

		//*(idras->result.edgeID + i) = stay->edgeID;
		//printf("\n%d->", *(idras->result.edgeID + i));
		rNode.nodeID = temp.nodeID;
		rNode.cdnt.longitude = temp.cdnt.longitude;
		rNode.cdnt.latitude = temp.cdnt.latitude;
		rNode.cdnt.height = temp.cdnt.height;
		rNode.attr = temp.attr;
		//printf("stay->parentID:%d",stay->parentID);
		memcpy((idras->result.resultNode)+i, &rNode, sizeof(III_RESULTNODE));    // 將該資料拷至指定位置

		if(HASH_GET_FAIL == getHash(idras->nodeCache, &temp, stay->parentID)){
			
			return ERROR_SET_RESULT_FAIL;
		}
		stay->parentID = temp.parentID;
	}

	//add fix edges of Start to result
	edge.idEdge = idras->resultIdr.pntDeparture.nLineSegIdxFix;
	edge.distanceEdge = temp.dist;//because upon while loop, temp is fnode or tnode of start
	memcpy(idras->resultIdr.resultEdge, &edge, sizeof(III_PDRTEDGE));    // 將該資料拷至指定位置

	//add fix edges of Start to result
	rNode.nodeID = -1;
	rNode.cdnt.longitude = idras->result.pntDeparture.pntFix.longitude;
	rNode.cdnt.latitude = idras->result.pntDeparture.pntFix.latitude;
	rNode.cdnt.height = idras->result.pntDeparture.pntFix.height;
	rNode.attr = 0;
	memcpy(idras->result.resultNode, &rNode, sizeof(III_RESULTNODE));    // 將該資料拷至指定位置

	return ERROR_SET_RESULT_SUCCESS;
}


//get indoor routing result
III_INT freeIdrResult(IndoorAStar* idras){
	III_free(idras->resultIdr.resultEdge);//release hash array
	III_free(idras->result.resultNode);//release hash array
}

//get Neighbor info of Node
III_INT getIdrNeighbor(IndoorAStar* idras, Node* neighbor, III_INT offset, III_INT order){
	
	III_UCHAR nbbuf[MAXNEIGHBOR]; //read file buffer(neighbor)
	Node newneighbor;
	III_INT *intTemp;

	III_fseek(idras->binfile, offset+(order*MAXNEIGHBOR), 0); //shift offset + order to read info

	if(!feof(idras->binfile)){	
		III_fread(nbbuf, sizeof(III_CHAR), MAXNEIGHBOR, idras->binfile);//file read

		//printf("\n///////////////");

		//get offset
		newneighbor.offset = ((0x000000ff & nbbuf[3]) << 20) | ((0x000000ff & nbbuf[2]) << 12) | ((0x000000ff & nbbuf[1]) << 4) | ((0x000000ff & nbbuf[0]) >> 4);
		//printf("\n offset:%d", newneighbor.offset);

		//get amount
		newneighbor.amount = 0x0000000f & nbbuf[0];
		//printf("\n amount:%d", newneighbor.amount);

		//get nodeID
		//newneighbor.nodeID = ((0x000000ff & nbbuf[7]) << 6) | ((0x000000ff & nbbuf[6]) >> 2);
		newneighbor.nodeID = ((0x000000ff & nbbuf[5]) << 8) | (0x000000ff & nbbuf[4]);
		printf("\n nodeID:%d", newneighbor.nodeID);

		//get edgeID
		//newneighbor.edgeID = ((0x00000003 & nbbuf[6]) << 12) | ((0x000000ff & nbbuf[5]) << 4) | ((0x000000ff & nbbuf[4]) >> 4);
		newneighbor.edgeID = ((0x000000ff & nbbuf[7]) << 8) | (0x000000ff & nbbuf[6]);
		//printf("\n edgeID:%d", newneighbor.edgeID);

		//get distance		
		newneighbor.dist = ((0x000000ff & nbbuf[9]) << 8) | (0x000000ff & nbbuf[8]);
		//printf("\n dist:%d", newneighbor.dist);

		//get passage flag
		newneighbor.pflag = (0x000000f0 & nbbuf[10]) >> 4 ;
		//printf("\n pflag:%d", newneighbor.pflag);

		//get attr
		newneighbor.attr = 0x0000000f & nbbuf[10];
		//printf("\n attribute:%d", newneighbor.attr);

		//get floor
		if(0x80 == (0x80 & nbbuf[11])){
			newneighbor.cdnt.height = 0xffffff00 | nbbuf[11];
		}else{	
			newneighbor.cdnt.height = 0x0000000f & nbbuf[11];
		}
		//printf("\n floor:%d", newneighbor.cdnt.height);

		//get lontitude
		intTemp = &nbbuf[12];
		newneighbor.cdnt.longitude = *intTemp;
		//newneighbor.cdnt.longitude = ((0x000000ff & nbbuf[15]) << 24) | ((0x000000ff & nbbuf[14]) << 16) | ((0x000000ff & nbbuf[13]) << 8) | (0x000000ff & nbbuf[12]);
		//printf("\n x:%d", newneighbor.cdnt.longitude);

		//get latitude
		intTemp = &nbbuf[16];
		newneighbor.cdnt.latitude = *intTemp;
		//newneighbor.cdnt.latitude = ((0x000000ff & nbbuf[19]) << 24) | ((0x000000ff & nbbuf[18]) << 16) | ((0x000000ff & nbbuf[17]) << 8) | (0x000000ff & nbbuf[16]);
		//printf("\n y:%d", newneighbor.cdnt.latitude);
		
		memcpy(neighbor, &newneighbor, sizeof(Node));    // 將該資料拷至指定位置
	}

	return 0;
}

//put Node into Hash, 0 is new Node, 1 is refrash Node info
III_INT addHash(HashNode hash[], Node node, III_INT nodeID){
	
	III_INT hashNum = nodeID % MAXHASH;
	HashNode *hashnode;
	HashNode newnode;
	
	hashnode = &(hash[hashNum]);

	if(0 == hash[hashNum].hnode.amount){//if hash[] is empty, put into array
		newnode.hashID = nodeID;
		newnode.hnode = node;
		newnode.next = NULL;
		hash[hashNum] = newnode;
		return 0;
	}else if(nodeID == hash[hashNum].hashID){//if Node is exist(hashID equal to nodeID), refrash Node info.
		hash[hashNum].hnode = node;
		return 1;
	}else{//if hash[] is not empty, add to backward by linking list
		hashnode = &(hash[hashNum]);

		while(NULL != hashnode->next){
			hashnode = hashnode->next;

			if(nodeID == hashnode->hashID){//if Node is exist(hashID equal to nodeID), refrash Node info.
				hashnode->hnode = node;
				//memcpy(hashnode->hnode, node, sizeof(Node));    // 將該資料拷至指定位置
				return 1;
			}
		}

		newnode.hashID = nodeID;
		newnode.hnode = node;
		newnode.next = NULL;
		hashnode->next = (HashNode*)III_malloc(sizeof(HashNode));   // 弄個新空間;
		memcpy(hashnode->next, &newnode, sizeof(HashNode));    // 將該資料拷至指定位置
		return 0;
	}	
}

//get Node from Hash, 0 is geted, 1 is not.
III_INT getHash(HashNode hash[], Node *node, III_INT nodeID){
	
	III_INT hashNum = nodeID % MAXHASH;
	HashNode *hashnode = &(hash[hashNum]);

	while(0 != hashnode->hnode.amount){
		if(hashnode->hashID == nodeID){
			memcpy(node, &(hashnode->hnode), sizeof(Node));    // 將該資料拷至指定位置
			return HASH_GET_SUCCESS;
		}
		hashnode = hashnode->next;
	}

	node = NULL;
	return HASH_GET_FAIL;
}

//get G value of Node from Hash, -1 is not in Hash.
III_INT getHashGValue(HashNode hash[], III_INT nodeID){
	III_INT hashNum = nodeID % MAXHASH;
	HashNode *hashnode = &(hash[hashNum]);

	while(0 != hashnode->hnode.amount){
		if(hashnode->hashID == nodeID){
			return hashnode->hnode.g;
		}
		hashnode = hashnode->next;
	}

	return HASH_GET_GVALUE_FAIL;
}

//Add node to close list.
III_INT addCloseList(IndoorAStar *idras, III_INT nodeID){
	
	III_INT listShift;
	III_INT maskShift;

	if(0 > nodeID){
		return ERROR_CLLIST_NID_LTZ;
	}else{
		listShift = nodeID / 8;
		maskShift = nodeID % 8;
	}
	//printf("\n ID:%d list:%d mask:%d", nodeID, listShift, maskShift);
	//printf("\n mask:%2X", *(idras->cllist + listShift));
	*((idras->clList.list) + listShift) = *((idras->clList.list) + listShift) | BITMASK[maskShift];
	//printf("\n mask:%2X", *(idras->cllist + listShift));
	//getch();
	return ERROR_CLLIST_ADD_SUCCESS;
}

//Check that node have added to close list. 0 is added. 1 is not.
III_INT checkCloseList(IndoorAStar *idras, III_INT nodeID){
	
	III_INT listShift;
	III_INT maskShift;
	III_CHAR cmp;

	if(0 > nodeID){
		return ERROR_CLLIST_NID_LTZ;
	}else{
		listShift = nodeID / 8;
		maskShift = nodeID % 8;
	}

	cmp = *((idras->clList.list) + listShift) & BITMASK[maskShift];

	if(0 == strncmp(&(BITMASK[maskShift]), &cmp, 1)){
		return CLLIST_HAS_ADDED;
	}else{
		return CLLIST_HAS_NOT_ADDED;
	}
	
}

//Initial A*, read header of .bin file.
III_INT initialAStar(IndoorAStar *idras){
	III_UCHAR headbuf[MAXHEADER]; //read file buffer(header)
	HashNode hash[MAXHASH] = {NULL};//hash初始值為零
	III_INT i; //for loop
	// Norman modify
	III_INT *intTemp = NULL;


	idras->binfile = fopen(idras->binName, "rb");
	if(!idras->binfile) { 
        puts("無法讀入檔案");
		//getch();
        return 1; 
    }

	// Norman do not know why eof??? if less than header size
	// we could use fseek the file end to check the file size if less than header size
	III_fseek(idras->binfile, 0, SEEK_END);

	if(III_ftell(idras->binfile) > (MAXHEADER+MAXNEIGHBOR*2+MAXEDGE)){
	//if(!feof(idras->binfile)){
		III_fseek(idras->binfile, SEEK_SET, 0); 
		III_fread(headbuf, sizeof(III_CHAR), MAXHEADER, idras->binfile);

		//get maxN		
		idras->header.maxN = ((0x000000ff & headbuf[1]) << 8) | (0x000000ff & headbuf[0]);
		//printf("\n maxN:%d", idras->header.maxN);
		
		//set closelist amount by maxN
		// Norman modify
		//idras->clList.num = idras->header.maxN / 8; 
		//if(idras->header.maxN % 8 > 0){
		idras->clList.num = idras->header.maxN >> 3; 
		if( 0 != (idras->header.maxN & 0x07)){
			idras->clList.num++;
		}

		idras->clList.list = (III_CHAR*)III_malloc(sizeof(III_CHAR)*(idras->clList.num));//locate memery to close list	

		for(i=0; i<idras->clList.num; i++){				
			*(idras->clList.list + i) = 0;
		}

		//printf("\n cllist:%2X", idras->clList.list);
		//printf("\n clSize:%d", idras->clList.num);

		//get maxE		
		idras->header.maxE = ((0x000000ff & headbuf[3]) << 8) | (0x000000ff & headbuf[2]);
		//printf("\n maxE:%d", idras->header.maxE);

		//get fullsize
		// Norman modify
		intTemp = &headbuf[4];
		idras->header.fullsize = *intTemp;
		//idras->header.fullsize = ((0x000000ff & headbuf[7]) << 24) | ((0x000000ff & headbuf[6]) << 16) | ((0x000000ff & headbuf[5]) << 8) | (0x000000ff & headbuf[4]);
		printf("\n fullsize:%d", idras->header.fullsize);
	

		//get buildingID
		intTemp = &headbuf[8];
		idras->header.buildingID = *intTemp;
		//idras->header.buildingID = ((0x000000ff & headbuf[11]) << 24) | ((0x000000ff & headbuf[10]) << 16) | ((0x000000ff & headbuf[9]) << 8) | (0x000000ff & headbuf[8]);
		printf("\n buildingID:%d", idras->header.buildingID);

		//get dxdyScale
		idras->header.dxdyScale = 0x000000ff & headbuf[12];
		//printf("\n dxdyScale:%d", idras->header.dxdyScale);

		//get distScale
		idras->header.distScale = 0x000000ff & headbuf[13];
		//printf("\n distScale:%d", idras->header.distScale);

		//get mapScale 
		idras->header.mapScale = 0x000000ff & headbuf[14];
		//printf("\n mapScale:%d", idras->header.mapScale);
	}

	idras->opList.count = 0;
	idras->nodeCache = (HashNode*)III_malloc(sizeof(HashNode)*(MAXHASH));//locate memery to hash

	for(i=0; i<MAXHASH; i++){
		memset((idras->nodeCache + i), NULL, sizeof(HashNode));//initialize hash
	}

	idras->stopFlag = 0;//set A* stop flag

	return 0;
}

//release memory, clear start & goal, close .bin file
III_INT finishAStar(IndoorAStar *idras){
	III_INT i;//for loop
	
	III_fclose(idras->binfile);//close .bin file
	III_free(idras->clList.list);//release close list
	freeHash(idras->nodeCache);
	III_free(idras->nodeCache);//release hash array
		
	return 0;
}

//release memory of hash
III_INT freeHash(HashNode hash[]){
	III_INT i;//for loop
	HashNode *hashnode;
	HashNode *nextnode;
	
	for(i=0; i<MAXHASH; i++){
		if(NULL != hash[i].next){
			nextnode = hash[i].next;
			while(NULL != nextnode->next){//由前往後free
				hashnode = nextnode;
				nextnode = nextnode->next;
				III_free(hashnode);
			}
			III_free(nextnode);
		}
	}
	return 0;
}

//insert node to heap
III_INT insertHeaptree(Heaptree *tree, Node *node){
	III_INT p; //tree[parent]
	III_INT c; //tree[child]

	tree->count = tree->count + 1;
	p = tree->count/2;
	c = tree->count;
	tree->heap[tree->count] = *node;

	while(p >= 1 && tree->heap[p].f > tree->heap[c].f) {
	   	swapHeap(tree->heap, p, c);
	    c = p;
	    p = c/2;
	}
	return 0;
}

//delete root of heap
III_INT deleteHeaptreeRoot(Heaptree *tree){
	III_INT p = 1;
	III_INT c = 2 * p;

	tree->heap[1] = tree->heap[tree->count];//root與 tail交換
	tree->count = tree->count - 1;//count-1

	//調整heap tree

    while(c <= tree->count) {
    	if(c < tree->count && tree->heap[c+1].f < tree->heap[c].f) {
    		c++;
	    }
	    if(tree->heap[p].f <= tree->heap[c].f) {
	    	break;
	    }
	    swapHeap(tree->heap, p, c);
	    p = c;
	    c = 2 * p;
	}
	//root = tree[1];
	return 0;
}

//testing code, print heap
III_INT printHeaptree(Heaptree tree){
	III_INT i;

	printf("\n%d: ", tree.count);
	for(i = 1; i < tree.count + 1; i++){
		printf("%d:%d, ", tree.heap[i].nodeID, tree.heap[i].f);
	}
	printf("\n");
	return 0;
}

//swap node of heap
III_INT swapHeap(Node tree[], III_INT i, III_INT j){
	Node t = tree[i];
	tree[i] = tree[j];
	tree[j] = t;
	return 0;
}

//get distance between a node and b node
III_INT getDistance(III_INT ax, III_INT ay, III_INT bx, III_INT by){
	//III_DOUBLE distance = sqrt((pow((ax - bx), 2) + pow((ay - by), 2))*pow((60*1.852*1000), 2)) * (0.000001);
	//Norman modify
	//(abs(edge.x-endX)+abs(edge.y-endY))
	III_DOUBLE distance = (abs(ax - bx) + abs(ay - by))*(60*1.852*1000) * (0.000001);
	return (III_INT)distance;
}

