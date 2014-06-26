// IdrPOISearch.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "IdrPOISearch.h"


int IdrPOISearch(int *orgLat, int *orgLong,	int *orgFloor,	char *orgType, char *building) {

	//int poiLat;
	//int poiLong;
	//int poiFloor;
	//char poiType[10];

	//int tempLat;
	//int tempLong;
	//int tempFloor;
	//char tempType[10];

	int distance = 0;
	IndoorPOI *filePOI = (IndoorPOI*)malloc(sizeof(IndoorPOI));   // 弄個新空間;
	IndoorPOI *tempPOI = (IndoorPOI*)malloc(sizeof(IndoorPOI));   // 弄個新空間;

	FILE *file = fopen(building, "r"); 
    if(!file) { 
        puts("無法讀入檔案"); 
        return OPEN_FILE_FAIL; 
    }

	
	printf("orig:%d\t%d\t%d\t%s\n", *orgLat, *orgLong, *orgFloor, orgType);
	/*
	*orgLat=1; 
	*orgLong=2; 
	*orgFloor=3; 
	//*orgType="platform";
	memcpy(orgType, "platform", 10);
	*/

	puts("Latitude\tLongitude\tFloor\tCatalog"); 
	while(fscanf(file, "%d\t%d\t%d\t%s", &(filePOI->latitude), &(filePOI->longitude), &(filePOI->floor), filePOI->type) != EOF) { 
		if(*orgFloor == filePOI->floor){
			int tempDist = CountDistance(orgLat, orgLong, filePOI->latitude, filePOI->longitude);
			//printf("dist: %d\t", tempDist); 
			if(0 == distance || distance > tempDist){
				distance = tempDist;
				tempPOI->latitude = filePOI->latitude;
				tempPOI->longitude = filePOI->longitude;
				tempPOI->floor = filePOI->floor;
				memcpy(tempPOI->type, filePOI->type, sizeof(char)*TYPE_SIZE);
			}
		}
        //printf("%d\t%d\t%d\t%s\n", filePOI->latitude, filePOI->longitude, filePOI->floor, filePOI->type);
    } 

	if(*orgFloor == tempPOI->floor){
		printf("temp:%d\t%d\t%d\t%s\n", tempPOI->latitude, tempPOI->longitude, tempPOI->floor, tempPOI->type); 
		*orgLat = tempPOI->latitude;
		*orgLong = tempPOI->longitude;

		fclose(file);

		free(filePOI);
		free(tempPOI);

		return SEARCH_INDOOR_POI_SUCCESS; 
	}

	fclose(file);
	
	free(filePOI);
	free(tempPOI);
	
	return SEARCH_INDOOR_POI_FAIL; 
}

int CountDistance(int ax, int ay, int bx, int by){
	double dist = abs(ax-bx)+abs(ay-by);
	return (int)dist;
}