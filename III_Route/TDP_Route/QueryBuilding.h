
#ifndef _QUERY_BUILDING_H_
#define _QUERY_BUILDING_H_

#include "prototype.h"
#include "III_Route_Manager.h"


#if defined(__cplusplus)
extern "C" {
#endif

III_BOOL QueryBuildingString(int nLongitude, int nLatitude, char *sBuildingString);
void Swap(int *a, int *b);


#if defined(__cplusplus)
}
#endif

#endif
