#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "_freecplus.h"

// 把对象定义为全局的
CLogFile logfile;

int x = 0, y = 0;   // x用于线程1，y用于线程2

pthread_t pthid1, pthid2;

// 第一个线程的主函数
void *pth1_main(void *arg);

// 第二个线程的主函数
void *pth2_main(void *arg);

int main(){

    // 打开日志文件
    logfile.Open("/apps/xsw/tests/gdbthread.log", "w+");

    // 创建线程1
    if(pthread_create(&pthid1, NULL, pth1_main, (void*)0) != 0){
        logfile.Write("pthread_create pthid1 failed.\n");
        return -1;
    }

    // 创建线程2
    if(pthread_create(&pthid2, NULL, pth2_main, (void*)0) != 0){
        logfile.Write("pthread_create pthid2 failed.\n");
        return -1;
    }

    logfile.Write("111\n");
    pthread_join(pthid1, NULL);

    logfile.Write("222\n");
    pthread_join(pthid2, NULL);

    logfile.Write("333\n");

    return 0;
}

// 第一个线程的主函数
void *pth1_main(void *arg){
    for(x = 0; x < 10; ++x){
        logfile.Write("x = %d\n", x);
        sleep(1);
    }
    pthread_exit(NULL);
}

// 第二个线程的主函数
void *pth2_main(void *arg){
    for(y = 0; y < 10; ++y){
        logfile.Write("y = %d\n", y);
        sleep(1);
    }
    pthread_exit(NULL);
}
