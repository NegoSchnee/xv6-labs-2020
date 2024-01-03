// lab1 find

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char* argv[]) {
    // 参数不足
    if (argc < 3) {    // 参考rm.c
        fprintf(2, "find输入参数数目过少...\n");
        exit(1);
    }
    // 参数过多
    if (argc > 3) {    // 参考rm.c
        fprintf(2, "find输入参数数目过多...\n");
        exit(1);
    }

    exit(0);
}