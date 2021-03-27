#include "scanner.h"// 单线程扫描
#include <pthread.h>//多线程

#define Startport 1
#define Endport 10000
#define MaxThread 5

char* IP[MaxThread] = {"192.168.120.130","192.168.1.1","192.168.120.129","192.168.120.131","127.0.0.1",};
//主要c语言没有java方便，就直接指定ip，如要更改设置网段只要加个int to string即可

int main(int argc,char **argv) {
    pthread_t thread[MaxThread];//默认5个线程 要自己加，或者改成calloc
    Target target[MaxThread];

    for(int i = 0 ; i < MaxThread ; i++){
        target[i].StartPort = Startport;
        target[i].EndPort = Endport;
        target[i].Ip = IP[i];
        pthread_create(&thread[i] , NULL, (void *)scan,&(target[i]));
    }

    for (int i = 0; i < MaxThread; i++) {
        void *res;
        pthread_join(thread[i],res);
    }

    return 0;
}

// 单线程
/*int main(int argc,char **argv){
    int ret;
    char* ip_addr;
    ip_addr = (char *)calloc(80,sizeof(char));
    strcpy(ip_addr,"192.168.1.103");
    ret = scan(ip_addr,1,10000);
    if(ret)
        printf("Scan OK\n");
    return 0;
}*/