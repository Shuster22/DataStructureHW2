//
// Created by bensh on 16/01/2026.
//
#include "DynamicArray.h"
#ifndef UNION_H
#define UNION_H

template <class T>
class Union {
DynamicArray<T> unionF;

public:
    Makeset(unique_ptr<T> value) {
        unionF->push_back(value,-1); // -1 : to put himself as parent
    }
};

#endif //UNION_H
