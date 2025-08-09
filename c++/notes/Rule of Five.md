# 객체 지향 프로그래밍의 규칙
**Rule of Five**는 객체 지향 프로그래밍에서 동적 할당된 메모리와 기타 리소스들을 다루는 클래스가 해당 자원들을 더 안전하게 다룰수 있도록 하는 클래스의 다섯가지 멤버 함수를 의미한다. 원래는 **Rule of Three**였지만, C++ 11 표준에서 move라는 개념이 추가되면서 Rule of Five로 변경 되었다. 

## Rule of Five
Rule of Five를 구성하는 클래스의 멤버 함수들에는, **복사 생성자, 복사 대입 연산자, 소멸자, 이동 생성자, 이동 대입 연산자** 이렇게 총 5가지가 있다.

### 복사 생성자 (Copy Constructor)
```cpp
MyClass(const MyClass& other){
    _data = new Data;
    _data = *other._data;
}

MyClass my = other;
```
복사 생성자는 **MyClass(const MyClass& other)**의 형태로 선언한다. 이 멤버 함수의 내부에서는 인자인 other 클래스 내부 포인터의 값들과 자원들의 깊은 복사가 이루어 진다. 해당 생성자는 **MyClass my = other**과 같이 객체를 생성과 동시에 다른 객체를 복사 할때 사용한다.

### 복사 대입 연산자 (Copy Assignment Constructor)
```cpp
MyClass& operator= (const MyClass& other){
    if(*this != other){
        delete _data;
        _data = new Data;
        _data = *other._data;
    }
    return *this;
}

my = other;
```
복사 대입 연산자는 = 연산자를 통해 객체를 이미 존재하는 객체로 복사 연산을 통해 복사 할 때 사용하는 연산자 이다. 복사 대입 연산자는 **MyClass& operator= (const MyClass& other)**로 생성한다. 이 함수는 other 이라는 다른 객체 참조를 인자로 받아서, 객체로 인자로 받은 객체 값을 복사한다. 그리고 해당 객체의 참조 값을 반환해주어서 연산 체이닝을 가능하게 할수 있다. 이 연산자는 **my = other**의 형식으로 사용 할 수 있다.
### 소멸자 (Destructor)
```cpp
~MyClass(){
    delete _data;
}
```
소멸자는 해당 객체가 범위를 벗어나 소멸하거나 할당 해제를 통해 소멸될때, 내부적으로 할당된 자원들을 안전하게 해제하기 위해서 구현된다. 소멸자에선 객체 내부에 할당된 시스템 리소스들의 해제나 동적으로 할당된 자원들을 해제하여, 객체가 소멸되었을때, 메모리 누수 혹은 남은 시스템 리소스가 누수 되지 않도록 하는 역할을 한다. 소멸자는 **~MyClass()**와 같이 선언하며, 객체가 소멸될 때 자동으로 호출된다.
### 이동 생성자 (Move Constructor)
```cpp
MyClass(MyClass&& other) noexcept {
    _data = other._data;
    other._data = nullptr;
}

MyClass my = other;
```
복사 생성자는 C++11에서 새롭게 추가된 개념인 **이동(Move)**이라는 개념을 사용하는 생성자 이다. 이 이동이라는 개념은 객체의 내부 자원을 복사 하는 것이 아니라, 자원 자체의 소유권을 넘겨줌으로서 효율적인 자원 재활용을 구현 할 수 있다. 객체의 이동은 이동 할 객체를 std::move를 통해 r-value로 변환 한 뒤, 자원을 모두 새로운 객체에게 이동 한 후, 기존의 객체는 nullptr로 처리하는 방식으로 구현된다.

복사 생성자는 **MyClass(MyClass&& other)**의 형식으로 선언한다. 그리고 **MyClass my = std::move(other)**의 형식으로 호출한다.

R-value와 이동 시멘틱에 대해선 다음 링크를 통해 알 수 있다. 

[Rvalue와 Lvalue](https://velog.io/@vbn930/Rvalue%EC%99%80-Lvalue)
### 이동 대입 연산자 (Move Assignment Constructor)
```cpp
MyClass& operator= (MyClass&& other) noexcept {
    if(*this != other){
        delete _data;
        _data = other._data;
        other._data = nullptr;
    }
    return *this;
}

my = other;
```
이동 대입 연산자는 복사 대입 연산자와 마친가지로 작동하지만, 자원의 복사 대신 이동을 수행한다는 차이점이 있다. 이동 대입 연산자는 **MyClass& operator= (MyClass&& other)**와 같이 선언하고, **my = std::move(other)**와 같은 방법으로 호출한다.

이동 대입 연산자에서도 이동 생성자와 마친가지로, 원본 객체 자원의 소유권을 이전 한 뒤, 객체의 자원을 nullptr로 무효화 해야한다. 그렇지 않으면, 원본 객체가 이동을 수행 하고 난 뒤, 객체가 소멸 할 때, 기존에 이동시킨 자원의 포인터도 함께 할당 해제되게 되는데, 이때 이동 받은 객체가 가진 자원의 포인터가 할당 해제된 메모리를 가르키는 상황이 발생 할 수 있다.