// IOCP를 활용한 간단한 채팅 서버 구현

#include <stdlib.h>
#include <process.h>
#include <windows.h>
#include <winsock2.h>
#include <iostream>
#include <vector>
#include <algorithm>

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

    // 서버로 전달된 메세지를 전달할 클라이언트를 저장할 목록과 저장 횟수를 카운트 하기위한 변수
    std::vector<SOCKET>* socks;
    int sendCnt;

    // 임계 지점 보호를 위한 변수
    CRITICAL_SECTION* cs;
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

    std::vector<SOCKET>* socks;

    CRITICAL_SECTION* cs;

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

    socks = new std::vector<SOCKET>();
    cs = new CRITICAL_SECTION();

    InitializeCriticalSection(cs);

    while(true){
        SOCKET hClntSock;
        SOCKADDR_IN clntAddr;
        int addrLen = sizeof(clntAddr);
        hClntSock = accept(hServSock, (SOCKADDR*) &clntAddr, &addrLen);
        handleInfo = (LPPER_HANDLE_DATA) malloc(sizeof(PER_HANDLE_DATA)); // 소켓 구조체 정보를 저장할 포인터 할당
        handleInfo->hClntSock = hClntSock;
        memcpy(&handleInfo->clntSockAddr, &clntAddr, sizeof(SOCKADDR_IN));

        // 생성된 클라이언트 소켓을 벡터에 저장
        EnterCriticalSection(cs);
        socks->push_back(hClntSock);
        LeaveCriticalSection(cs);

        CreateIoCompletionPort((HANDLE) hClntSock, hComPort, (ULONG_PTR) handleInfo, 0); // CP오브젝트와 소켓을 연결

        ioInfo = (LPPER_IO_DATA) malloc(sizeof(PER_IO_DATA));
        ioInfo->overlapped = {};
        ioInfo->wsaBuf.len = BUF_SIZE;
        ioInfo->wsaBuf.buf = ioInfo->buffer;
        ioInfo->rwMode = READ; // IO 모드를 READ 모드로 설정
        // 저장된 벡터의 포인터를 할당
        ioInfo->socks = socks;
        ioInfo->sendCnt = 0;
        // CS 저장
        ioInfo->cs = cs;
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
    CRITICAL_SECTION* cs;
    std::vector<SOCKET>* socks;

    while(true){
        // 할당된 CP 오브젝트에 IO가 완료 되었을때까지 블록킹 상태
        // CP 오브젝트의 IO가 완료되면, 해당 IO의 handleInfo과 ioInfo 값을 반환 
        // (실제로 handleInfo와 ioInfo 값이 GetQueuedCompletionStatus 함수 내부에서 사용되는것이 아니라, 
        // 메인 함수에서 CreateIoCompletionPort로 인해 전달된 두 변수의 값을 다른 쓰레드에서 사용하기 위해 전달하는 역할)
        // 즉, GetQueuedCompletionStatus의 세번째, 네번째 함수는 다른 스레드로 컨텍스트(Context) 정보를 전달하는 통로 역할이다
        GetQueuedCompletionStatus(hComPort, &bytesTrans, (PULONG_PTR) &handleInfo, (LPOVERLAPPED*) &ioInfo, INFINITE);
        sock = handleInfo->hClntSock;
        socks = ioInfo->socks;
        cs = ioInfo->cs;

        if(ioInfo->rwMode == READ){ // 완료된 IO가 READ 모드 일때
            std::cout << "message received!" << std::endl;
            if(bytesTrans == 0){
                // 소켓이 EOF를 전달하면 종료하고 메모리를 할당 해제
                closesocket(sock);
                free(handleInfo);
                free(ioInfo);

                // 닫힌 소켓을 저장 벡터에서 삭제
                EnterCriticalSection(cs);
                socks->erase(std::find(socks->begin(), socks->end(), sock));
                LeaveCriticalSection(cs);
                continue;
            }

            // 데이터를 클라이언트에게 전송하기 위해서 ioInfo를 갱신
            ioInfo->overlapped = {};
            ioInfo->wsaBuf.len = bytesTrans;
            ioInfo->rwMode = WRITE;

            // 할당된 클라이언트 소켓 전체에게 메세지 전송
            EnterCriticalSection(cs);
            for(auto socket : *socks){
                WSASend(socket, &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);
                std::cout << "Sent message to " << socket << std::endl;
            }
            LeaveCriticalSection(cs);

            // 데이터를 클라이언트로 부터 수신받기 위한 ioInfo 할당 (위의 ioInfo는 데이터 전송이 완료되면 할당이 해제된다)
            // 만약 메모리를 재할당 하지않고 바로 재활용 한다면, 전송이 완료되지 않은 데이터가 오염될 가능성이 존재한다. (전송이 비동기적으로 동작하기 때문)
            ioInfo = (LPPER_IO_DATA) malloc(sizeof(PER_IO_DATA));
            ioInfo->overlapped = {};
            ioInfo->wsaBuf.len = BUF_SIZE;
            ioInfo->wsaBuf.buf = ioInfo->buffer;
            ioInfo->rwMode = READ;

            ioInfo->socks = socks;
            ioInfo->sendCnt = 0;
            ioInfo->cs = cs;
            WSARecv(sock, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
        }else{
            // 만약 모든 소켓에게 전송을 완료했다면 메모리를 할당해제
            EnterCriticalSection(cs);
            ioInfo->sendCnt++;
            std::cout << "Send count: " << ioInfo->sendCnt << ", Socks size: " << ioInfo->socks->size() << std::endl;

            if(ioInfo->sendCnt >= ioInfo->socks->size()){
                free(ioInfo);
                std::cout << "message sent to all client!" << std::endl;
            }
            LeaveCriticalSection(cs);
        }
    }

    return 0;
}

void ErrorHandling(const char* message){
    std::cerr << message << " error!" << "\n";
}