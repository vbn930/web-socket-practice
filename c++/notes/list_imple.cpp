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