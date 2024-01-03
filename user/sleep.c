// lab1 sleep

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    // 参数不足
    if (argc < 2) {    // 参考rm.c
        fprintf(2, "sleep输入参数数目过少...\n");
        exit(1);
    }
    // 参数过多
    if (argc > 2) {    // 参考rm.c
        fprintf(2, "sleep输入参数数目过多...\n");
        exit(1);
    }

    char* s = argv[1];

    // 检查参数格式
    for (int slen = strlen(s); slen > 0; slen--, ++s) {
        if (*s > '9' || *s < '0') {
            fprintf(2, "sleep参数不正确...\n");
            exit(1);
        }
    }

    // 重新把参数赋值，因为之前++s已经到了结尾'\0'处。或者直接传check好的argv[1]
    // s = argv[1];

    int time = atoi(argv[1]);    // 函数在ulib.c中
    sleep(time);    // 用户方对系统调用sleep()，其定义在user.h中，跟这个同名文件不是同一个函数。  内核里的sleep()函数有两个参数？

    exit(0);
}