#include "file_system.h"
#include <string.h>


int main() {
////open same directory//////
printf("//open same directory//\n");
    mymkfs(10000);
    myDIR * file_1 = myopendir("root/haim");
    myDIR * file_2 = myopendir("root/haim");
    myDIR * file_3 = myopendir("root/haim");
    myDIR * file_4 = myopendir("root/haim");
    myDIR * file5 = myopendir("root/ilan");//for test
    if (file_1->fd == file_2->fd) {
        printf("test passed\n");
    } else {
        printf("test failed\n");
    }
    if (file_3->fd == file_4->fd) {
        printf("test passed\n");
    } else {
        printf("test failed\n");
    }
//////////different directory with same file..same fd//////////
    printf("//different directory with same file//\n");
    int dir1 = myopen("root/haim/test1",1);
    int dir2 = myopen("root/ilan/test1",1);
    int dir4 = myopen("root/moshe/test2",1); 
    if(dir1 == dir2)
    {
        printf("test passed\n");
    }else{
        printf("test failed\n");
    }
//////////////my close/////////////////
printf("//close files//\n");
//int dir3 = myopen("root/netzer/test2",1);
dir2 = myclose(dir2);
//dir3 = myfclose(dir3);
if (dir1 != dir2)
    {
    printf("test passed\n");
    } else {
    printf("test failed\n");
    }

    

////////////read,write lseek //////////////
    printf("//read,write lseek//\n");
    char* test = "my_test";  

    int ptr = mywrite(dir1, test, 7);
    char buffer[7];
    mylseek(dir1, -8, SEEK_CUR);
    myread(dir1, buffer, 7);
    printf("%s\n",buffer);

     if(strcmp(buffer, test) == 0)
    {
        printf("read test PASSED\n");
    }else
    {
        printf("read test FAILED\n");
    }

mymount(NULL, "test1.txt" , NULL,NULL,NULL);
}