# select()를 이용한 IO 멀티플렉싱
앞선 게시물에 작성했던 것 처럼, select는 리눅스 환경에서 IO 멀티플렉싱을 위한 함수 중 가장 오래된 함수이다. 함수가 오래된 만큼, 사용 하는 방법도 비교적 까다롭다.

## select()
먼저 select 함수는 파일 디스크립터의 세가지 이벤트를 감지 할 수 있다. read, write, exception 이렇게 세가지 이벤트이다.

select 함수는 함수를 호출 할 때, 관찰 할 파일 디스크립터들을 인자로 받아서, 해당 넘겨받은 파일 디스크립터 중 이벤트가 발생한 파일 디스크립터의 비트는 1로 남겨져 있고, 이벤트가 발생하지 않은 파일 디스크립터의 비트는 0으로 바뀌게 된다.

fd_set을 파일 디스크립터의 비트를 저장하는 변수라고 하자. 이때, 초기엔 1-5까지의 파일 디스크립터 비트가 1, 1, 1, 1, 1 이다. 하지만, 만약 select 함수를 통해 1번과 3번의 파일 디스크립터에 이벤트가 감지 되면, fd_set의 비트 값들은 1, 0, 1, 0, 0 으로 바뀌게 된다. 이벤트가 발생하지 않은 파일 디스크립터의 비트값이 모두 0으로 바뀌게 되는 것이다.

## select() 함수의 사용
먼저 select 함수를 사용하기 위해서는 fd_set형 변수에 대해서 알아야 한다. fd_set 변수는 비트들의 배열이라고 생각하면 되는데, 이 변수에 값을 등록하거나 변경 하기 위해서는 다음과 같은 매크로 함수를 사용할 수 있다.

```cpp
// 모든 비트를 0으로 설정한다.
FD_ZERO(fd_set* fdset);

// 전달된 fd 값에 해당하는 비트를 fdset에 등록한다.
// 즉, 해당하는 fd 값의 비트를 1로 변경한다.
FD_SET(int fd, fd_set* fdset); 

// 전달된 fd 값에 해당하는 비트를 fdset에서 삭제한다.
// 즉, 해당하는 fd 값의 비트를 0으로 변경한다.
FD_CLR(int fd, fd_set* fdset); 

// fd 값에 해당하는 비트가 1이면, 양수를 반환한다.
FD_ISSET(int fd, fd_set* fdset); 
```

select 함수의 형태는 다음과 같다.
```cpp
#include <sys/select.h>
int select(int maxfd, fd_set* readset, fd_set* writeset, const struct timeval* timeout);
// -> 타임 아웃 시 0, 성공 시 이벤트가 감지된 fd의 수, 실패시 -1 반환

// 위 함수의 인자 중 하나인 timeval 구조체는, select 함수의 타임 아웃 시간을 설정하기 위한 구조체로, 아래와 같다.
struct timeval{
    long tv_sec; // seconds
    long tv_usec; // microseconds
}
```

## select 함수를 이용 예시
select 함수를 이용해서 사용자의 표준 입력을 감지하는 예시 코드이다.

```cpp
#include <iostream>
#include <sys/select.h>
#include <sys/time.h>

#define BUF_SIZE 100

int main(){
    fd_set reads, cpy_reads;
    char buf[BUF_SIZE];
    int res, str_len;
    timeval time_out;

    FD_ZERO(&reads);
    FD_SET(0, &reads); // 0은 표준 입력의 파일 디스크립터다.

    while(true){
        time_out.seconds = 5;
        time_out.microseconds = 0;

        cpy_reads = reads;
        res = select(1, &cpy_reads, 0, 0, &time_out);
        if(res == -1){
            std::cerr << "select() error" << std::endl;
        }else if(res == 0){
            std::cout << "time-out" << std::endl;
        }else{
            if(FD_ISSET(0, &cpy_reads)){
                str_len = read(0, buf, BUF_SIZE);
                buf[str_len] = 0;
                std::cout << "Message from console: " << buf << std::endl;
            }
        }
    } 
    return 0;
}
```

위 코드에서 주의깊게 봐야 할 점들이 있다. 우선 fd_set을 reads, cpy_reads 이렇게 총 두개를 선언했다는 점이다. 이는 select 함수의 특성과 관련이 있다. select 함수는 전달받은 fd_set의 비트를 변형시킴으로서 이벤트가 발생한 파일 디스크립터를 표시한다. 즉 이벤트를 관찰 할 파일 디스크립터의 원본 정보가 담긴 reads 변수를 select 함수에 넘기게 되면, 관찰이 된 후, reads 함수에 담긴 비트 정보가 달라지게 된다. 그렇기 때문에 원본의 비트 정보는 남긴 채, 복사본을 만들어 비트 정보를 복사 한 후 select에 넘기는 것이 일반적인 사용 방법이다.

또 다른 한가지는 timeval 변수를 반복문 안에서 매번 초기화 하고 있다는 점이다. 이 이유 또한 select 함수의 특징과 관련이 있다. select 함수는 timeval의 포인터를 넘겨받아 사용하기 때문에, 함수 내부에서 사용하는 과정에서 데이터의 변경이 발생한다. 그러므로 항상 select 함수를 사용 전에 timeval 변수를 초기화 해 주어야한다.