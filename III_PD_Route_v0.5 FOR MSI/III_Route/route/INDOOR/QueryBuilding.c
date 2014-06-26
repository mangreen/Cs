#include <string.h>
#include "QueryBuilding.h"

III_BOOL QueryBuildingString(int nLongitude, int nLatitude, char *sBuildingString) {

    char sTempBuildingString[10]; 
    int  nMinLong, nMinLat, nMaxLong, nMaxLat;
    III_BOOL IsInBuilding = III_FALSE;
    
	FILE *file = fopen("BuildingString.str", "r"); 
    if(!file) { 
        puts("無法讀入檔案 BuildingString.str"); 
        return 1; 
    }

	
	while(fscanf(file, "%d\t%d\t%d\t%d\t%s", &nMinLong, &nMinLat, &nMaxLong, &nMaxLat,  sTempBuildingString) != EOF) { 
	    if(nMinLong > nMaxLong)
	        Swap(&nMinLong, &nMaxLong);
	    
	    if(nMinLat > nMaxLat)
	        Swap(&nMinLat, &nMaxLat);
	    
	    if(nLongitude >= nMinLong && nLongitude <= nMaxLong && nLatitude >= nMinLat && nLatitude <= nMaxLat) {
	        strcpy(sBuildingString, sTempBuildingString); 
	        IsInBuilding = III_TRUE; 
	    }  	
    } 

	fclose(file);
	
	return IsInBuilding; 
}

void Swap(int *a, int *b)
{
    int Temp;
    
    Temp = *a;
    *a = *b;
    *b = Temp;   
}

