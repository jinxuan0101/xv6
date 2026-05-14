#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char*
fmtname(char *path)
{
  
  static char *p;

  // 从末尾向前找最后一个 '/'
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;  // 跳过 '/'，指向文件名首字符

  
  return p;
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

void find_exec(char *path,char *filename,char *newargv[],int n)
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
        int pid;
        pid = fork();
        if(pid > 0)
        {
            pid = wait(0);
        }else if(pid == 0)
        {
            newargv[n] = path; // exec要求参数列表以0结尾
            newargv[n+1] = 0;
            exec(newargv[0], newargv);
            exit(0);
        }else 
        {
        printf("fork error\n");
        }


    }
    break;

  case T_DIR:  // 目录 —— 逐个读取目录项
    if(strcmp(fmtname(path),filename)==0)
    {
        printf("%s\n", path);
    }
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

      find_exec(buf,filename,newargv,n);
      
    }
  }
  close(fd);
}

int main(int argc, char *argv[])
{
 

  // 参数不够时报错
  if(argc < 3){
    fprintf(2, "not enough argv\n");
    exit(0);
  }

  if(argc ==3)
  {
    char *path=argv[1];
    char *filename=argv[2];
    find(path,filename);
  }

  else
  {
    if(strcmp("-exec",argv[3])!=0)
    {
        fprintf(2, "wrong argv\n");
        exit(0);
    }
    char *path=argv[1];
    char *filename=argv[2];
    int n = argc-4;
    char *newargv[n+2];
    for(int i=0;i<n;i++)
    {
        newargv[i]=argv[i+4];

    }
    find_exec(path,filename,newargv,n);


  }
  
  exit(0);
}