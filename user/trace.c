#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int i;
  char *nargv[MAXARG];

  if(argc < 3 || (argv[1][0] < '0' || argv[1][0] > '9')){
    fprintf(2, "Usage: %s mask command\n", argv[0]);
    exit(1);
  }

  if (trace(atoi(argv[1])) < 0) {    // argv[1]中存的是掩码，对应要跟踪的系统调用函数集合
    fprintf(2, "%s: trace failed\n", argv[0]);
    exit(1);
  }
  
  for(i = 2; i < argc && i < MAXARG; i++){  // 从第二个参数开始，copy参数列表argv到新参数列表nargv，后者用于运行后面的exec()
    nargv[i-2] = argv[i];
  }
  exec(nargv[0], nargv);
  exit(0);
}
