#include <memory>
#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

using namespace std;

template <class T>
struct Node {
    unique_ptr<T> value;
    int parent;
    int nextChrono;
    int size;
    int lastChrono;
    Node() : value(nullptr) , parent(-1) ,nextChrono(-1) ,size(0) ,lastChrono(-1){}
};

template <class T>
class DynamicArray {
    unique_ptr<Node<T>[]> head;
    int size;
    int capacity;
    void reserve();
public:
    DynamicArray(): head(make_unique<Node<T>[]>(1)),size(0),capacity(1){}
    Node<T>& operator[](int i);
    int push_back(unique_ptr<T> newPtr);

};
template <class T>
void DynamicArray<T>::reserve() {
    auto temp =  make_unique<Node<T>[]>(capacity*2); // to not change capacity if storge fail
    capacity = capacity * 2 ;
    for(int i = 0;i < size;i++) {
        temp[i].value = move(head[i].value);
        temp[i].parent = head[i].parent;
        temp[i].size = head[i].size;
    }
    head = std::move(temp);
}

template <class T>
Node<T>& DynamicArray<T>::operator[](int i) {
    if(i < size) return head[i];
    throw out_of_range("DynamicArray index out of range");;
}
template <class T>
int DynamicArray<T>::push_back(unique_ptr<T> newPtr) {
    if(size == capacity) {
        reserve();
    }
    head[size].value = move(newPtr);
    head[size].parent = size;
    head[size].lastChrono = size;
    head[size].size = 1;
    size++;
    return size-1;
}
#endif //DYNAMICARRAY_H
