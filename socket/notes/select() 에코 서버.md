# select() 함수를 이용한 에코 서버
select()가 서버를 구현할 때 어떻게 활용되는지 알 수 있는 에코 서버 예시 코드이다.

```cpp
#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>

#define BUF_SIZE 1024
void error_handling(const char* message);

int main(int argc, char* argv[]){
    int sock_server, sock_clnt;
    char message[BUF_SIZE];
    int str_len = 0;
    sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_sz;

    timeval timeout;
    fd_set reads, cpy_reads;

    int fd_max, str_len, fd_num;


    if(argc != 2){
        std::cout << "Usage: " << argv[0] << " <PORT>\n";
        exit(1);
    }

    sock_server = socket(AF_INET, SOCK_STREAM, 0);
    
    serv_addr = sockaddr_in{};
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_family = AF_INET;

    if(bind(sock_server, (sockaddr*) &serv_addr, sizeof(serv_addr)) == -1){
        error_handling("bind()");
    }

    if(listen(sock_server, 5) == -1){
        error_handling("listen()");
    }

    FD_ZERO(&reads);
    FD_SET(sock_server, &reads);
    fd_max = sock_server;

    while(true){
        cpy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        if((fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout)) == -1){
            break;
        }

        if(fd_num == 0){
            continue;
        }

        for(int i = 0; i < fd_max + 1; i++){
            if(FD_ISSET(i, &cpy_reads)){
                if(i == sock_server){
                    clnt_addr_sz = sizeof(clnt_addr);
                    sock_clnt = accept(sock_server, (sockaddr*) &clnt_addr, &clnt_addr_sz);
                    FD_SET(sock_clnt, &reads);
                    if(fd_max < sock_clnt){
                        fd_max = sock_clnt;
                    }
                    std::cout << "connected client: " << sock_clnt << "\n";
                }else{
                    str_len = read(i , message, BUF_SIZE);
                    if(str_len == 0){
                        FD_CLR(i, &reads);
                        close(i);
                        std::cout << "closed client: "  << i << "\n";
                    }else{
                        write(i, message, str_len);
                    }
                }
            }
        }
    }
    close(sock_server);
    return 0;
}

void error_handling(const char* message){
    std::cerr << message << " error!" << std::endl;
    exit(1);
}
```

## 코드 설명
예제 코드에서 select()를 활용한 방법은, 리스닝 소켓과 클라이언트 소켓을 select 함수의 관찰 대상에 추가하고, 소켓에 입력 이벤트가 발생하면, 이를 처리하는 방식으로 구현하였다. 소켓을 논블록킹으로 생성하여 비동기적으로 서버를 구성한 것은 아니지만, select 함수의 기본적인 사용법을 알수 있도록 코드가 구성 되어있다.

먼저 fd_set에 리스닝 소켓을 관찰 대상에 추가하였는데, 이는 클라이언트의 accept 요청이 리스닝 소켓의 입력 이벤트로 발생하기 때문이다. select 함수를 통해서 reads를 관찰하며, 이벤트가 발생한것이 감지되면, select 함수가 반환되고, reads를 순회하며 FD_ISSET 매크로 함수를 이용해 이벤트가 발생한 소켓을 파악한다. 만약 이벤트가 발생 한 소켓이 리스닝 소켓이라면, accept를 통해 클라이언트 소켓을 생성 후 reads에 추가한다. 만약 이벤트가 발생한 소켓이 리스닝 소켓이 아닌 클라이언트 소켓이라면, 클라이언트의 메세지를 수신 한 뒤, 수신받은 메세지를 다시 클라이언트 측으로 재전송 한다. 만약 클라이언트가 연결을 종료 하였다면, FD_CLR를 통해 종료된 클라이언트를 reads 에서 제외시킨다.

## 코드의 특징
위의 코드에서 눈여겨 봐야 할 요소들이 몇가지 있는데, 먼저 reads 객체를 select를 호출 하기 전 매번 cpy_reads에 복사 하고 있다는 점이다. 이 이유는 전 포스팅에서 다루었는데, 간단하게 다시 설명하자면, select를 통한 관찰이 끝나고 나면, 관찰 대상인 fd_set의 비트가 변경되기 때문에, 원본 fd_set을 보관하여 매번 임시 객체에 전달한 후, 임시 객체를 통해 select로 관찰 해야한다. 

또 다른 요소는 fd_max의 값을 새롭게 추가된 클라이언트 소켓의 값으로 설정하고 있는데, 이는 리눅스 환경에서 파일 디스크립터는 생성된 순서대로 값이 1씩 증가한 번호로 할당 되기 때문이다. 즉, 5번째로 생성된 파일 디스크립터는 값이 5이다 (표준 입출력을 제외하고 말하는 것이다). 만약, 윈도우 환경에서 위와 같은 방식으로 fd_max 값을 설정하게 된다면, 제대로 작동하지 않을 가능성이 크다. 윈도우에선 파일 디스크립터 (윈도우에선 소켓의 핸들이라고 한다)가 생성되는 순서대로 번호가 부여되는것이 아닌, 임의의 값이 부여되기 때문이다. 그래서 윈도우 환경에서는 다른 방식으로 fd_max의 값을 설정해야 한다.