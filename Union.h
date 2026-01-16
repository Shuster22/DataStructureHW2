
#include "DynamicArray.h"
#ifndef UNION_H
#define UNION_H

template <class T>
class Union {
DynamicArray<T> unionF;

public:
    Union() = default;
    int makeSet(unique_ptr<T> value);
    int find(int idx);
    void combine(int idx1 , int idx2);
};
template <class T>
int Union<T>::makeSet(unique_ptr<T> value) {
    int idx = unionF.push_back(move(value));
    return idx;
}

template <class T>
int Union<T>::find(int idx) {
    int next = idx; //given index
    int tempP = 0; //parent
    while(unionF[idx].parent != idx) idx=unionF[idx].parent;
    tempP = idx;
    idx = next;
    while(unionF[idx].parent != idx) {
        next = unionF[idx].parent;
        unionF[idx].parent = tempP;
        idx = next;
    }
    return tempP;
}

template <class T>
void Union<T>::combine(int idx1 , int idx2) {
    int rIdx1 = find(idx1);
    int rIdx2 = find(idx2);
    if(rIdx1 == rIdx2) return;
    else if(unionF[rIdx1].size >= unionF[rIdx2].size) {
        unionF[rIdx2].parent = rIdx1;
        unionF[rIdx2].nextChrono = unionF[rIdx1].lastChrono;
        unionF[rIdx1].lastChrono = unionF[rIdx2].lastChrono;
        unionF[rIdx1].size += unionF[rIdx2].size;
    }
    else {
        unionF[rIdx1].parent = rIdx2;
        unionF[rIdx1].nextChrono = unionF[rIdx2].lastChrono;
        unionF[rIdx2].lastChrono = unionF[rIdx1].lastChrono;
        unionF[rIdx2].size += unionF[rIdx1].size;
    }
}

#endif //UNION_H
