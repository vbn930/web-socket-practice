# C++ 환경에서 Rvalue 와 Lvalue
C++에서는 값이 가지는 특성에 따라 Rvalue 혹은 Lvalue 로 구분된다.

## Rvalue
Rvalue 란, 특정한 메모리 공간을 차지한 값을 의미한다. 즉, 메모리 공간과 주소를 가지고 있으면서, 해당 메모리 공간에 값을 대입할 수 있는 변수 혹은 객체를 의미한다. Rvalue는 이름과 같이 = 연산에서 오른쪽에 위치 할 수 있다. 
```cpp
int num;
num = 20; // OK
20 = num; // Error
```
위 코드에서 num 변수는 값이 저장되기 위한 메모리 공간의 주소을 가지고 있는데, 이 공간에 = 대입 연산으 통해서 20 이라는 리터럴 상수를 대입하는 것은 가능하지만, 리터럴 상수 20은 = 을 통한 대입 연산이 불가능하다. 여기서 리터럴 상수 20이 Lvalue 이다.

## Lvalue
Lvalue는 Rvalue와 달리 값을 저장하기 위한 메모리 주소를 가지지 않고, 값이 임시로 저장 되기 위한 메모리 공간만을 가지고, 이러한 특성 때문에 = 연산의 오른쪽에 위치 할 수 없고, 반드시 왼쪽에 위치하여야 하기 때문에 Lvalue라는 이름이 붙은것이다.

Lvalue의 종류는 다음과 같다.
- 리터럴 상수
- 일반적인 (x+y)와 같은 연산 결과
- 함수의 반환값이 임시 객체인 경우
- 임시 객체 (e.g std::string("Hello"))

## Lvalue Reference
Lvalue 참조는 Rvalue의 참조가 불가능하고 값이 저장된 메모리에 접근이 가능한 Lvalue의 참조만 가능하다는 것을 의미한다.
```cpp
void Add(int& a){
    a += 5;
}

int main(){
    int num = 10;
    Add(num);
    Add(5); // 불가능
    Add(num + 10); // 불가능

    return 0;
}
```

위 코드를 살펴보면, Add 함수에 num 이라는 Lvalue를 인자로 넣는것은 가능하지만, 5, (num+10)과 같은 Rvalue를 인자로 넣는 것은 불가능하다. 이 이유는, 참조자가 변수의 값이 저장된 메모리의 값을 참조하는 것이기 때문인데, Rvalue의 경우에는 값이 임시로 저장된 메모리 공간만을 가지기 때문에, 이러한 연산이 허용되지 않는다.

하지만 Rvalue를 참조할수 있는 에외가 존재하는데, 이는 const 참조자를 인자로 가지는 것이다. const 참조자는 참조로 전달된 변수의 값을 변경하지 않겠다는 것을 명시적으로 표시하는 것이기 때문에, Rvalue와 Lvalue 모두 대입이 가능하다.

## Rvalue Reference
Rvalue 참조는 Rvalue의 참조 연산을 가능하게 한다. Lvalue 참조에서 Rvalue 연산을 가능하게 하려면, const 참조를 사용 해야 했고, 값의 변경 또한 불가능 했지만, Rvalue 참조를 사용하면, Rvalue의 대입과 값 변경이 가능해진다. 하지만, Rvalue는 어디까지나 임시 객체이기 때문에, 변경된 값은 해당 함수 내부에서만 유효하다.

```cpp
int Add(int&& num){
    num += 5;
    return num;
}

int main(){
    int num = Add(5); // 가능
    int num2 = Add(num); // 불가능

    return 0;
}
```

위 코드를 보면, Type&& 의 형식을 Rvalue 참조를 하고 있다. 여기서 주의 깊게 보아야 할 것은 함수 내부에서 Rvalue의 값을 변경하고 있다는 것이다. 또한, Rvalue 참조를 할때는, Lvalue를 대입하는 것도 불가능하다.

### std::move
std::move 함수를 통해서 Lvalue를 Rvalue로 캐스팅하여 Rvalue에 대입 할 수 있다. 하지만, 이건 변수의 타입을 변경 하는것이 아니라, 단순히 Rvalue로 사용한다고 컴파일러에게 알려주는 것 뿐이다.

### Rvalue 참조의 사용
Rvalue는 객체의 효율적인 이동과 대입에 주로 사용된다.즉, Rvalue는 효율적인 자원 이동 시맨틱 구현에 사용된다.

**이동 시맨틱**이란, 자원의 이동을 진행 할때, 객체의 포인터 자체를 이동 할 객체에게 넘겨주어서, 깊은 복사등의 불필요한 연산을 줄이고, 객체의 소유권 자체를 이전 하므로써 효율적인 객체의 이동을 가능하게 한다.

이동 시맨틱은 주로 클래스 객체의 이동 생성자와 이동 대입 연산자에 사용된다.

객체의 이동 생성자란, 클래스 객체를 생성 할 때, 기존에 존재하는 객체의 자원을 그대로 옮겨 오는것을 의미한다. 이때, 값이 단순히 복사되는것이 아니라, 기존 객체의 자원의 소유권을 새로운 객체로 이전 하고, 기존 객체의 자원은 nullptr 등으로 비우는 것이다.

객체의 이동 생성자는, 이미 존재하는 객체의 자원을 또 다른 객체에 이동하는 것을 의미한다. 이때, 이동 생성자와 다른점은, 새롭게 생성되는 객체에게 자원을 이동하는 것이 아니라, 이미 존재하는 객체에게 기존 객체의 자원을 이동하는것이다. 이동 생성자와 같이 자원을 넘겨 받는 객체에게 기존의 객체가 자원의 소유권을 넘겨주고, 기존의 객체는 비워진다.

```cpp
// 이동 생성자 (Move Constructor)
Object(Object&& other) noexcept;
Object obj = Object(std::move(other));

// 이동 대입 연산자 (Move Assignment Operator)
Object& operator=(Object&& other) noexcept;
obj = other;
```

위 코드는 클래스의 이동 생성자와 이동 대입 연산자의 선언과 사용 예시이다. 위의 선언에서 noexcept 라는 키워드가 각 생성자와 연산자에 붙어있는데, 이는 컴파일러에게 해당 함수는 exception을 발생시키지 않는다는 것을 명시적으로 알려주어서, 컴파일러가 exception handling에 필요한 부가적인 연산을 생략하도록 하여서, 더 빠른 연산이 수행하도록 할 수 있다.