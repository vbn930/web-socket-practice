#include <iostream>
#include <windows.h>
#include <process.h>

#define STR_LEN 100

unsigned WINAPI NumOfA(void* arg);
unsigned WINAPI NumOfOthers(void* arg);

// static 키워드를 사용하는 이유는 해당 전역 변수가 해당 파일에서만 사용하는걸 명시적으로 표시하기 위해서임
// 다른 파일에서 같은 이름의 전역 변수를 선언해도 문제없음 (static 변수는 해당 파일 내에서만 유효하기 때문)
static char str[STR_LEN];
static HANDLE hSema;

int main(int argc, char* argv[]){
    HANDLE hThread1, hThread2;
    hSema = CreateSemaphore(NULL, 0, 2, NULL); // 세마포어의 최댓값을 2로 설정하여, 최대 두개의 스레드에서 동시에 접근 가능하도록 설정
    hThread1 = (HANDLE) _beginthreadex(NULL, 0, NumOfA, NULL, 0, NULL);
    hThread2 = (HANDLE) _beginthreadex(NULL, 0, NumOfOthers, NULL, 0, NULL);

    std::cout << "Input string: ";
    std::cin >> str;
    ReleaseSemaphore(hSema, 2, NULL); // 두 스레드의 동시 진입을 위해서 세마포어의 값을 2로 변경 (signaled 상태가 됨)
    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);

    CloseHandle(hSema);
    return 0;
}

unsigned WINAPI NumOfA(void* arg){
    int cnt = 0;

    //세마포어는 auto-reset 모드의 동기화이기 때문에 WaitForSingleObject 호출 이후 세마포어 값이 자동으로 1 감소
    WaitForSingleObject(hSema, INFINITE);
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

    WaitForSingleObject(hSema, INFINITE);
    for(int i = 0; str[i] != 0; i++){
        if(str[i] != 'A'){
            cnt++;
        }
    }
    
    std::cout << "Number of others: " << cnt <<"\n";
    return 0;
}