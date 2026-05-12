// Simple grep.  Only supports ^ . * $ operators.
// 这个程序实现了一个简单的grep工具，用于在文件中搜索匹配正则表达式的行。
// 支持的操作符：
// ^ : 匹配行首
// . : 匹配任意单个字符
// * : 匹配零个或多个前一个字符
// $ : 匹配行尾

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[1024];  // 缓冲区，用于读取文件内容
int match(char*, char*);  // 正则匹配函数声明

// grep函数：从文件描述符fd中读取内容，搜索匹配pattern的行并输出
void
grep(char *pattern, int fd)
{
  int n, m;
  char *p, *q;

  m = 0;  // 缓冲区中已读取的数据长度
  while((n = read(fd, buf+m, sizeof(buf)-m-1)) > 0){  // 循环读取文件内容
    m += n;
    buf[m] = '\0';  // 确保字符串以null结尾
    p = buf;
    while((q = strchr(p, '\n')) != 0){  // 查找换行符，处理每一行
      *q = 0;  // 临时将换行符替换为null，用于字符串比较
      if(match(pattern, p)){  // 检查当前行是否匹配模式
        *q = '\n';  // 恢复换行符
        write(1, p, q+1 - p);  // 输出匹配的行到标准输出
      }
      p = q+1;  // 移动到下一行
    }
    if(m > 0){  // 如果缓冲区有剩余数据，移动到开头
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

// main函数：程序入口，处理命令行参数
int
main(int argc, char *argv[])
{
  int fd, i;
  char *pattern;

  if(argc <= 1){  // 检查参数数量
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];  // 获取搜索模式

  if(argc <= 2){  // 如果没有指定文件，从标准输入读取
    grep(pattern, 0);
    exit(0);
  }

  for(i = 2; i < argc; i++){  // 处理每个指定的文件
    if((fd = open(argv[i], O_RDONLY)) < 0){  // 打开文件
      printf("grep: cannot open %s\n", argv[i]);
      exit(1);
    }
    grep(pattern, fd);  // 在文件中搜索
    close(fd);  // 关闭文件
  }
  exit(0);
}

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9, or
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html
// 正则表达式匹配器，来自Kernighan & Pike的《程序设计实践》第9章

int matchhere(char*, char*);  // 辅助函数声明
int matchstar(int, char*, char*);  // 辅助函数声明

// match函数：检查正则表达式re是否匹配文本text
int
match(char *re, char *text)
{
  if(re[0] == '^')  // 如果以^开头，匹配行首
    return matchhere(re+1, text);
  do{  // must look at empty string  // 必须检查空字符串
    if(matchhere(re, text))
      return 1;
  }while(*text++ != '\0');
  return 0;
}

// matchhere: search for re at beginning of text
// matchhere函数：在text开头搜索re
int matchhere(char *re, char *text)
{
  if(re[0] == '\0')  // 如果正则表达式为空，匹配成功
    return 1;
  if(re[1] == '*')  // 如果下一个字符是*，处理Kleene星号
    return matchstar(re[0], re+2, text);
  if(re[0] == '$' && re[1] == '\0')  // 如果是$结尾，检查text是否结束
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))  // 匹配单个字符或.
    return matchhere(re+1, text+1);
  return 0;
}

// matchstar: search for c*re at beginning of text
// matchstar函数：在text开头搜索c*re
int matchstar(int c, char *re, char *text)
{
  do{  // a * matches zero or more instances
    if(matchhere(re, text))  // 尝试匹配剩余部分
      return 1;
  }while(*text!='\0' && (*text++==c || c=='.'));  // 消耗匹配c或.的字符
  return 0;
}

