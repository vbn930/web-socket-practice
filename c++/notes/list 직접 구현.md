# list의 핵심 기능들 구현
list의 핵심 함수들만 직접 구현해보자.

## Node
```cpp
template <typename T>
class Node{
public:
    Node() : _data(T()), _next(nullptr), _prev(nullptr){}
    Node(const T& data) : _data(data), _next(nullptr), _prev(nullptr){}
public:
    T _data;
    Node* _next;
    Node* _prev;
};
```
Node 클래스는 단순히 데이터를 담을 멤버 변수와 이전, 다음 노드를 가르키는 포인터로 이루어져있다.

## list
```cpp
template <typename T>
class List{
public:
    typedef Iterator<T> iterator;

    List() : _header(new Node()), _size(0){
        _header->_next = _header;
        _header->_prev = _header;
    }

    List(const T& data) : _header(new Node(data)), _size(0){
        _header->_next = _header;
        _header->_prev = _header;
    }

    int size(){
        return _size;
    }

    void push_back(const T& data){
        Node* before = _header->_prev;
        AddNode(before, data);
    }

    void push_front(const T& data){
        AddNode(_header, data);
    }

    void pop_back(){
        RemoveNode(_header)
    }

    iterator insert(const iterator& iter, const T& data){
        return iterator(AddNode(iter->_node, data));
    }

    iterator erase(const iterator& iter){
        return iterator(RemoveNode(iter->_node));
    }

    void remove(const T& data){
        for(Node* curr = _header->_next; curr != _header;){
            Node* next = curr->_next;

            if(curr->_data == data)
                RemoveNode(curr);
            
            curr = next;
        }
    }

    iterator begin(){
        return iterator(_header->_next);
    }

    iterator end(){
        return iterator(_header);
    }

    ~List(){
        for(Node* curr = _header->_next; curr != _header;){
            RemoveNode(curr);
            curr = _header->_next;
        }

        delete _header;
    }

private:
    typedef Node<T> Node;

    Node* AddNode(Node* before, const T& data){
        Node* next = before->_next;
        Node* newNode = new Node(data);

        before->_next = newNode;
        newNode->_prev = before;

        newNode->_next = next;
        next->_prev = newNode;

        _size++;

        return newNode;
    }

    Node* RemoveNode(Node* node){
        Node* next = node->_next;
        Node* prev = node->_prev;

        prev->_next = next;
        next->_prev = prev;

        delete node;

        _size--;

        return prev;
    }

    Node* _header;
    int _size;
};
```

List 클래스의 멤버 변수는 _header 노트 포인터와 _size 두가지로 단순하다. _header 노드는 _prev 노드로 리스트의 마지막 노드를 가르키고, _next 노트로 리스트의 첫 노드를 가르킨다. 리스트 클래스 내부에서는 _header 노드를 기준으로 리스트의 원소에 접근 하고 있다.

노드의 삭제와 추가를 모듈화 하기 위해서 helper 함수 AddNode와 RemoveNode를 구현하였다.

list의 핵심기능인 원소의 첫/끝부분 삽입과 삭제, 그리고 중간 삽입/삭제를 구현하였다.

또한, Node를 간단하게 사용할수 있도록 타입을 정의하였다.

## iterator
```cpp
template <typename T>
class Iterator{
public:
    Iterator() : _node(nullptr){
    }

    Iterator(Node<T>* node) : _node(node){
    }

    T& operator ++(){
        _node = _node->_next;
        return _node->_data;
    }

    T operator ++(int){
        T temp = _node->_data;
        _node = _node->_next;
        return temp;
    }

    T& operator --(){
        _node = _node->_prev;
        return _node->_data;
    }

    T operator --(int){
        T temp = _node->_data;
        _node = _node->_prev;
        return temp;
    }

    bool operator == (const Iterator& other){
        return _node == other._node;
    }

    bool operator != (const Iterator& other){
        return _node != other._node;
    }

    T& operator *(){
        return _node->_data;
    }

public:
    Node<T>* _node;
};
```
리스트의 반복자는 증감 연산자와 비교연산자, *를 통한 값 획득 연산자를 구현 해 주었다.

