// lab1 primes

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


// int转char数组
void ita(int n, char *buf) {
    if (n < 10) {
        buf[0] = n + '0';
        buf[1] = '\0';
        return;
    }
    int m = n, cnt = 0;
    while (m) {
        cnt++;
        m /= 10;
    }
    while (n) {
        buf[--cnt] = (n % 10) + '0';
        n /= 10;
    }
    
    // 错误思路，不能一个字节一个字节对应。根据atoi()的逻辑反着写
    // for (int i = 0; i < 4; i++)
    //     buf[i] = (n >> (8 * i)) & 0xff;
}



int main(int argc, char *argv[]) {
    if (argc > 1) {
        fprintf(2, "参数数目过多...\n");
        exit(1);
    }
    int p[35][2];
    char buf[2] = {0};
    int idx = 0, sz = sizeof(buf);
    pipe(p[idx]);
    int pid = fork();

    // 最开始输入所有数字的进程
    if (pid > 0) {
        close(p[idx][0]);
        for (int i = 2; i <= 35; i++) {
            ita(i, buf);
            write(p[idx][1], buf, sz);
        }
        close(p[idx][1]);
        wait(0);
    }
    // 判断素数的进程
    else if (pid == 0) {
        close(p[idx][1]);
        read(p[idx][0], buf, sz);
        int a = atoi(buf), n, children = 0;
        fprintf(1, "prime %d\n", a);
        while (read(p[idx][0], buf, sz) > 0) {
            n = atoi(buf);
            if (n % a) {
                if (!children) {
                    pipe(p[idx + 1]);
                    pid = fork();
                    if (pid == 0) {
                        close(p[++idx][1]);
                        read(p[idx][0], buf, sz);
                        a = atoi(buf);  // 数值是buf转换来的。而不是read()返回结果转化来的，read()返回读到的字节数。
                        fprintf(1, "prime %d\n", a);
                        children = 0;
                        continue;
                    }
                    else if (pid > 0) {
                        children = 1;
                        close(p[idx + 1][0]);
                    }
                    else {
                        fprintf(2, "子进程创建失败...\n");
                        exit(1);
                    }
                }
                ita(n, buf);
                write(p[idx + 1][1], buf, sz);
            }
        }
        // 及时关掉管道写端，使子进程能够结束。
        close(p[idx + 1][1]);
        // 等子进程结束，父进程再结束。否则有孤儿进程、会提前结束祖先进程。
        wait(0);
    }
    else {
        fprintf(2, "子进程创建失败...\n");
        exit(1);
    }

    exit(0);
}