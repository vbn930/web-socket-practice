#include <iostream>
#include <string>
#include <winsock2.h>

#define BUF_SIZE 1024
void ErrorHandling(const char* message);

int main(int argc, char* argv[]){
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAddr, clntAddr;
    int clntAddrSize;
    char message[BUF_SIZE];
    int strLen = 0, fdNum;

    TIMEVAL timeout;
    fd_set reads, cpyReads;

    if(argc != 2){
        std::cout << "Usage: " << argv[0] << " <PORT>\n";
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        ErrorHandling("WSAStartup()");
    }

    hServSock = socket(AF_INET, SOCK_STREAM, 0);
    if(hServSock == INVALID_SOCKET){
        ErrorHandling("socket()");
    }

    servAddr = SOCKADDR_IN{};
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(atoi(argv[1]));

    if(bind(hServSock, (SOCKADDR*) &servAddr, sizeof(servAddr)) == SOCKET_ERROR){
        ErrorHandling("bind()");
    }

    if(listen(hServSock, 5) == SOCKET_ERROR){
        ErrorHandling("listen()");
    }

    clntAddrSize = sizeof(clntAddr);

    FD_ZERO(&reads);
    FD_SET(hServSock, &reads);

    while(true){
        cpyReads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        if((fdNum = select(0, &cpyReads, 0, 0, &timeout)) == SOCKET_ERROR){
            break;
        }

        if(fdNum == 0){
            continue;
        }

        for(int i = 0; i < reads.fd_count; i++){
            if(FD_ISSET(reads.fd_array[i], &cpyReads)){
                if(reads.fd_array[i] == hServSock){
                    clntAddrSize = sizeof(clntAddr);
                    hClntSock = accept(hServSock, (SOCKADDR*) &clntAddr, &clntAddrSize);
                    FD_SET(hClntSock, &reads);
                    std::cout << "connected client: " << hClntSock << "\n";
                }else{
                    strLen = recv(reads.fd_array[i], message, BUF_SIZE-1, 0);
                    if(strLen == 0){
                        FD_CLR(reads.fd_array[i], &reads);
                        closesocket(cpyReads.fd_array[i]);
                        std::cout << "closed clinet: " << cpyReads.fd_array[i] << "\n";
                    }else{
                        send(reads.fd_array[i], message, strLen, 0);
                    }
                }
            }
        }
    }

    closesocket(hServSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(const char* message){
    std::cerr << message << " error!" << std::endl;
    exit(1);
}