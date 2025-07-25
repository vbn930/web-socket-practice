# IO 멀티플렉싱 방식
IO 멀티 플렉싱이란, 멀티 프로세스 혹은 멀티 스레드 환경이 아닌 단일 프로세스, 스레드 환경에서 비동기적으로 IO 처리를 하는 방식을 의미한다. 하나의 IO 작업이 끝날때 까지 블록킹 되어 대기 하는 방식이 아니라, 다른 소켓의 IO 작업을 처리하면서, 다른 IO 이벤트가 발생하면, 이를 처리하는 방식이다.

## IO 멀티플렉싱 함수
IO 멀티플렉싱에는 다양한 함수들이 사용된다. select, poll, epoll 등의 함수가 사용되는데, 이중 epoll이 최신 함수이고, select가 가장 오래된 함수이다.

## 동기 vs 비동기
IO 멀티플랙싱을 이해하기 위해서는, 동기와 비동기에 대해서 이해해야한다. 

### 동기적으로 동작하는 함수
동기적으로 작동하는 함수란, 함수의 호출로 인해 시작 된 작업이 종료되는 시점이 함수의 반환 시점과 일치하는 함수이다. 쉽게 말해서, A 함수를 호출하면, 작업 A가 시작된다고 가정하자. A 함수를 호출하면 바로 작업이 시작된다. 이때, 작업이 진행되는 동안 함수 A가 반환을 하지 않고 블록킹된 상태로 대기한다. 그 후 작업이 완료 되면, 그때 함수 A가 반환된다. 이렇게 동작하는 함수를 동기적으로 동작하는 함수라고 한다.

### 비동기적으로 동작하는 함수
비동기적으로 작동하는 함수는 작업의 완료 시점과 함수의 반환 시점이 다르다. 즉, 작업이 완료 될때 까지 함수가 블록킹된 채로 대기하는 것이 아니라, 함수의 호출로 작업이 시작되고, 함수는 바로 반환된다. 이러한 비동기 함수를 사용하면, 작업이 진행되는 중에도 다른 작업을 진행 할 수 있다.

즉, IO 멀티플렉싱은 비동기 IO 함수를 사용하여 동작하는 입출력 처리 방식이다. 멀티 플랙싱 함수 자체가 비동기적으로 동작 하는건 아니지만, IO 입출력 함수를 논블록킹 방식으로 사용하고, IO 이벤트를 기반으로 이벤트가 발생한 경우에만 해당 IO를 처리 한다는 관점에서 IO 멀티플렉싱은 비동기적으로 동작 하는 것이다.