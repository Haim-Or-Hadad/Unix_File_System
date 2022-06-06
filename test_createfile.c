#include <stdio.h>
#include "file_system.h"

void main(){
    create_fs();
    sync_fs("example1");
    printf("done\n");
}