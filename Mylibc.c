#include "Mylibc.h"
#include "file_system.h"

int myfclose(myFILE *stream){
myclose(stream->fd);
   free(stream);   
}

size_t myfread(void * _ptr, size_t size, size_t nmemb, myFILE * stream){
    char* buffer = malloc(size*nmemb+1);
    size_t i;
    int o_ptr = stream->_ptr;
    for (i = 0; i < size*nmemb; i++)
    {
        if (stream->_ptr+i>stream->size) {
            break;
        }
        buffer[i] = stream->name[stream->_ptr+i];
        o_ptr++;
    }
    stream->_ptr = o_ptr;
    buffer[size*nmemb] = '\0';
    strncpy(_ptr, buffer, size*nmemb);
    free(buffer);
    return stream->_ptr;
}


myFILE* myfopen(const char *pathname, const char *mode){
    int fd = myopen(pathname,0);
    myFILE* selected_file = (myFILE*)malloc(sizeof(myFILE));
    strcpy(selected_file->mode, mode);
    selected_file->fd = fd;
    if (!strcmp(mode, "a")) { 
        selected_file->_ptr = inodes[fd].size;
    } else {
        selected_file->_ptr = 0;
    }
    selected_file->size = inodes[fd].size;
    selected_file->name = malloc(selected_file->size);
    if (strcmp(mode, "w")) { 
        for (size_t i = 0; i < selected_file->size; i++)
        {
            selected_file->name[i] = read_byte(fd, i);
        }

    } else {
        for (size_t i = 0; i < selected_file->size; i++)
        {
            selected_file->name[i] = ' ';
        }
        selected_file->name[selected_file->size] = '\0';
    }
    myclose(fd);
    return selected_file;
}


size_t myfwrite(const void * _ptr, size_t size, size_t nmemb, myFILE * stream){
    
    char* buff = (char*)_ptr;
    strcpy(stream->mode ,"+w");
    if (stream->_ptr+size*nmemb>stream->size) {
        char* temp = malloc(stream->size+1);
        for (size_t i = 0; i < stream->size; i++)
        {
            temp[i] = stream->name[i];
        }
        free(stream->name);
        stream->name = malloc(stream->_ptr+size*nmemb);
        for (size_t i = 0; i < stream->size; i++)
        {
            stream->name[i] = temp[i];
        }
        free(temp);
    }
    int o_ptr = stream->_ptr;
    for (size_t i = 0; i < size*nmemb; i++)
    {
        stream->name[stream->_ptr+i] = buff[i];
        o_ptr++;
    }
    stream->_ptr = o_ptr;
    return stream->_ptr;
}


int myfscanf(myFILE * stream, const char * format, ...){
    va_list args;
    va_start ( args, format );
    int length = strlen(format);
    strcpy(stream->mode,"r");
    size_t i = 0; 
    size_t j = 0;
    char* buffer; 
    for (i = 0; i < length; i++){
        if (format[i] == '%')
         { 
            if (format[i+1]=='c') 
            {
               *(char *)va_arg( args, char* ) = stream->name[j];
            }
            else if (format[i+1] == 'd') {           
                *(int *)va_arg( args, int* ) = strtol(&stream->name[j], &buffer, 10);
                j+= strlen(buffer) - stream->size;
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
    int len = strlen(format);
    char *currbuffer = malloc(500);
    char *buffer = malloc(strlen(format)+5000);
    int newPos = 0;
    for (size_t i = 0; i < len; i++)
    {
        memset(currbuffer, 0, 500);
        if (format[i] == '%')
        {
            if (format[i+1] == 'c')
            {
                char currvar = va_arg ( arguments, int );
                currbuffer[0] = currvar;                

            }
            if (format[i+1] == 'd')
            {
                int currvar = va_arg ( arguments, int );
                sprintf(currbuffer,"%d",currvar);
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


int myfseek(myFILE *stream, int offset, int whence){
    mylseek(stream->fd, offset, whence);
    return 1;
}