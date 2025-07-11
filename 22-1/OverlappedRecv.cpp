#include <winsock2.h>
#include <iostream>
#include <stdlib.h>

#define BUF_SIZE 100

void ErrorHandling(const char* msg);

int main(int argc, char* argv[]){
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servSockAddr, clntSockAddr;

    int clntSockAddrSz;
    int recvBytes = 0, flags = 0;

    WSABUF dataBuf;
    char msg[BUF_SIZE];

    WSAEVENT eventObj;
    WSAOVERLAPPED overlapped;

    if(argc != 2){
        std::cout << "Usage: " << argv[0] << " <PORT>\n";
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        ErrorHandling("WSAStartup()");
    }

    hServSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    servSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servSockAddr.sin_family = AF_INET;
    servSockAddr.sin_port = htons(atoi(argv[1]));

    if(bind(hServSock, (SOCKADDR*) &servSockAddr, sizeof(servSockAddr)) == SOCKET_ERROR){
        ErrorHandling("bind()");
    }

    if(listen(hServSock, 5) == SOCKET_ERROR){
        ErrorHandling("listen()");
    }

    clntSockAddrSz = sizeof(clntSockAddr);
    hClntSock = accept(hServSock, (SOCKADDR*) &clntSockAddr, &clntSockAddrSz);

    eventObj = WSACreateEvent();
    overlapped = {};
    overlapped.hEvent = eventObj;
    dataBuf.len = BUF_SIZE;
    dataBuf.buf = msg;

    if(WSARecv(hClntSock, &dataBuf, 1, (LPDWORD) &recvBytes, (LPDWORD) &flags, &overlapped, NULL) == SOCKET_ERROR){ // 데이터를 보내고 SOCKET_ERROR를 바로 반환
        if(WSAGetLastError() == WSA_IO_PENDING){ // WSA_IO_PENDING 라면 데이터를 전송중인 상태
            puts("Background data received");
            WSAWaitForMultipleEvents(1, &eventObj, TRUE, WSA_INFINITE, NULL); // 전송이 종료되면 event가 signaled 상태로 변경 (대기중)
            WSAGetOverlappedResult(hClntSock, &overlapped, (LPDWORD) &recvBytes, FALSE, NULL); // 전송 결과 확인
        }else{
            ErrorHandling("WSASend()");
        }
    }

    std::cout << "Received data size: " << recvBytes << "\n";
    WSACloseEvent(eventObj);
    WSACleanup();
    closesocket(hClntSock);
    closesocket(hServSock);
    return 0;
}

void ErrorHandling(const char* msg){
    std::cerr << msg << " Error!\n";
    exit(1);
}