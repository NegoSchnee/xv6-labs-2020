// lab1 xargs

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    if (argc == 1)  // xargs后面没有指令，直接返回
        exit(0);
    char* newargv[MAXARG];    // 不能直接在argv末尾添加，要重建一个参数数组
    for (int i = 1; i < argc; i++)
        newargv[i - 1] = argv[i];
    int n = 0, mx = 100, cnt = 0;
    char buf[mx];   // 参考《C程序设计语言>5.5，注意定义 char* 和 char[] 的区别
    while ((n = read(0, buf + cnt, 1)) > 0) {
        if (buf[cnt] != '\n') {
            cnt++;
            continue;
        }
        buf[cnt] = '\0';
        if (fork() == 0) {
            newargv[argc - 1] = buf;
            exec(argv[1], newargv);
        }
        else {
            cnt = 0;
            wait(0);
        }
    }
    exit(0);
}
