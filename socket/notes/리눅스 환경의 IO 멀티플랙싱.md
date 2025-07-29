# epoll과 select
이전 포스팅에서 select 함수를 이용해서 IO 멀티플랙싱 방식의 서버를 구현하였는데, 이는 어느정도 비효율 적인 측면이 있다. 매번 반목문을 돌며 select 함수를 실행하고, 함수를 호출 할 때 마다 감시할 파일 디스크립터의 목록을 전달해야 했기 때문이다. 또한 select 함수의 반환 이후, 모든 파일 디스크립터를 대상으로 반복문을 진행하여, 이벤트가 발생 한 파일 디스크립터를 찾아야 하기 때문에 속도가 비교적 느렸다.

하지만 이런 단점을 개선한 함수가 바로 epoll이다. epoll은 리눅스 환경에서만 동작하는 개선된 IO 멀티플랙싱 함수이다.

## epoll
epoll은 select와 비교하였을때, 방식은 거의 비슷하지만, 세부적인 사용 방법이 다르다. 우선, epoll은 select 처럼 모든 파일 디스크립터 목록을 반환하지 않고, 이벤트가 발생 한 파일 디스크립터만 모아서 반환한다. 이러한 이점으로, select를 사용한 코드에서 처럼 모든 파일 디스크립터를 대상으로한 반복문으로 이벤트 발생여부를 체크 할 필요가 없다는 것이다. 단순히 이벤트가 발생한 파일 디스크립터들을 순회하며, 확인하면 되는것이다. 

## epoll 함수와 구조체
epoll 함수는 생성, 수정, 이벤트 감지를 위한 함수가 존재힌다.
- epoll_create: epoll 파일 디스크립터 저장소를 생성
- epoll_ctl: epoll 저장소에 파일 디스크립터를 등록 및 삭제
- epoll_wait: epoll 저장소에 저장된 파일 디스크립터들의 이벤트 발생 대기

각 함수의 형태와 함수의 인자로 사용되는 구조체의 형태는 아래와 같다.
```cpp
int epoll_create(int size);

int epoll_ctl(int epfd, int op, int fd, struct epoll_event* event);

int epoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout);

struct epoll_event{
    __uint32_t events;
    epoll_data_t data;
}

typedef union epoll_data{
    void* ptr;
    int fd;
    __uint32_t u32;
    __uint64_t u64;
} epoll_data_t;
```

- **epoll_create**: 인자로 생성될 epoll 인스턴스 (파일 디스크립터 저장소)의 크기를 받고 있는데, 이 인사는 사실 무시되거, 커널 내에서 자동으로 적절하게 epoll 인스턴스의 크기를 조절해서 변형한다. 그렇기 때문에 size 인자는 사실 무의미하다. 해당 함수는 성공시에 epoll 파일 디스크립터를 반환, 실패하면 -1을 반환한다.
- **epoll_ctl**: epoll 파일 디스크립터, 옵션 값, 관찰 대상 파일 디스크립터, 관찰 대상의 이벤트 유형 순서로 인자에 넣는다. 해당 함수의 옵션 값은 EPOLL_CTL_ADD, EPOLL_CTL_DEL, EPOLL_CTL_MOD로, 각각 추가, 삭제, 변경의 용도로 사용된다.
- **epoll_wait**: epoll 인스턴스에 들어있는 파일 디스크립터들의 이벤트를 대기 할 때 사용하는 함수이다. 성공시에 이벤트가 발생한 파일 디스크립터의 수를 반환한다. events 인자를 통해 epoll_event 배열을 받고 있는데, 이는 이벤트가 발생 한 파일 디스크립터의 이벤트 정보들을 저장하기 위한 버퍼로 사용된다. 함수가 반환 된 후, 해당 버퍼에 접근해 파일 디스크립터의 이벤트 종류에 대해서 파악할 수 있다. maxevents는 events 버퍼에 저장 가능한 최대 이벤트 수이다. timeout은 최대 대기 시간이다. -1을 전달하면, 이벤트가 발생 할 때 까지 무한 대기한다.

## epoll vs select
epoll 과 select를 비교하면, 당연히 성능이 더 우수한 epoll을 선택해야 할 것 같지만, epoll의 경우엔 리눅스 환경에서만 지원되지 때문에, 리눅스와 윈도우 환경 모두에서 동작하는 프로그램을 개발하는 경우에는 select를 선택해야한다.