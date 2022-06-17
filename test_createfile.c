#include "file_system.h"


// void Failed_test() {
//     printf("Test Failed! :(\n");
// }

// void Good_Test() {
//     printf("Test Passed! :)\n");
// }

int main() {
//printf("problem\n");
    mymkfs(10000);
    int t1 = myopendir("root/haim");
    int t2 = myopendir("root/haim");
    printf("%d\n" , t1);
    printf("%d\n" , t2);
    // if (t1 == t2) {
    //     Failed_test();
    // } else {
    //     Good_Test();
    // }
}