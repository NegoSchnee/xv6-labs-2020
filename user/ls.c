#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)    // 只输出子文件名字，用'/'作为分隔符倒着找出最后的文件名
{
  static char buf[DIRSIZ+1];  // 宏定义，DIRSIZ值为14，规定了文件名最大为14字节 在kernel/fs.h 
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)    // 倒着往前找到'/'，再++，从而找到当前文件名称开始的位置
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));    // 不够长的部分用空格填充
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;    // 定义在 /kernel/fs.h
  struct stat st;    // 定义在 kernel/stat.h      如何找到：linux在xv6-lab-2020文件夹下运行命令   grep -rn "struct stat" .

  if((fd = open(path, 0)) < 0){    // 以只读方式打开文件
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){    // 把文件状态传入结构体st中
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){   // 路径/文件名'0'  加起来不能超过buf大小
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);  // buf存当前目录名
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){    // 遍历读目录文件中的内容，打印里面的文件信息
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);  // 把子文件名续到buf中的 当前目录名/后面
      p[DIRSIZ] = 0;  // 标识结束。后面函数读到这里就会停
      if(stat(buf, &st) < 0){    // 路径名：包含当前路径，来调用stat()函数
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);  // 最后注意关掉文件描述符
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit(0);
  }
  for(i=1; i<argc; i++)   // 遍历参数，ls命令支持列举多个路径下的文件
    ls(argv[i]);
  exit(0);
}
