#include <iostream>
#include <windows.h>
#include <process.h>

unsigned WINAPI ThreadFunc(void* arg);

int main(int argc, char* argv[]){
    HANDLE hThread;
    unsigned threadID;
    int param = 5;

    DWORD wr;

    hThread = (HANDLE) _beginthreadex(NULL, 0, ThreadFunc, (void*) &param, 0, &threadID);
    if(hThread == 0){
        puts("_beginthreadex() error!");
        return -1;
    }

    if((wr = WaitForSingleObject(hThread, INFINITE)) == WAIT_FAILED){
        puts("thread wait error");
        return -1;
    }

    std::cout << "wait result: " << ((wr == WAIT_OBJECT_0) ? "signaled" : "timed-out") << "\n";
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