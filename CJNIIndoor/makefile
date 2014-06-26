#this is the first example of make file
#gcc -Wall -D_JNI_IMPLEMENTATION_ -Wl,--kill-at -I"C:\Program Files\Java\jdk1.6.0_16\include" -I"C:\Program Files\Java\jdk1.6.0_16\include\win32" -I"C:\msys\1.0\local\include\json" -L"C:\msys\1.0\local\lib" -ljson -shared -o JNIIdrRoute.dll CJNIIndoor.c
#

all:
	gcc -Wall -D_JNI_IMPLEMENTATION_ -Wl,--kill-at -I"C:\Program Files\Java\jdk1.6.0_16\include" -I"C:\Program Files\Java\jdk1.6.0_16\include\win32" -I"C:\msys\1.0\local\include\json" -shared -o JNIIdrRoute.dll CJNIIndoor.c "C:\msys\1.0\local\lib\libjson.a"