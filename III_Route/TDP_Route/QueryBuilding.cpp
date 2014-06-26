#include <stdlib.h>
#include <tchar.h>
#include "QueryBuilding.h"

extern TCHAR g_wszMapFolder[256];

III_BOOL QueryBuildingString(int nLongitude, int nLatitude, char *sBuildingString)
{
    char sTempBuildingString[10]; 
    int  nMinLong, nMinLat, nMaxLong, nMaxLat;
    III_BOOL IsInBuilding = III_FALSE;
    
	TCHAR wszFilePath[256];
	FILE *file = NULL;

	wcscpy(wszFilePath, g_wszMapFolder);
	wcscat(wszFilePath, _T("BuildingString.str"));

	file = _wfopen(wszFilePath, _T("r")); 

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

