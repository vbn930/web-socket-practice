# vector의 핵심 기능들 구현
STL vector의 핵심 기능들만 직접 구현 해 보자. 먼저 구현 할 함수들은 push_back, resize, reserve 등의 가장 기초적인 함수들이다.

## 멤버 변수
```cpp
private:
    T* _data;
    int _size;
    int _capacity;
```
멤버 변수는 총 3가지로, 데이터를 저장하는 T* 타입 포인터 _data, 저장 된 원소의 개수를 저장할 _size 그리고 마지막으로 vector의 가용 용량을 저장 할 _capacity 변수이다.

## 생성자
```cpp
Vector(): _data(nullptr), _size(0), _capacity(0){
}

Vector(int size): _data(nullptr), _size(size), _capacity(0){
    reserve(size);
}

Vector(int size, const T& val): _data(nullptr), _size(size), _capacity(0){
    reserve(size);
    for(int i = 0; i < size; i++){
        _data[i] = val;
    }
}
```
먼저 생성자 구현부를 살펴보자. 간단하게 기본 생성자, 초기 크기를 받는 생성자, 초기 크기와 초기 값을 받는 생성자로 총 3가지의 생성자를 구현 해주었다.

기본 생성자에서는 _data를 nullptr로, _size와 _capacity 값을 0으로 초기화 해준다.

초기 크기를 받는 생성자에선 reserve 함수를 통해 size 값 만큼 capacity를 지정하여 vector를 생성한다. 그리고 size 값 만큼의 원소가 기본 생성자로 초기화 된 채로 들어있다.

초기 크기와 초기 값을 받는 생성자에선 reserve를 통해 size 값만큼 용량을 넓힌 뒤, 초기값인 val로 내부 원소들을 모두 초기화 해 주었다.

## resize & reserve
```cpp
void resize(int size){
    _size = size;
}

void reserve(int capacity){
    if(capacity <= _capacity){
        return;
    }
    
    _capacity = capacity;

    T* newData = new T[capacity];

    if(_data != nullptr){
        for(int i = 0; i < _size; ++i){
            newData[i] = _data[i];
        }
        delete[] _data;
    }

    _data = newData;
}
```

resize 함수에선 단순히 _size의 값만 지정 해준 만큼 초기화 해준다.

reserve 함수는 resize 함수에 비해서 복잡한데, 이는 새로운 capacity 만큼의 메모리를 새로 할당하고, 기존 데이터를 새로운 데이터에 복사하는 작업이 추가로 이루어지기 때문이다.

## push_back
```cpp
void push_back(const T& val){
    if(_data == nullptr){
        reserve(1);
    }

    if(_size == _capacity){
        reserve(_capacity * 2);
    }

    _data[_size] = val;
    _size++;
}
```
push_back 함수에선 val 객체를 받아서 _data의 마지막 위치에 할당해준다. 만약, vector의 capacity가 가득 찼다면, reserve 함수를 통해 기존 capacity의 두배의 용량으로 저장 공간을 증설해준다.

## operator
```cpp
T& operator [](int idx){
    return _data[idx];
}
```
배열처럼 []을 이용한 원소 접근, 즉, 임의 접근을 구현하기 위해서 [] 연산자 오버로딩도 해준다.

## iterator 클래스 구현
```cpp
template <typename T>
class Iterator{
public:
    Iterator() : _ptr(nullptr){}
    Iterator(T* ptr) : _ptr(ptr) {}
    Iterator& operator ++(){
        _ptr++;
        return (*this);
    }

    Iterator operator ++(int){
        Iterator temp = (*this);
        _ptr++;
        return temp;
    }

    Iterator operator +(int idx){
        Iterator temp = (*this);
        temp._ptr += idx;
        return temp;
    }

    T& operator *(){
        return (*_ptr);
    }

    bool operator ==(const Iterator& other){
        return (_ptr == other._ptr);
    }

    bool operator !=(const Iterator& other){
        return (_ptr != other._ptr);
    }

public:
    T* _ptr;
};
```

