/*
*	CopyRight : ESTI_資策會新興智慧所
*	Description : III_Pedestrian-Route WIN32 Function定義
*	Version & Date : v0.3  2010/5/24
*/

#ifndef _III_WIN32_H_
#define _III_WIN32_H_

#define III_PORTING_WIN32

/**
 * add corresponding OS/platform depend header files below
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/**
 * add corresponding OS/platform definitions below
 */
#if defined(III_64BIT)
  #undef  III_64BIT
  #define III_64BIT   _int64
#endif

#if defined(III_U64BIT)
  #undef  III_U64BIT
  #define III_U64BIT  unsigned _int64
#endif

III_FILE *III_fopen(const III_CHAR *_filename, const III_CHAR *_mode);
III_INT III_fclose(III_FILE *_fpointer);
III_INT III_feof(III_FILE *_fpointer);
III_INT III_fseek(III_FILE *_fpointer, III_SIZE _seeksize, III_INT _whence);
III_LONG III_ftell(III_FILE *_fpointer);
III_SIZE III_fread(void *_ptr, III_SIZE _size, III_SIZE _nmemb, III_FILE *_fpointer);
III_SIZE III_fwrite(const III_VOID *_ptr, III_SIZE _size, III_SIZE _nmemb, III_FILE *_fpointer);
III_CHAR *III_fgets(III_CHAR *_str, III_SIZE _size, III_FILE *_fpointer);
III_INT III_fputs(const III_CHAR *_str, III_FILE *_fpointer);

#endif/*_III_WIN32_H_*/
