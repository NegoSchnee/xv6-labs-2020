// lab1 pingpong
// 用两对管道实现pingpong

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc > 1) {
        fprintf(2, "pingpong参数数目过多...\n");
        exit(1);
    }
    int p1[2], p2[2];
    char buf[1];
    pipe(p1);
    pipe(p2);
    int pid = fork();
    if (pid == 0) {
        read(p1[0], buf, 1);
        pid = getpid();
        fprintf(1, "%d: received ping\n", pid);
        // putc(1, buf);
        write(p2[1], buf, 1);
        close(p2[1]);
    }
    else if (pid > 0){
        write(p1[1], buf, 1);
        close(p1[1]);
        read(p2[0], buf, 1);
        pid = getpid();
        fprintf(1, "%d: received pong\n", pid);
    }
    else {
        fprintf(2, "子进程创建失败...\n");
        exit(1);
    }
    exit(0);
}