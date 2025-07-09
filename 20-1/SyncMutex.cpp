#include <iostream>
#include <windows.h>
#include <process.h>

#define NUM_THREAD 50
unsigned WINAPI threadInc(void* arg);
unsigned WINAPI threadDes(void* arg);

long long num = 0;
HANDLE hMutex;

int main(int argc, char* argv[]){
    HANDLE tHandles[NUM_THREAD];

    // 두번째 인자를 FALSE 로 전달하는건 최초로 생성된 뮤텍스를 signaled (handle의 사용이 종료된 상태) 상태로 생성하기 위해서 
    // (그래야 다른 쓰레드에서 사용 가능한 상태가 됨)
    hMutex = CreateMutex(NULL, FALSE, NULL);

    for(int i = 0; i < NUM_THREAD; i++){
        if(i%2){
            tHandles[i] = (HANDLE) _beginthreadex(NULL, 0, threadInc, NULL, 0, NULL);
        }else{
            tHandles[i] = (HANDLE) _beginthreadex(NULL, 0, threadDes, NULL, 0, NULL);
        }
    }

    WaitForMultipleObjects(NUM_THREAD, tHandles, true, INFINITE);
    CloseHandle(hMutex);
    std::cout << "result: " << num << "\n";
    return 0;
}

unsigned WINAPI threadInc(void* arg){
    WaitForSingleObject(hMutex, INFINITE); // hMutex handle을 non-signaled 상태로 변환 -> 해당 뮤텍스가 사용중인 상태
    for(int i = 0; i < 50000000; i++){
        num += 1;
    }
    ReleaseMutex(hMutex); // hMutex handle을 signaled 상태로 변환 -> 해당 뮤텍스가 사용 가능한 상태
    return 0;
}

unsigned WINAPI threadDes(void* arg){
    WaitForSingleObject(hMutex, INFINITE);
    for(int i = 0; i < 50000000; i++){
        num -= 1;
    }
    ReleaseMutex(hMutex);
    return 0;
}