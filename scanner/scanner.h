//
// Created by 16645 on 2021/3/26.
//
#ifndef UNTITLED_SCANNER_H
#define UNTITLED_SCANNER_H
#include <stdio.h>
#include <WinSock2.h>
#include <string.h>
#include <time.h>
#pragma comment(lib,"ws2_32")
typedef struct Target{
    char *Ip;
    int StartPort;
    int EndPort;
}Target;
int scan(Target* target);
#endif //UNTITLED_SCANNER_H
