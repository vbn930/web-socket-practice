#include <iostream>
#include <windows.h>
#include <process.h>

unsigned WINAPI ThreadFunc(void* arg);

int main(int argc, char* argv[]){
    HANDLE hThread;
    unsigned threadID;
    int param = 5;

    hThread = (HANDLE) _beginthreadex(NULL, 0, ThreadFunc, (void*) &param, 0, &threadID);
    if(hThread == 0){
        puts("_beginthreadex() error!");
        return -1;
    }

    Sleep(3000);
    puts("end of main");
    return 0;
}

unsigned WINAPI ThreadFunc(void* arg){
    int cnt = *((int*) arg);
    for(int i = 0; i < cnt; i++){
        Sleep(1000);
        puts("running thread");
    }

    return 0;
}