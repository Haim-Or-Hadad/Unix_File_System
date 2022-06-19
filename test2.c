
#include "Mylibc.h"


int main(){
    mymount("test1.txt", NULL, NULL, NULL, NULL);
    myFILE * p_file1 = myfopen("root/haim/test1", "r");
    
    char buffer[2];
    myfread(buffer, 1, 2, p_file1);
    printf("%s\n",buffer);
    if (strncmp("my", buffer, 4) == 0)
    {
        printf("test paased\n");
    }
    else{
        printf("test failed\n");
    }
    
////////////////////////////
    // strcpy(p_file1->mode ,"+w");
    char* test5 = "test";
    myfwrite(test5, 1, 1, p_file1);
    myfseek(p_file1, -1, SEEK_CUR);
    char* buff = malloc(6);
    myfread(buff, 4, 1, p_file1);
    printf("--%s\n",buff);
    if(strcmp(test5, buff) == 0)
    {
        printf("myfwrite write mode PASSED\n");
    }else
    {
        printf("myfwrite write mode FAILED\n");
    }
    myfclose(p_file1);


    ////////////////////////////////////
    //  myFILE * p_file2 = myfopen("root/haim/test2", "r");
    // myfprintf(p_file2, "%d%c", 1, 'a');
    // // printf("%c\n", )
    // int num1;
    // char char2;

    // myfscanf(p_file2, "%d%c", &num1, &char2);
    // printf("%d\n", num1);
    // printf("%s\n", char2);
}