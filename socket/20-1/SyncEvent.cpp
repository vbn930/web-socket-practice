#include <iostream>
#include <windows.h>
#include <process.h>

#define STR_LEN 100

unsigned WINAPI NumOfA(void* arg);
unsigned WINAPI NumOfOthers(void* arg);

// static 키워드를 사용하는 이유는 해당 전역 변수가 해당 파일에서만 사용하는걸 명시적으로 표시하기 위해서임
// 다른 파일에서 같은 이름의 전역 변수를 선언해도 문제없음 (static 변수는 해당 파일 내에서만 유효하기 때문)
static char str[STR_LEN];
static HANDLE hEvent;

int main(int argc, char* argv[]){
    HANDLE hThread1, hThread2;
    hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    hThread1 = (HANDLE) _beginthreadex(NULL, 0, NumOfA, NULL, 0, NULL);
    hThread2 = (HANDLE) _beginthreadex(NULL, 0, NumOfOthers, NULL, 0, NULL);

    std::cout << "Input string: ";
    std::cin >> str;
    SetEvent(hEvent); // 두 스레드의 동시 진입을 위해서 event를 signaled 상태로 변경
    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);

    ResetEvent(hEvent); // 스레드 종료 후 event의 상태를 non-signaled로 변경
    CloseHandle(hEvent);
    return 0;
}

unsigned WINAPI NumOfA(void* arg){
    int cnt = 0;

    WaitForSingleObject(hEvent, INFINITE);
    for(int i = 0; str[i] != 0; i++){
        if(str[i] == 'A'){
            cnt++;
        }
    }
    
    std::cout << "Number of A: " << cnt <<"\n";
    return 0;
}

unsigned WINAPI NumOfOthers(void* arg){
    int cnt = 0;

    WaitForSingleObject(hEvent, INFINITE);
    for(int i = 0; str[i] != 0; i++){
        if(str[i] != 'A'){
            cnt++;
        }
    }
    
    std::cout << "Number of others: " << cnt <<"\n";
    return 0;
}