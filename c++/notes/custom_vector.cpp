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

        if(_data == nullptr){
            _data = newData;
            return;
        }

        for(int i = 0; i < _size; ++i){
            newData[i] = _data[i];
        }

        delete[] _data;
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