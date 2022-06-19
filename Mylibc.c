#include "Mylibc.h"
#include "file_system.h"


myFILE* myfopen(const char *pathname, const char *mode){

    if (!mode) {
        return -1; 
    }
    int fd = myopen(pathname,0);
    myFILE* currfile = (myFILE*)malloc(sizeof(myFILE));
    strcpy(currfile->mode, mode);
    currfile->fd = fd;
    currfile->size = inodes[fd].size;
    currfile->name = malloc(currfile->size);
    if (!strcmp(mode, "a")) { // set the pointer to the end of the file
        currfile->ptr = inodes[fd].size;
    } else {
        currfile->ptr = 0;
    }
    if (!strcmp(mode, "w")) { 
        for (size_t i = 0; i < currfile->size; i++)
        {
            currfile->name[i] = ' ';
        }
        currfile->name[currfile->size] = '\0';
    } else {
        for (size_t i = 0; i < currfile->size; i++)
        {
            currfile->name[i] = read_byte(fd, i);
        }
    }
    myclose(fd);
    return currfile;
}
int myfclose(myFILE *stream){
myclose(stream->fd);
   free(stream);   
   return 1;
}

size_t myfread(void * ptr, size_t size, size_t nmemb, myFILE * stream){
    char* buffer = malloc(size*nmemb+1);
    size_t i;
    int optr = stream->ptr;
    for (i = 0; i < size*nmemb; i++)
    {
        if (stream->ptr+i>stream->size) {
            break;
        }
        buffer[i] = stream->name[stream->ptr+i];
        optr++;
    }
    stream->ptr = optr;
    buffer[size*nmemb] = '\0';
    strncpy(ptr, buffer, size*nmemb);
    free(buffer);
    return stream->ptr;
}


size_t myfwrite(const void * ptr, size_t size, size_t nmemb, myFILE * stream){
    strcpy(stream->mode ,"+w");
    char* buffer = (char*)ptr;
    if (stream->ptr+size*nmemb>stream->size) {
        char* temp = malloc(stream->size+1);
        for (size_t i = 0; i < stream->size; i++)
        {
            temp[i] = stream->name[i];
        }
        free(stream->name);
        stream->name = malloc(stream->ptr+size*nmemb);
        for (size_t i = 0; i < stream->size; i++)
        {
            stream->name[i] = temp[i];
        }
        free(temp);
    }
    int optr = stream->ptr;
    for (size_t i = 0; i < size*nmemb; i++)
    {
        stream->name[stream->ptr+i] = buffer[i];
        optr++;
    }
    stream->ptr = optr;
    return stream->ptr;
}

int myfseek(myFILE *stream, int offset, int whence){
    mylseek(stream->fd, offset, whence);
    return 1;
}

int myfscanf(myFILE * stream, const char * format, ...){
    va_list args;
    va_start ( args, format );
    char* buffer; 
    int length = strlen(format);
    strcpy(stream->mode,"r");
    size_t i = 0; 
    size_t j = 0;
    for (i = 0; i < length; i++){
        if (format[i] == '%')
         { 
            if (format[i+1]=='d') 
            {
                *(int *)va_arg( args, int* ) = strtol(&stream->name[j], &buffer, 10);
                j+= strlen(buffer) - stream->size;
            }
            else if (format[i+1] == 'c') {
               *(char *)va_arg( args, char* ) = stream->name[j];
                }
                i++;
         }
         j++;
        }
    return 2;
         }



int myfprintf(myFILE * stream, const char * format, ...){
    va_list arguments; 
     size_t j= 0;                    
    /* Initializing arguments to store all values after num */
    va_start ( arguments, format );
    char *currbuffer = malloc(500);
    char *buffer = malloc(strlen(format)+5000);
    int len = strlen(format);
    int newPos = 0;
    for (size_t i = 0; i < len; i++)
    {
        memset(currbuffer, 0, 500);
        if (format[i] == '%')
        {
            if (format[i+1] == 'd')
            {
                int currvar = va_arg ( arguments, int );
                sprintf(currbuffer,"%d",currvar);
            }
            if (format[i+1] == 'c')
            {
                char currvar = va_arg ( arguments, int );
                currbuffer[0] = currvar;
            }
        }
        i++;
    
    for (size_t k = 0; k < strlen(currbuffer); k++)
        {
            buffer[j] = currbuffer[k];
            j++;
        }
    }
    buffer[j] = '\0';
    myfwrite(buffer, strlen(buffer), 1, stream);
    return 1; 
    }
