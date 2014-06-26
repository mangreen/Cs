#include "stdafx.h"
#include "IdrPOISearch.h"

int main(int argc, char* argv[]) {

	int orgLat = 25041000; 
	int orgLong = 121565000;
	int orgFloor = 1;
	char orgType[10] = "exit";


	if(SEARCH_INDOOR_POI_SUCCESS == IdrPOISearch(&orgLat, &orgLong,	&orgFloor,	&orgType, "mrt_yc.poi")){
		printf("test:%d\t%d\t%d\t%s\n", orgLat, orgLong, orgFloor, orgType);
	}

	_getch();
    return 0; 
}