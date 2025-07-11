#include <winsock2.h>
#include <iostream>
#include <stdlib.h>

void ErrorHandling(const char* msg);

int main(int argc, char* argv[]){
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN sockAddr;

    WSABUF dataBuf;
    char msg[] = "Network is computer";
    int sendBytes = 0;

    WSAEVENT eventObj;
    WSAOVERLAPPED overlapped;

    if(argc != 3){
        std::cout << "Usage: " << argv[0] << " <IP> <PORT>\n";
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        ErrorHandling("WSAStartup()");
    }

    hSocket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    sockAddr.sin_addr.s_addr = inet_addr(argv[1]);
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(atoi(argv[2]));

    if(connect(hSocket, (SOCKADDR*) &sockAddr, sizeof(sockAddr)) == SOCKET_ERROR){
        ErrorHandling("connect()");
    }

    eventObj = WSACreateEvent();
    overlapped = {};
    overlapped.hEvent = eventObj;
    dataBuf.len = strlen(msg) + 1;
    dataBuf.buf = msg;

    if(WSASend(hSocket, &dataBuf, 1, (LPDWORD) &sendBytes, 0, &overlapped, NULL) == SOCKET_ERROR){ // 데이터를 보내고 SOCKET_ERROR를 바로 반환
        if(WSAGetLastError() == WSA_IO_PENDING){ // WSA_IO_PENDING 라면 데이터를 전송중인 상태
            puts("Background data sent");
            WSAWaitForMultipleEvents(1, &eventObj, TRUE, WSA_INFINITE, NULL); // 전송이 종료되면 event가 signaled 상태로 변경 (대기중)
            WSAGetOverlappedResult(hSocket, &overlapped, (LPDWORD) &sendBytes, FALSE, NULL); // 전송 결과 확인
        }else{
            ErrorHandling("WSASend()");
        }
    }

    std::cout << "Send data size: " << sendBytes << "\n";
    WSACloseEvent(eventObj);
    WSACleanup();
    closesocket(hSocket);
    return 0;
}

void ErrorHandling(const char* msg){
    std::cerr << msg << " Error!\n";
    exit(1);
}