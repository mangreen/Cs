/*
*	CopyRight : ESTI_資策會新興智慧所
*	Description : 跨平台介面定義
*	Version & Date : v0.3  2010/5/24
*/
#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#define III_PLATFORM_WIN32     0x00000001    // generic Win32/WinCE/WinMobile family
#define III_PLATFORM_LINUX     0x00000010    // generic Linux/FreeBSD/unix/cygwin family
#define III_PLATFORM_GT35      0x00000100    // GT35
#define III_PLATFORM_ECOS      0x00001000    // generic eCos family

#define III_GUI_CONSOLE        0x10000000    // no GUI !
#define III_GUI_GENERIC        0x10000001
#define III_GUI_QTOPIA4PHONE   0x10000010
#define III_GUI_QT4OPENSOURCE  0x10000100
#define III_GUI_GT35           0x10001000
#define III_GUI_GTK1           0x10001001
#define III_GUI_GTK2           0x10001010    // GTK2 !
#define III_GUI_LIBGD          0x20000001    // libgd for offline graph rendering

#if    !defined(_PROTOTYPE_H_)
 #include "prototype.h"
#endif

#if   defined(GUILITE_DLL_EX) || defined(W99702)
  #define III_GUI             III_GUI_GT35
  #define III_PLATFORM        III_PLATFORM_GT35
#elif  defined(_WIN32)            /* generic Win32/WinCE platform */
  #define III_PLATFORM        III_PLATFORM_WIN32
  #if  defined(_QT4OS)
    #define III_GUI           III_GUI_QT4OPENSOURCE
  #else
    #define III_GUI           III_GUI_CONSOLE
  #endif
#else                            /* generic Linux/GNU platform */
  #define III_PLATFORM        III_PLATFORM_LINUX
  #if   defined(_GTK2)
    #define III_GUI           III_GUI_GTK2
  #elif defined(_LIBGD)
    #define	III_GUI           III_GUI_LIBGD
  #else
    #define III_GUI           III_GUI_CONSOLE
  #endif
#endif

#if !defined(III_GUI)
  #define III_GUI             III_GUI_CONSOLE
#endif

/* -- platform -- */
#if   III_PLATFORM == III_PLATFORM_WIN32
  #include "III_win32.h"
#elif III_PLATFORM == III_PLATFORM_GT35
  #include "III_gt35.h"
  #ifndef   diag_printf
    #define diag_printf       printf
  #endif
#elif III_PLATFORM == III_PLATFORM_LINUX
  #include "III_linux.h"
#else
  #error "no specific III_PLATFORM found!"
#endif

/* -- GUI      -- */
#if   III_GUI != III_GUI_CONSOLE
  #include "GUI_prototype.h"
  #include "GUI_color.h"
  #if   III_GUI == III_GUI_GT35
    //#include "GUI_gt35.c"         /* may illegal usage on various platforms */
  #elif III_GUI == III_GUI_GTK2
    #include <gtk/gtk.h>
    #include <gdk/gdk.h>
  #elif III_GUI == III_GUI_LIBGD
    #include <gd.h>
  #else
    #error   "not a vaild III_GUI value assigned!"
  #endif
#endif

#if !defined(MAPDATAPATH)         /* may assign via compiler pre-compiler definitions */
 #ifdef W99702
   #define MAPDATAPATH "..\\ShareFolder\\MapData\\"
 #else
   //#define MAPDATAPATH "./MapData/"

 #endif
#endif

#endif/*_PLATFORM_H_*/