iterator 클래스에선 대부분 연산자 오버로딩을 구현 해주었다. 먼저 기초적인 증가 연산자들과 비교 연산자를 오버로딩 해주었다. 또한 포인터와 같이 * 기호로 가르키고있는 객체에 접근하기 위해서 해당 연산자도 오버로딩 해 주었다.

STL의 vector iterator에선 원소를 가르키는 포인터 말고도 반복자가 가르키는 원소를 가진 컨테이너 정보 등을 가진 멤버 변수들이 존재하지만, 간단한 구현만을 위서 포인터만 멤버 변수로 정의해 주었다.

## vector의 iterator 함수와 typedef
```cpp
public:
    typedef Iterator<T> iterator; 

    iterator begin(){
        if(_data == nullptr){
            return nullptr;
        }

        Iterator it = Iterator(&_data[0]);
        return it;
    }

    iterator end(){
        if(_data == nullptr){
            return nullptr;
        }

        return begin() + _size;
    }
```
vector를 통해 반복자를 정의 할 수 있도록 typedef를 통해 타입을 추가 해주었고, vector의 첫 원소와 끝 원소의 다음을 가르키는 반복자를 반환하는 begin, end 함수를 구현 해주었다.

## 코드와 예시
```cpp
#include <iostream>
#include <vector>

template <typename T>
class Iterator{
public:
    Iterator() : _ptr(nullptr){}
    Iterator(T* ptr) : _ptr(ptr) {}
    Iterator& operator ++(){
        _ptr++;
        return (*this);
    }

    Iterator operator ++(int){
        Iterator temp = (*this);
        _ptr++;
        return temp;
    }

    Iterator operator +(int idx){
        Iterator temp = (*this);
        temp._ptr += idx;
        return temp;
    }

    T& operator *(){
        return (*_ptr);
    }

    bool operator ==(const Iterator& other){
        return (_ptr == other._ptr);
    }

    bool operator !=(const Iterator& other){
        return (_ptr != other._ptr);
    }

public:
    T* _ptr;
};

template <typename T>
class Vector{
public:
    Vector(): _data(nullptr), _size(0), _capacity(0){
    }
    
    Vector(int size): _data(nullptr), _size(size), _capacity(0){
        reserve(size);
    }

    Vector(int size, const T& val): _data(nullptr), _size(size), _capacity(0){
        reserve(size);
        for(int i = 0; i < size; i++){
            _data[i] = val;
        }
    }

    void push_back(T val){
        if(_data == nullptr){
            reserve(1);
        }

        if(_size == _capacity){
            reserve(_capacity * 2);
        }

        _data[_size] = val;
        _size++;
    }

    void resize(int size){
        _size = size;
    }

    void reserve(int capacity){
        if(capacity <= _capacity){
            return;
        }
        
        _capacity = capacity;

        T* newData = new T[capacity];

        if(_data != nullptr){
            for(int i = 0; i < _size; ++i){
                newData[i] = _data[i];
            }
            delete[] _data;
        }

        _data = newData;
    }

    int size(){
        return _size;
    }

    int capacity(){
        return _capacity;
    }

    T& operator [](int idx){
        return _data[idx];
    }

    ~Vector(){
        delete[] _data;
    }

private:
    T* _data;
    int _size;
    int _capacity;

public:
    typedef Iterator<T> iterator; 

    iterator begin(){
        if(_data == nullptr){
            return nullptr;
        }

        Iterator it = Iterator(&_data[0]);
        return it;
    }

    iterator end(){
        if(_data == nullptr){
            return nullptr;
        }

        return begin() + _size;
    }
};

int main(){
    int vSize = 10;
    const int MAX = 1000;
    Vector<int> v(10, 10);

    for(int i = 0; i < MAX; ++i){
        v.push_back(i);
        std::cout << "Value: " << v[i] << ", Size: " << v.size() << ", Capacity: " << v.capacity() << std::endl; 
    }

    Vector<int>::iterator it;
    for(it = v.begin(); it != v.end(); it++){
        std::cout << "Val: " << (*it) << std::endl;
    }
}
```