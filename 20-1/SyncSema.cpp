#include <iostream>
#include <windows.h>
#include <process.h>

#define NUM_THREAD 50
unsigned WINAPI Read(void* arg);
unsigned WINAPI Accu(void* arg);

static int num = 0;
static HANDLE semaOne;
static HANDLE semaTwo;

int main(int argc, char* argv[]){
    HANDLE hThread1, hThread2;

    semaOne = CreateSemaphore(NULL, 0, 1, NULL); // 1번 세마포어를 사용 불가능한 상태로 생성 (순서 제어를 위해)
    semaTwo = CreateSemaphore(NULL, 1, 1, NULL); // 2번 세마포어를 사용해서 동기화를 먼저 하는 스레드가 먼저 동작함

    hThread1 = (HANDLE) _beginthreadex(NULL, 0, Read, NULL, 0, NULL);
    hThread2 = (HANDLE) _beginthreadex(NULL, 0, Accu, NULL, 0, NULL);

    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);

    CloseHandle(semaOne);
    CloseHandle(semaTwo);
    return 0;
}

unsigned WINAPI Read(void* arg){
    for(int i = 0; i < 5; i++){
        WaitForSingleObject(semaTwo, INFINITE); // 먼저, 세마포어 값이 1인 두번째 세마포어를 이용해 먼저 실행한다.
        // WaitForSingleObject 이후에 세마포어의 값이 1 감소한다. -> 바이너리 세마포어에서는 non-singnaled 상태가 된다.
        std::cout << "Input num: ";
        std::cin >> num;
        ReleaseSemaphore(semaOne, 1, NULL); // 첫번째 세마포어 값을 1로 변경해서 반환 가능한 상태로 만든다.
    }

    return 0;
}

unsigned WINAPI Accu(void* arg){
    int sum = 0;
    for(int i = 0; i < 5; i++){
        WaitForSingleObject(semaOne, INFINITE); // 첫번째 세마포어 값이 1이되어 반환 가능한 상태가 되면 반환한다.
        sum += num;
        ReleaseSemaphore(semaTwo, 1, NULL); // 다시 두번째 세마포어 값을 1로 설정하여, 반환 가능한 상태로 설정한다.
    }

    std::cout << "result: " << sum << "\n";
    return 0;
}