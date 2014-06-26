/*
 * III_win32.c - III Navigation Porting Layer for Win32/CE platforms
 *
 * (c)2007 Institute for Information Industry, All Rights Reserved
 *
 * Maintainer: edwardc@iii.org.tw
 *
 * CVS: $Id: III_win32.c,v 1.3 2007/09/29 03:46:00 edwardc Exp $
 */

#include "prototype.h"
#include "III_win32.h"
/*
 * memory operations
 */

III_VOID *
III_malloc(III_SIZE _size)
{
	/*
	 * add Win32 malloc() implementation here
	 */
  return malloc(_size);
}

III_VOID
III_free(III_VOID *_ptr)
{
	/*
	 * add Win32 free() implementation here
	 */
  free(_ptr);
}

/*
 * FILE IOs
 */

III_FILE *
III_fopen(const III_CHAR *_filename, const III_CHAR *_mode)
{
	/*
	 * add Win32 fopen() implementation here
	 */
  return fopen(_filename, _mode);
}

III_INT
III_fclose(III_FILE *_fpointer)
{
	/*
	 * add Win32 fclose() implementation here
	 */
  return fclose(_fpointer);
}

III_INT
III_feof(III_FILE *_fpointer)
{
	/*
	 * add Win32 feof() implementation here
	 */
  return feof((FILE *)_fpointer);
}

III_INT
III_fseek(III_FILE *_fpointer, III_SIZE _seeksize, III_INT _whence)
{
	/*
	 * add Win32 fseek() implementation here
	 */
  return fseek(_fpointer, _seeksize, _whence);
}

III_LONG
III_ftell(III_FILE *_fpointer)
{
	/*
	 * add Win32 fseek() implementation here
	 */
  return ftell(_fpointer);
}

III_SIZE
III_fread(void *_ptr, III_SIZE _size, III_SIZE _nmemb, III_FILE *_fpointer)
{
	/*
	 * add Win32 fread() implementation here
	 */
  return fread(_ptr, _size, _nmemb, _fpointer);
}

III_SIZE
III_fwrite(const III_VOID *_ptr, III_SIZE _size, III_SIZE _nmemb, III_FILE *_fpointer)
{
	/*
	 * add Win32 fwrite() implementation here
	 */
//  return fwrite(&_ptr, _size, _nmemb, _fpointer);
  return fwrite(_ptr, _size, _nmemb, _fpointer);

}

III_CHAR*
III_fgets(III_CHAR *_str, III_SIZE _size, III_FILE *_fpointer)
{
	/*
	 * add Win32 fgets() implementation here
	 */
  return fgets(_str, _size, _fpointer);
}

III_INT
III_fputs(const III_CHAR *_str, III_FILE *_fpointer)
{
	/*
	 * add Win32 fputs() implementation here
	 */
  return fputs(_str, _fpointer);
}

