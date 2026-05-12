
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"


int main(int argc, char *argv[])
{
   int slpptime;
  
    if(argc <= 1){  // 检查参数数量
    fprintf(2, "need sleep time\n");
    exit(1);
    }
    slpptime = atoi(argv[0]);  // 获取睡眠时间

    if(slpptime < 0){  // 检查睡眠时间是否合法
        fprintf(2, "usage: sleep n\n");
        exit(1);    
    }

    pause(slpptime);  // 调用pause系统调用，睡眠指定的时间
  exit(0);
}