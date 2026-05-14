#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];  // 静态缓冲区，返回后不会被释放
  char *p;

  // 从末尾向前找最后一个 '/'
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;  // 跳过 '/'，指向文件名首字符

  // 如果名字已超长（>= DIRSIZ），直接返回
  if(strlen(p) >= DIRSIZ)
    return p;
  // 否则拷贝名字并用空格填充对齐
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  buf[sizeof(buf)-1] = '\0';
  return buf;
}

void find(char *path,char *filename)
{
  char buf[512], *p;
  int fd;
  struct dirent de;  // 目录项（inode + 文件名）
  struct stat st;    // 文件元信息

  // 打开目标文件/目录
  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  // 获取文件元信息（类型、inode号、大小等）
  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type)
  {

  case T_DEVICE:  // 设备文件
   

  case T_FILE:    // 普通文件
    if(strcmp(fmtname(path),filename)==0)
      {
        printf("%s\n", path);
      }
    break;

  case T_DIR:  // 目录 —— 逐个读取目录项
    // 检查拼接路径是否超过 buffer 大小
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';          // p 指向 path 末尾的 '/' 之后，用于拼接文件名
    // 逐项读取目录
    while(read(fd, &de, sizeof(de)) == sizeof(de))
    {
      if(de.inum == 0)   // 空的目录项（inode 号为 0）
        continue;
      if(strcmp(de.name,".")==0)
      {
        continue;
      }
      if(strcmp(de.name,"..")==0)
      {
        continue;
      }
      memmove(p, de.name, DIRSIZ);  // 将文件名拼接到路径后
      p[DIRSIZ] = 0;                // 确保末尾有 '\0'
      if(stat(buf, &st) < 0)
      {
        printf("ls: cannot stat %s\n", buf);
        continue;
      }

      if(strcmp(de.name,filename)==0)
      {
        printf("%s\n", buf);
        find(buf,filename);
      }
      else
      {
        find(buf,filename);
      }
      
    }
  }
  close(fd);
}

int main(int argc, char *argv[])
{
 

  // 参数不够时报错
  if(argc < 2){
    fprintf(2, "not enough argv\n");
    exit(0);
  }
  char *path=argv[1];
  char *filename=argv[2];
  find(path,filename);
  
  exit(0);
}