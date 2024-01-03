// lab1 find

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


void find(char* tar, char* path) {
    // 以只读方式打开文件
    char *p = path;
    for (p = path + strlen(path) - 1; p >= path && *p != '/'; p--) {}
    p++;    // 从p开始往后读，是当前文件名
    int fd;
    if ((fd = open(path, 0)) < 0) {    // 读文件时需要传入：从初始目录开始的完整路径，而不是当前文件名
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    struct stat st;    // 结构体定义在 kernel/stat.h
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);  // 退出前，关闭文件描述符
        return;
    }
    
    switch (st.type) {
    case T_FILE:    // 不是目录文件，直接判断匹配并返回
        if (strcmp(tar, p) == 0)
            fprintf(1, "%s\n", path);
        break;
    case T_DIR:    // 是目录文件，递归查找
        if (strcmp(tar, p) == 0)
            fprintf(1, "%s\n", path);
        // 如果冒号后面直接跟声明，必须在前面加个分号，否则报错 a label can only be part of a statement and a declaration is not a statement
        ;struct dirent de;    // 定义在 /kernel/fs.h
        int len0 = strlen(path);
        while(read(fd, &de, sizeof(de)) == sizeof(de)) {    // 遍历子文件
            if (de.inum == 0 || strcmp(de.name, "..") == 0 || strcmp(de.name, ".") == 0)
                continue;
            char* p1 = path + len0;
            *p1++ = '/';
            int lendn = strlen(de.name);
            for (int i = 0; i < lendn; i++, p1++)
                *p1 = de.name[i];
            *p1 = '\0';    // 最后需要加结束符，否则之前写的文件名可能会续上
            find(tar, path);
        }
        break;
    }
    close(fd);  // 最后退出前，关闭文件描述符
}

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
    // 要递归查找的目录
    char path[512] = {0};
    strcpy(path, argv[1]);
    // 目标文件
    char* tar = argv[2];
    find(tar, path);

    exit(0);
}