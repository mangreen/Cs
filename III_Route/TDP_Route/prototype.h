/*
*	CopyRight : ESTI_資策會新興智慧所
*	Description : 跨平台變數定義
*	Version & Date : v0.3  2010/5/24
*/
#ifndef _PROTOTYPE_H_
#define _PROTOTYPE_H_

#include <windows.h>
/*
*   Error Definition
*/

/*
 * Data Types
 */

//#define III_CHAR          signed char
#define III_CHAR          char
#define III_UCHAR         unsigned char

#define III_SHORT         signed short
#define III_USHORT        unsigned short

#define III_INT           /*signed*/ int
#define III_UINT          unsigned int

#define III_FLOAT         float
#define III_DOUBLE        double

#define III_LONG          long
#define III_ULONG         unsigned long

#define III_64BIT         void
#define III_U64BIT        void     /* use particular implementation regarding platforms */

#define III_BOOL          char     /* use char for boolean type in case lack of native `bool' support */
#define III_TRUE          1
#define III_FALSE         0

#define III_VOID          void

#define III_NULL          0       /* for 0x00000000 cases */
#define III_INVALID       -1			/* for 0xffffffff cases */

#define III_S01           III_CHAR
#define III_U01           III_UCHAR
#define III_S02           III_SHORT
#define III_U02           III_USHORT
#define III_S04           III_INT
#define III_U04           III_UINT
#define III_S08           III_64BIT
#define III_U08           III_U64BIT

/*
 * Memory Management
 */

#define III_SIZE          III_UINT

#ifdef HAVE_MEMORY_DETAIL_IMPLEMENTATION
III_VOID*                 III_memcpy(III_VOID *, const III_VOID *, III_SIZE);
III_VOID*                 III_memset(III_VOID *, III_INT, III_SIZE);
III_VOID                  III_bzero(III_VOID *, III_SIZE);
#endif

/*
 * File Operations
 */


//#define III_FILE          FILE
#define III_FILE           void
#define III_SEEK_SET      0
#define III_SEEK_CUR      1
#define III_SEEK_END      2

III_FILE*                 III_fopen(const III_CHAR*, const III_CHAR*);
III_INT                   III_fclose(III_FILE *);
III_INT                   III_feof(III_FILE *);
III_INT                   III_fseek(III_FILE *, III_SIZE, III_INT );
III_SIZE                  III_fread(III_VOID*, III_SIZE , III_SIZE , III_FILE *);
III_SIZE                  III_fwrite(const III_VOID*, III_SIZE, III_SIZE, III_FILE *);

III_CHAR*                 III_fgets(III_CHAR *, III_SIZE, III_FILE *);
III_INT                   III_fputs(const III_CHAR *, III_FILE *);

/*
 * Platform-depend functions
 */

III_VOID                  III_USleep(III_UINT); /* sleep resolution in microseconds */
III_VOID                  III_MSleep(III_UINT);  /* sleep resolution in milliseconds */
III_VOID                  III_SSleep(III_UINT);  /* sleep resolution in seconds */
III_VOID                  III_sleep(III_UCHAR, III_UINT); /* multi-functional sleep function portol */

/*
 * Serial Communication functions
 */

III_VOID*                III_SerialPort_Init ( III_VOID *, III_INT, III_CHAR, III_CHAR, III_CHAR );
//HANDLE					  III_SerialPort_Init ( TCHAR *, III_INT);
III_VOID                 III_SerialPort_Shutdown ( III_VOID * );
III_INT                III_SerialPort_ReadLine ( III_CHAR *, III_UINT );

#endif/*_PORTING_H_*/
