
#include "file_system.h"
#include <stdlib.h>
#include <stdarg.h>

typedef struct myFILE {
	char* name;
	char mode[3];
	int fd;
	int ptr;
	int size;
}myFILE;



myFILE* myfopen(const char *pathname, const char *mode);
int myfclose(myFILE *stream);
size_t myfread(void * ptr, size_t size, size_t nmemb, myFILE * stream);
size_t myfwrite(const void * ptr, size_t size, size_t nmemb, myFILE * stream);
int myfseek(myFILE *stream, int offset, int whence);
int myfscanf(myFILE * stream, const char * format, ...);
int myfprintf(myFILE * stream, const char * format, ...);