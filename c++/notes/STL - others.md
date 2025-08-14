set -> map과 비슷하지만 key와 value가 같은값
key 값이 곧 value 값
[] 연산은 불가능

multimap -> 중복 키를 허용하는 map
map과 비슷하다. map과 같은 내부구조를 가짐. [] 연산자 안됨 erase하면 해당 키에 할당된 값 모두 삭제
equal_range -> 찾은 범위의 반복자 pair 반환 (second는 찾은 범위의 다음 위치 반환)

multiset -> 중복 키를 허용하는 set
multimap과 대부분 같은 문법
multimap과 같은 방법으로 같은 키를 가진 값의 범위를 찾을수 있다. 