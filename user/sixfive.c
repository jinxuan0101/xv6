#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "user/ulib.c"

void solvefile(int fd){
    int a= 1;
    int b=0;
    while(a==1){
        char buf[180];
        int sum = read(fd, buf ,sizeof buf);//读取最多180个
        for(int i=0;i<180;i++){
            if(buf[i]=='/'||buf[i]=='-'||buf[i]=='\'|| buf[i]=='')
        }

        if(sum<180){
            a=0;
        }
    }
    

}

int main(int argc, char *argv[])
{
    char buf[180];
    if(argc == 0){
        solvefile(0);
    }
    else{
        for(int i=0;i<argc;i++)
        {
            char *filepath=argv[i];
            int fd = open( *filepath, O_RDONLY);
            if(fd==-1){
                fprintf(2, "open fail n\n");
            }
            else{
                solvefile(fd);
            }
        }
    }
    open("input.txt", O_RDONLY); 
    
    
}