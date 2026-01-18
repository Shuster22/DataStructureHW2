
#include "DynamicArray.h"
#include "wet2util.h"
#ifndef UNION_H
#define UNION_H

template <class T>
class Union {
DynamicArray<T> unionF;

public:
    Union() = default;
    int makeSet(T value);
    int find(int idx);
    int fightsHad(int idx);
    void combine(int idx1 , int idx2);
};


template <class T>
int Union<T>::makeSet(T value) {
    int idx = unionF.push_back(move(value));
    return idx;
}

template <class T>
int Union<T>::find(int idx) {
    int next = idx; //given index
    int tempP = 0; //parent
    int fights = 0;
    int tempFights = 0;
    while(unionF[idx].parent != idx) {
        fights +=unionF[idx].value->getFights();
        idx= unionF[idx].parent;
    }
    fights +=unionF[idx].value->getFights();
    tempP = idx;
    idx = next;
    while(unionF[idx].parent != idx) {
        tempFights = unionF[idx].value->getFights();
        unionF[idx].value->setFights(fights-unionF[tempP].value->getFights());
        fights-= tempFights;
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
    if(rIdx1 == rIdx2) throw StatusType::FAILURE;
    else if(unionF[rIdx1].size >= unionF[rIdx2].size) {
        unionF[rIdx2].parent = rIdx1;
        unionF[rIdx2].value->setFights(unionF[rIdx2].value->getFights() - unionF[rIdx1].value->getFights());
        unionF[rIdx2].nextChrono = unionF[rIdx1].lastChrono;
        unionF[rIdx1].lastChrono = unionF[rIdx2].lastChrono;
        unionF[rIdx1].size += unionF[rIdx2].size;
    }
    else {
        unionF[rIdx1].parent = rIdx2;
        unionF[rIdx1].value->setFights(unionF[rIdx1].value->getFights() - unionF[rIdx2].value->getFights());
        unionF[rIdx1].nextChrono = unionF[rIdx2].lastChrono;
        unionF[rIdx2].lastChrono = unionF[rIdx1].lastChrono;
        unionF[rIdx2].size += unionF[rIdx1].size;
    }
}

template <class T>
int Union<T>::fightsHad(int idx) {
    int sum = 0;
    while(unionF[idx].parent != idx) {
        sum+=unionF[idx].value->getFights();
        idx = unionF[idx].parent;
    }
    sum+=unionF[idx].value->getFights();
    return sum;
}

#endif //UNION_H