리스트의 반복자는 리스트 클래스의 begin, end 함수를 통해 얻을 수 있다.

또한 리스트 클래스 내부에 반복자 타입을 정의하고 사용하고있다.

## 최종 코드와 사용 예시
```cpp
#include <iostream>
#include <list>

using namespace std;

template <typename T>
class Node{
public:
    Node() : _data(T()), _next(nullptr), _prev(nullptr){}
    Node(const T& data) : _data(data), _next(nullptr), _prev(nullptr){}
public:
    T _data;
    Node* _next;
    Node* _prev;
};

template <typename T>
class Iterator{
public:
    Iterator() : _node(nullptr){
    }

    Iterator(Node<T>* node) : _node(node){
    }

    T& operator ++(){
        _node = _node->_next;
        return _node->_data;
    }

    T operator ++(int){
        T temp = _node->_data;
        _node = _node->_next;
        return temp;
    }

    T& operator --(){
        _node = _node->_prev;
        return _node->_data;
    }

    T operator --(int){
        T temp = _node->_data;
        _node = _node->_prev;
        return temp;
    }

    bool operator == (const Iterator& other){
        return _node == other._node;
    }

    bool operator != (const Iterator& other){
        return _node != other._node;
    }

    T& operator *(){
        return _node->_data;
    }

public:
    Node<T>* _node;
};

template <typename T>
class List{
public:
    typedef Iterator<T> iterator;

    List() : _header(new Node()), _size(0){
        _header->_next = _header;
        _header->_prev = _header;
    }

    List(const T& data) : _header(new Node(data)), _size(0){
        _header->_next = _header;
        _header->_prev = _header;
    }

    void push_back(const T& data){
        Node* before = _header->_prev;
        AddNode(before, data);
    }

    void push_front(const T& data){
        AddNode(_header, data);
    }

    void pop_back(){
        RemoveNode(_header->_prev);
    }

    void pop_front(){
        RemoveNode(_header->_next);
    }

    iterator insert(const iterator& iter, const T& data){
        return iterator(AddNode(iter->_node, data));
    }

    iterator erase(const iterator& iter){
        return iterator(RemoveNode(iter->_node));
    }

    void remove(const T& data){
        for(Node* curr = _header->_next; curr != _header;){
            Node* next = curr->_next;

            if(curr->_data == data)
                RemoveNode(curr);
            
            curr = next;
        }
    }

    iterator begin(){
        return iterator(_header->_next);
    }

    iterator end(){
        return iterator(_header);
    }

    ~List(){
        for(Node* curr = _header->_next; curr != _header;){
            RemoveNode(curr);
            curr = _header->_next;
        }

        delete _header;
    }

private:
    typedef Node<T> Node;

    Node* AddNode(Node* before, const T& data){
        Node* next = before->_next;
        Node* newNode = new Node(data);

        before->_next = newNode;
        newNode->_prev = before;

        newNode->_next = next;
        next->_prev = newNode;

        return newNode;
    }

    Node* RemoveNode(Node* node){
        Node* next = node->_next;
        Node* prev = node->_prev;

        prev->_next = next;
        next->_prev = prev;

        delete node;

        return prev;
    }

    Node* _header;
    int _size;
};

int main(){
    const int MAX = 100;
    List<int> li;

    cout << "==============push_back==============" << endl;
    for(int i = 0; i < MAX; i++){
        li.push_back(i);
    }

    cout << "==============push_front==============" << endl;
    for(int i = 0; i < MAX; i++){
        li.push_front(i);
    }

    cout << "==============remove==============" << endl;
    
    li.remove(0);

    for(List<int>::iterator it = li.begin(); it != li.end(); it++){
        cout << "Val: " << (*it) << endl;
    }
}
```