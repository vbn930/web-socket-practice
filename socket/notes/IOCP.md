# IOCP - Input Output Completion Port
IOCP는 이전 포스트에서 다루었던 Overlapped IO의 완료 처리 방법 중 하나이다. IOCP 방식을 간단하기 설명하자면, 하나의 소켓을 담당하는 Completion Port를 생성하고, 이를 소켓과 연결해서, 해당 소켓의 IO 작업이 완료 되면, Completion Queue에 들어가고, 해당 큐에 담긴 완료된 IO 작업들을 여러개의 워커 스레드들이 처리하는 방식이다.

## IOCP의 구현
IOCP의 구현에서 중요한 포인트는 CompletionKey의 역할에 대해서 이해하는것이다. CompletionKey는 Completion Port를 생성 할 때 함수의 인자로 전달되는 변수인데, 이 변수를 통해 해당 포트에 할당 된 소켓에 대한 정보를 담을수 있다. 워커 스레드에선 완료된 IO 작업을 처리 할 때, CompletionKey에 담긴 소켓의 정보를 바탕으로 처리하게 된다. 또한 IO 함수에 전달된 LPOVERLAPPED를 전달 받아서 완료된 IO에 대한 정보를 파악 할 수 있다.

## IOCP 에코 서버 구현
```cpp
#include <stdlib.h>
#include <process.h>
#include <windows.h>
#include <winsock2.h>
#include <iostream>

#define BUF_SIZE 100
#define READ 3 // 데이터의 전송 모드 저장을 위한 값
#define WRITE 5

typedef struct{ // 소켓 데이터 저장을 위한 구조체
    SOCKET hClntSock;
    SOCKADDR_IN clntSockAddr;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

// 구조체 변수의 주소값은 구조체의 첫번째 변수의 주소값과 일치한다
typedef struct{
    OVERLAPPED overlapped;
    WSABUF wsaBuf;
    char buffer[BUF_SIZE];
    int rwMode;
} PER_IO_DATA, *LPPER_IO_DATA;

DWORD WINAPI EchoThreadMain(LPVOID CompletionPortIO); // IO 처리를 위한 쓰레드에서 실행할 함수

void ErrorHandling(const char* message);

int main(int argc, char* argv[]){
    WSADATA wsaData;
    HANDLE hComPort; // CP 오브젝트의 핸들 (해당 소켓에서 완료된 IO의 정보 저장을 위한 오브젝트)
    SYSTEM_INFO sysInfo; // 시스템의 코어수 확인을 위한 변수
    LPPER_IO_DATA ioInfo;
    LPPER_HANDLE_DATA handleInfo;

    SOCKET hServSock;
    SOCKADDR_IN servAddr;
    int recvBytes, flag = 0;

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        ErrorHandling("WSAStartup()");
    }

    hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0); // CP 오브젝트 생성
    GetSystemInfo(&sysInfo);
    for(int i = 0; i < sysInfo.dwNumberOfProcessors; i++){
        // 시스템의 코어 수 만큼의 쓰레드 생성
        _beginthreadex(NULL, 0 , (unsigned int (*)(void*)) EchoThreadMain, (LPVOID) hComPort, 0, NULL);
    }

    hServSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED); // 논블록킹 소켓 생성
    servAddr = {};
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(atoi(argv[1]));

    bind(hServSock, (SOCKADDR*) &servAddr, sizeof(servAddr));
    listen(hServSock, 5);

    while(true){
        SOCKET hClntSock;
        SOCKADDR_IN clntAddr;
        int addrLen = sizeof(clntAddr);
        hClntSock = accept(hServSock, (SOCKADDR*) &clntAddr, &addrLen);
        handleInfo = (LPPER_HANDLE_DATA) malloc(sizeof(PER_HANDLE_DATA)); // 소켓 구조체 정보를 저장할 포인터 할당
        handleInfo->hClntSock = hClntSock;
        memcpy(&handleInfo->clntSockAddr, &clntAddr, sizeof(SOCKADDR_IN));

        CreateIoCompletionPort((HANDLE) hClntSock, hComPort, (ULONG_PTR) handleInfo, 0); // CP오브젝트와 소켓을 연결

        ioInfo = (LPPER_IO_DATA) malloc(sizeof(PER_IO_DATA));
        ioInfo->overlapped = {};
        ioInfo->wsaBuf.len = BUF_SIZE;
        ioInfo->wsaBuf.buf = ioInfo->buffer;
        ioInfo->rwMode = READ; // IO 모드를 READ 모드로 설정
        WSARecv(handleInfo->hClntSock, &(ioInfo->wsaBuf), 1, (LPDWORD) &recvBytes, (LPDWORD) &flag, &(ioInfo->overlapped), NULL);
    }

    return 0;
}

DWORD WINAPI EchoThreadMain(LPVOID pComPort){
    HANDLE hComPort = (HANDLE) pComPort;
    SOCKET sock;
    DWORD bytesTrans;
    LPPER_HANDLE_DATA handleInfo;
    LPPER_IO_DATA ioInfo;
    DWORD flags = 0;

    while(true){
        // 할당된 CP 오브젝트에 IO가 완료 되었을때까지 블록킹 상태
        // CP 오브젝트의 IO가 완료되면, 해당 IO의 handleInfo과 ioInfo 값을 반환 
        // (실제로 handleInfo와 ioInfo 값이 GetQueuedCompletionStatus 함수 내부에서 사용되는것이 아니라, 
        // 메인 함수에서 CreateIoCompletionPort로 인해 전달된 두 변수의 값을 다른 쓰레드에서 사용하기 위해 전달하는 역할)
        // 즉, GetQueuedCompletionStatus의 세번째, 네번째 함수는 다른 스레드로 컨텍스트(Context) 정보를 전달하는 통로 역할이다
        GetQueuedCompletionStatus(hComPort, &bytesTrans, (PULONG_PTR) &handleInfo, (LPOVERLAPPED*) &ioInfo, INFINITE);
        sock = handleInfo->hClntSock;

        if(ioInfo->rwMode == READ){ // 완료된 IO가 READ 모드 일때
            std::cout << "message received!" << std::endl;
            if(bytesTrans == 0){
                // 소켓이 EOF를 전달하면 종료하고 메모리를 할당 해제
                closesocket(sock);
                free(handleInfo);
                free(ioInfo);
                continue;
            }

            // 데이터를 클라이언트에게 전송하기 위해서 ioInfo를 갱신
            ioInfo->overlapped = {};
            ioInfo->wsaBuf.len = bytesTrans;
            ioInfo->rwMode = WRITE;
            WSASend(sock, &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);

            // 데이터를 클라이언트로 부터 수신받기 위한 ioInfo 할당 (위의 ioInfo는 데이터 전송이 완료되면 할당이 해제된다)
            // 만약 메모리를 재할당 하지않고 바로 재활용 한다면, 전송이 완료되지 않은 데이터가 오염될 가능성이 존재한다. (전송이 비동기적으로 동작하기 때문)
            ioInfo = (LPPER_IO_DATA) malloc(sizeof(PER_IO_DATA));
            ioInfo->overlapped = {};
            ioInfo->wsaBuf.len = BUF_SIZE;
            ioInfo->wsaBuf.buf = ioInfo->buffer;
            ioInfo->rwMode = READ;
            WSARecv(sock, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
        }else{
            std::cout << "message sent!" << std::endl;
            free(ioInfo);
        }
    }

    return 0;
}

void ErrorHandling(const char* message){
    std::cerr << message << " error!" << "\n";
}
```