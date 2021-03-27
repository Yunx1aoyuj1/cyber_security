//
// Created by 16645 on 2021/3/26.
//
#include "scanner.h"

int scan(Target* target){
    //clock_t StartTime,EndTime;
    //float CostTime;
    TIMEVAL TimeOut;
    FD_SET mask;
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;
    int CurrPort;
    int ret;
    unsigned long mode=1;  //ioctlsocket函数的最后一个参数
    WSAStartup(MAKEWORD(2,2),&wsa);
    TimeOut.tv_sec = 0;
    TimeOut.tv_usec = 50;  //超时为50ms
    FD_ZERO(&mask);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(target -> Ip);
    //StartTime = clock();
    for(CurrPort = target -> StartPort ; CurrPort <= target -> EndPort ; CurrPort ++ )    {
        s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        FD_SET(s,&mask);
        ioctlsocket(s,FIONBIO,&mode);  //设置为非阻塞模式
        server.sin_port = htons(CurrPort);
        connect(s,(struct sockaddr *)&server,sizeof(server));
        ret = select(0,NULL,&mask,NULL,&TimeOut);  //查询可写入状态
        if(ret == 0 || ret == -1){
            closesocket(s);
        }
                else{
            printf("%s:%d\n",target -> Ip,CurrPort);
            closesocket(s);
        }
    }
    //EndTime = clock();
    //CostTime = (float)(EndTime-StartTime) / CLOCKS_PER_SEC;
    //printf("Cost time:%f second\n",CostTime);
    WSACleanup();
    return 1;
}
