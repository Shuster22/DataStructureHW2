#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H
#include <stdexcept>
#include <memory>
using namespace std;

template <class T>
struct ANode {
    unique_ptr<T> value;
    int parent;
    int size;
    int experience;
    ANode() : value(nullptr) , parent(-1) ,size(0) ,experience(0){}
};

template <class T>
class DynamicArray {
    unique_ptr<ANode<T>[]> head;
    int size;
    int capacity;
    void reserve();
public:
    DynamicArray(): head(make_unique<ANode<T>[]>(1)),size(0),capacity(1){}
    ANode<T>& operator[](int i);
    int push_back(T newPtr);
};
template <class T>
void DynamicArray<T>::reserve() {
    auto temp =  make_unique<ANode<T>[]>(capacity*2); // to not change capacity if storge fail
    capacity = capacity * 2 ;
    for(int i = 0; i < size ;i++) {
        temp[i].value = move(head[i].value);
        temp[i].parent = head[i].parent;
        temp[i].size = head[i].size;
        temp[i].experience = head[i].experience;
    }
    head = std::move(temp);
}

template <class T>
ANode<T>& DynamicArray<T>::operator[](int i) {
    if(i < size) return head[i];
    throw out_of_range("DynamicArray index out of range");;
}
template <class T>
int DynamicArray<T>::push_back(T newPtr) {
    unique_ptr<T> uPtr = make_unique<T>(move(newPtr));
    if(size == capacity) {
        reserve();
    }
    head[size].value = move(uPtr);
    head[size].parent = size;
    head[size].size = 1;
    size++;
    return size-1;
}
#endif //DYNAMICARRAY_H
