#include <stdio.h>
#include "file_system.h"

void main(){
    mymkfs(10000);
    sync_fs("example1");
    //mymount(NULL, "example1", NULL,NULL,NULL);
    //allocate_file("first");
    print_fs();
    printf("done\n");
}