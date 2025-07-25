#include <iostream>
#include <windows.h>
#include <process.h>

#define NUM_THREAD 50
unsigned WINAPI threadInc(void* arg);
unsigned WINAPI threadDes(void* arg);

long long num = 0;
CRITICAL_SECTION cs;

int main(int argc, char* argv[]){
    HANDLE tHandles[NUM_THREAD];

    InitializeCriticalSection(&cs);
    for(int i = 0; i < NUM_THREAD; i++){
        if(i%2){
            tHandles[i] = (HANDLE) _beginthreadex(NULL, 0, threadInc, NULL, 0, NULL);
        }else{
            tHandles[i] = (HANDLE) _beginthreadex(NULL, 0, threadDes, NULL, 0, NULL);
        }
    }

    WaitForMultipleObjects(NUM_THREAD, tHandles, true, INFINITE);
    DeleteCriticalSection(&cs);
    std::cout << "result: " << num << "\n";
    return 0;
}

unsigned WINAPI threadInc(void* arg){
    EnterCriticalSection(&cs);
    for(int i = 0; i < 50000000; i++){
        num += 1;
    }
    LeaveCriticalSection(&cs);
    return 0;
}

unsigned WINAPI threadDes(void* arg){
    EnterCriticalSection(&cs);
    for(int i = 0; i < 50000000; i++){
        num -= 1;
    }
    LeaveCriticalSection(&cs);
    return 0;
}