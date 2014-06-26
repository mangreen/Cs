/*
*	CopyRight : ESTI_資策會新興智慧所
*	Description : III_Pedestrian-Route WIN32 Function定義
*	Version & Date : v0.3  2010/5/24
*/

#include "prototype.h"
#include "III_win32.h"

/*
 * memory operations
 */


/*
*	Function:
*	      add Win32 malloc() implementation here
*	parameter:
*	     _size - [in] malloc size
*/
III_VOID * III_malloc(III_SIZE _size)
{
	return malloc(_size);
}


/*
*	Function:
*	      add Win32 free() implementation here
*	parameter:
*	     _ptr - [in] ptr point
*/
III_VOID III_free(III_VOID *_ptr)
{
	free(_ptr);
}

/*
 * FILE IOs
 */


/*
*	Function:
*	      add Win32 fopen() implementation here
*	parameter:
*	     _filename - [in] file name
*        _mode - [in] w or r 
*/
III_FILE * III_fopen(const III_CHAR *_filename, const III_CHAR *_mode)
{
	return fopen(_filename, _mode);
}

/*
*	Function:
*	      add Win32 fclose() implementation here
*	parameter:
*	     _fpointer - [in] File point
*/
III_INT III_fclose(III_FILE *_fpointer)
{	
	 return fclose(_fpointer);
}

/*
*	Function:
*	      add Win32 feof() implementation here
*	parameter:
*	     _fpointer - [in] File point
*/
III_INT III_feof(III_FILE *_fpointer)
{	
	return feof((FILE *)_fpointer);
}

/*
*	Function:
*	      add Win32 fseek() implementation here
*	parameter:
*	     _fpointer - [in] File point
*		 _seeksize - [in] offsetsize
*		 _whence - [in] file location
*/
III_INT III_fseek(III_FILE *_fpointer, III_SIZE _seeksize, III_INT _whence)
{
	return fseek(_fpointer, _seeksize, _whence);
}


III_LONG III_ftell(III_FILE *_fpointer)
{
	/*
	 * add Win32 fseek() implementation here
	 */
  return ftell(_fpointer);
}

/*
*	Function:
*	      add Win32 fread() implementation here
*	parameter:
*	     _ptr - [in] I/O file point
*		 _size - [in] _size Byte
*		 _nmemb - [in] Input Byte Data
*		 _fpointer - [in] I/O file
*/
III_SIZE III_fread(void *_ptr, III_SIZE _size, III_SIZE _nmemb, III_FILE *_fpointer)
{
	return fread(_ptr, _size, _nmemb, _fpointer);
}

/*
*	Function:
*	      add Win32 fwrite() implementation here
*	parameter:
*	     _ptr - [in] I/O file point
*		 _size - [in] _size Byte
*		 _nmemb - [in] Input Byte Data
*		 _fpointer - [in] I/O file
*/
III_SIZE III_fwrite(const III_VOID *_ptr, III_SIZE _size, III_SIZE _nmemb, III_FILE *_fpointer)
{
	return fwrite(_ptr, _size, _nmemb, _fpointer);
}


/*
*	Function:
*	      add Win32 fgets() implementation here
*	parameter:
*	     _str - [in] Data buffer
*		 _size - [in] Data offset
*		 _fpointer - [in] I/O File
*/
III_CHAR* III_fgets(III_CHAR *_str, III_SIZE _size, III_FILE *_fpointer)
{
	return fgets(_str, _size, _fpointer);
}

/*
*	Function:
*	      add Win32 fputs() implementation here
*	parameter:
*	     _str - [in] Data buffer
*		 _fpointer - [in] I/O File
*/
III_INT III_fputs(const III_CHAR *_str, III_FILE *_fpointer)
{
	return fputs(_str, _fpointer);
}

