#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     // 用于fork()、getpid()、sleep() 和 getppid() 函数
#include "_freecplus.h"
int main(){
    CLogFile logfile;

    // 打开日志文件
    logfile.Open("/apps/xsw/tests/gdbfork.log", "w+");

    logfile.Write("begin\n");

    if(fork() != 0){
        logfile.Write("我是父进程：pid = %d, ppid = %d\n", getpid(), getppid());

        int i;
        for(i = 0; i < 10; ++i){
            logfile.Write("i = %d\n", i);
            sleep(1);
        }
        exit(0);
    } else {
        logfile.Write("我是子进程：pid = %d, ppid = %d\n", getpid(), getppid());

        int j;
        for(j = 0; j < 10; ++j){
            logfile.Write("j = %d\n", j);
            sleep(1);
        }
        exit(0);
    }
    return 0;
}
