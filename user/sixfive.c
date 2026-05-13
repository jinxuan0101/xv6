#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"


void solvefile(int fd){
    int a=0;
    int b=0;
    int sum=0;
    char c1=0;
    char c2=0;
    char buf[180];
    sum = read(fd, buf ,sizeof buf);//读取最多180个
    for(int i=0;i<sum;i++)
    {
        if(buf[i]>='0' && buf[i]<='9')
        {
            

            if(buf[i]=='6')
            {
                if(i>1)
                {
                    if(buf[i-1]=='v'&&buf[i-2]=='x')
                    {
                        continue;
                    }
                }
            }
            a=1;
            b=b*10+buf[i]-'0';
        }  
        
        else
        {
            if(a==1)
            {
                if(b%5==0||b%6==0)
                {
                    //fprintf(1, "%c", buf[i]);
                    fprintf(1, "%d", b);
                    fprintf(1, "\n");
                }

            }
            a=0;
            b=0;

        }
    }
    if(sum >= 179) { c1 = buf[178]; c2 = buf[179]; }

    while(1)
    {//该循环内确保每次读满180个字符
        
        sum = read(fd, buf ,sizeof buf);//读取最多180个
        if(sum<180)
        {
            //最后处理不满180个字符的情况
            for(int i=0;i<sum;i++)
            {
                if(buf[i]>='0' && buf[i]<='9')
                {
                    b=b*10+buf[i]-'0';

                    if(buf[i]=='6')
                    {
                        if(i==0)
                        {
                           if(c1=='x'&&c2=='v')
                           {
                               continue;
                           }
                        }
                        else if(i==1)
                        {
                           if(buf[i-1]=='v'&&c2=='x')
                           {
                               continue;
                           }
                        }
                        else
                        {
                           if(buf[i-2]=='x'&&buf[i-1]=='v')
                           {
                               continue;
                           }
                        }
                    }
                
                }
                else
                {
                    if(a==1)
                    {
                       if(b%5==0||b%6==0)
                       {
                           //fprintf(1, "%c", buf[i]);
                           fprintf(1, "%d", b);
                           fprintf(1, "\n");
                       }
                    }
                    a=0;
                    b=0;
                }
            }

            break;
        }
       

        for(int i=0;i<180;i++)
        {
            if(buf[i]>='0' && buf[i]<='9')
            {
                if(buf[i]=='6')
                {
                    if(i==0)
                    {
                        if(c1=='x'&&c2=='v')
                        {
                            continue;
                        }
                    }
                    else if(i==1)
                    {
                        if(buf[i-1]=='v'&&c2=='x')
                        {
                            continue;
                        }
                    }
                    else
                    {
                        if(buf[i-2]=='x'&&buf[i-1]=='v')
                        {
                            continue;
                        }

                    }
                }
                a=1;
                b=b*10+buf[i]-'0';
            }
            else
            {
                if(a==1)
                {
                    if(b%5==0||b%6==0)
                    {
                        //fprintf(1, "%c", buf[i]);
                        fprintf(1, "%d", b);
                        fprintf(1, "\n");
                    }
                }
                a=0;
                b=0;
            }

            // 更新 c1,c2 供下一个 chunk 的边界检测使用
            c1 = buf[178];
            c2 = buf[179];
        }

    }

    // 处理文件末尾遗留的数字（末尾无换行的场景）
    if(a==1 && (b%5==0 || b%6==0))
    {
        fprintf(1, "%d", b);
        fprintf(1, "\n");
    }
}

int main(int argc, char *argv[])
{
   
    if(argc == 0){
        solvefile(0);
    }
    else{
        for(int i=1;i<argc;i++)
        {
            char *filepath=argv[i];
            int fd = open(filepath, O_RDONLY);
            if(fd==-1){
                fprintf(2, "open fail n\n");
            }
            else{
                solvefile(fd);
            }
        }
    }
    
    return 0;
}