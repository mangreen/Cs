/*
*	CopyRight : ESTI_資策會新興智慧所
*	Description : III_Pedestrian-Route 動態資料存取變數定義
*	Version & Date : v0.3  2010/5/24
*/

#ifndef SETTING_H
#define SETTING_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string.h>

using namespace std;

//file name setting
#define MIDNAME ".mid"
#define MIFNAME ".mif"
const char OUTPUTNAME[16] = ".dat";
const char OUTPUTDIR[16] = "output";

//mid table setting
#define F_NODE_NUM 1		//Get FnodeID attribute
#define T_NODE_NUM 2		//Get TnodeID attribute
#define IS_STAIR 5			//Get Stair attribute
#define IS_SHELTER 6		//Get Shader attribute
#define IS_ACCESSABLE 7		//Get Accessibility attribute

#endif