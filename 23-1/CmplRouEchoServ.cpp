#include <stdlib.h>
#include <iostream>
#include <winsock2.h>

#define BUF_SIZE 1024
void CALLBACK ReadCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK WriteCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void ErrorHandling(const char* message);

typedef struct{
    SOCKET hClntSock; // 클라이언트 소켓
    char buf[BUF_SIZE];
    WSABUF wsaBuf; // Overlapped IO 함수를 이용할때, 수신, 발신 데이터를 저장하고 참조하는 함수
} PER_IO_DATA, *LPPER_IO_DATA;

int main(int argc, char* argv[]){
    WSADATA wsaData;
    SOCKET hLisnSock, hRecvSock;
    SOCKADDR_IN lisnAddr, recvAddr;
    LPWSAOVERLAPPED lpOvLp; // overlapped IO의 정보를 저장하기 위한 변수
    DWORD recvBytes; // 전송 혹은 수신된 데이터의 크기를 저장하기 위한 변수
    LPPER_IO_DATA hbInfo; // 소켓과 버퍼 관련 변수가 담긴 구조체
    int mode = 1, recvAddrSz, flagInfo = 0; // mode 변수는 non-blocking 소켓 모드로 설정하기 위한 변수

    if(argc != 2){
        std::cout << "Usage: " << argv[0] << " <PORT>\n";
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        ErrorHandling("WSAStartup()");
    }

    // overlapped 소켓을 생성 하기 위해서는 WSASocket 함수를 이용해 초기화를 진행해줘야한다.
    hLisnSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    ioctlsocket(hLisnSock, FIONBIO, (u_long*) &mode); // 리스닝 소켓을 논블록킹으로 설정

    lisnAddr = SOCKADDR_IN{};
    lisnAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    lisnAddr.sin_family= AF_INET;
    lisnAddr.sin_port = htons(atoi(argv[1]));

    if(bind(hLisnSock, (SOCKADDR*) &lisnAddr, sizeof(lisnAddr)) == SOCKET_ERROR){
        ErrorHandling("bind()");
    }

    if(listen(hLisnSock, 5) == SOCKET_ERROR){
        ErrorHandling("listen()");
    }

    recvAddrSz = sizeof(recvAddr);
    while(true){
        SleepEx(1000, TRUE); // alertalbe wait 상태로 만들기 위함
        // 논블록킹 리스닝 소켓으로 accept 함수를 호출해서 클라이언트 소켓을 할당하면, 클라이언트 소켓도 논블록킹 소켓이됨
        hRecvSock = accept(hLisnSock, (SOCKADDR*) &recvAddr, &recvAddrSz); 
        if(hRecvSock == INVALID_SOCKET){ // 논블록킹 모드라면 할당될 클라이언트가 없을때, accept는 바로 INVALID_SOCKET을 반환한다
            if(WSAGetLastError() == WSAEWOULDBLOCK){ // 만약 에러 메세지가 WSAEWOULDBLOCK이라면, 클라이언트가 없어서 반환됐다는 의미
                continue;
            }else{
                ErrorHandling("accept()");
            }
        }
        puts("Client connected.....");

        // overlapped 변수를 할당 (클라이언트 마다 새로운 overlapped 변수를 할당 해주어야함)
        lpOvLp = (LPOVERLAPPED) malloc(sizeof(WSAOVERLAPPED));
        lpOvLp = {};

        hbInfo = (LPPER_IO_DATA) malloc(sizeof(PER_IO_DATA)); // 버퍼들을 할당
        hbInfo->hClntSock = (DWORD) hRecvSock;
        (hbInfo->wsaBuf).buf = hbInfo->buf;
        (hbInfo->wsaBuf).len = BUF_SIZE;

        // overlapped 변수에 이벤트가 아닌 다른정보를 할당하고 있는데, 
        // 만약 Completion Routine 기반의 방식이라면 이벤트 변수가 사용되지 않기 때문에, 다른 값을 할당해주어도 된다.
        lpOvLp->hEvent = (HANDLE) hbInfo; 
        WSARecv(hRecvSock, &(hbInfo->wsaBuf), 1, &recvBytes, (LPDWORD) &flagInfo, lpOvLp, ReadCompRoutine); // overlapped IO 방식의 recv 함수 호출
    }
    
    closesocket(hRecvSock);
    closesocket(hLisnSock);
    WSACleanup();
    return 0;
}

void CALLBACK ReadCompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags){
    LPPER_IO_DATA hbInfo = (LPPER_IO_DATA) (lpOverlapped->hEvent); // hEvent값에 저장된 버퍼 구조체의 메모리 값 저장
    SOCKET hSock = hbInfo->hClntSock;
    LPWSABUF bufInfo = &(hbInfo->wsaBuf);
    DWORD sentBytes;
    
    if(szRecvBytes == 0){
        closesocket(hSock);
        free(lpOverlapped->hEvent); // 버퍼 구조체의 메모리 할당 해제
        free(lpOverlapped); // overlapped 구조체 메모리 할당 해제
        puts("Client disconnected.....");
    }else{
        bufInfo->len = szRecvBytes;
        WSASend(hSock, bufInfo, 1, &sentBytes, 0, lpOverlapped, WriteCompRoutine); // 수신받은 데이터를 다시 클라이언트에게 전송
    }
}

void CALLBACK WriteCompRoutine(DWORD dwError, DWORD szSendBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags){
    LPPER_IO_DATA hbInfo = (LPPER_IO_DATA) (lpOverlapped->hEvent); // hEvent값에 저장된 버퍼 구조체의 메모리 값 저장
    SOCKET hSock = hbInfo->hClntSock;
    LPWSABUF bufInfo = &(hbInfo->wsaBuf);
    DWORD recvBytes;
    int flagInfo = 0;
    
    WSASend(hSock, bufInfo, 1, &recvBytes, (DWORD) &flagInfo, lpOverlapped, WriteCompRoutine); // 데이터를 클라이언트에게서 다시 수신
}

void ErrorHandling(const char* message){
    std::cerr << message << " error!" << "\n";
}