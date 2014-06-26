// IdrPOISearch.cpp : Defines the entry point for the console application.
//
#include "IdrPOISearch.h"
#include <string.h>

extern char g_szMapFolder[256];

int IdrPOISearch(int *odrLong, int *odrLat, int idrFloor,	char *idrType, char *building) {

	char sBuildingFile[256]; 
	int distance = 0;
	
	IndoorPOI *filePOI = (IndoorPOI*)malloc(sizeof(IndoorPOI));   // 弄個新空間;
	IndoorPOI *tempPOI = (IndoorPOI*)malloc(sizeof(IndoorPOI));   // 弄個新空間;
	FILE *file = NULL;
 
	strcpy(sBuildingFile, g_szMapFolder);
    strcat(sBuildingFile, building);
    strcat(sBuildingFile, ".poi"); 
	tempPOI->floor = -100;
	file = fopen(sBuildingFile, "r"); 
    if(!file) { 
        puts("無法讀入檔案 sBuildingFile");  
        return OPEN_FILE_FAIL; 
    }

	while(fscanf(file, "%d\t%d\t%d\t%s", &(filePOI->latitude), &(filePOI->longitude), &(filePOI->floor), filePOI->type) != EOF) { 
		if(idrFloor == filePOI->floor && strcmp(idrType, filePOI->type) == 0){
			int tempDist = CountDistance(*odrLat, *odrLong, filePOI->latitude, filePOI->longitude);
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

	if(idrFloor == tempPOI->floor){
		//printf("temp:%d\t%d\t%d\t%s\n", tempPOI->latitude, tempPOI->longitude, tempPOI->floor, tempPOI->type); 
		*odrLat = tempPOI->latitude;
		*odrLong = tempPOI->longitude;

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