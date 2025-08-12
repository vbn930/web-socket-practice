# push_back과 emplace_back
vector에는 끝에 원소를 추가하는 함수인 push_back과 emplace_back가 존재한다. 이 두가지 함수는 같은 방법으로 사용할 수 있고, 결과도 같다. 하지만, 두 함수는 내부적으로 동작하는 방식이 다르다.

## push_back
먼저 push_back은 삽입할 원소의 객체를 복사 혹은 이동하는 방법으로 vector에 원소를 추가한다. 만약 그냥 객체를 넘겨주거나 l-value의 객체를 넘겨주면, 해당 객체를 복사해서 삽입하고, r-value 혹은 std::move를 통해 객체를 넘겨주면 해당 객체를 vector로 이동하는 방식으로 객체를 삽입한다.

## emplace_back
emplace_back은 push_back과 내부적으로 다른 방식으로 원소를 삽입한다. 만약 empalce_back에 push_back과 같이 넣으려는 객체 자체를 넣게 되면, push_back과 거의 비슷하게 동작하여 비슷한 성능을 내지만, 해당 객체의 자체가 아닌, 객체의 생성을 위한 생성 인자를 전달하면, emplace_back은 vector 내부적으로 해당 객체의 생성자를 호출하여 새롭게 객체를 생성하게 된다. 이는 복사와 이동보다 오버헤드가 더 적기 때문에, 더 좋은 성능을 낼 수 있다.

```cpp
class MyClass{
    MyClass() : _val(0){
        std::cout << "기본 생성자 호출!" << std::endl;
    }
    MyClass(int val)  : _val(val){
        std::cout << "생성자 호출!" << std::endl;
    }

    int val(){
        return _val;
    }

private:
    int _val;
};

std::vector<MyClass> vec;
MyClass myClass1(10);
vec.emplace_back(myClass1); // push_back과 같은 방법으로 동작 - 해당 객체를 복사
vec.emplace_back(myClass1.val()); // MyClass의 생성 인자를 전달하면, vector 내부에서 해당 생성인자로 객체를 새로 생성해서 삽입한다.
```

위의 클래스는 단순히 int 멤버 하나만을 가진 클래스이지만, 만약 클래스가 복잡하고 크기가 크다면, emplace_back을 통한 원소 삽입이 push_back을 통한 원소 삽입보다 더 효율적이고 좋은 성능을 낸다.