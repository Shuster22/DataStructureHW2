
#include "DynamicArray.h"
#include "wet2util.h"
#ifndef UNION_H
#define UNION_H

using namespace std;

template <class T>
class Union {
DynamicArray<T> unionF;

public:
    Union() = default;
    int makeSet(T value);
    int find(int idx);
    int fightsHad(int idx);
    int get_exp(int idx);
    void add_exp(int idx, int exp);

    NenAbility ability(int idx);
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
    NenAbility ability;
    NenAbility tempAbility;
    int tempFights = 0;
    while(unionF[idx].parent != idx) {
        fights +=unionF[idx].value->getFights();
        ability+=unionF[idx].value->getNenAbility();
        idx= unionF[idx].parent;
    }
    fights +=unionF[idx].value->getFights();
    ability+= unionF[idx].value->getNenAbility();
    tempP = idx;
    idx = next;
    while(unionF[idx].parent != idx) {
        tempFights = unionF[idx].value->getFights();
        unionF[idx].value->setFights(fights-unionF[tempP].value->getFights());
        fights-= tempFights;

        tempAbility = unionF[idx].value->getNenAbility();
        unionF[idx].value->getNenAbility() = ability - unionF[tempP].value->getNenAbility();
        ability-= tempAbility;

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
        NenAbility BigRoot = unionF[rIdx1].value->getNenAbility();
        NenAbility SmallRoot = unionF[rIdx2].value->getNenAbility();
        unionF[rIdx2].value->getNenAbility() = SmallRoot-BigRoot;
        ;
        unionF[rIdx2].value->setFights(unionF[rIdx2].value->getFights() - unionF[rIdx1].value->getFights());
        unionF[rIdx1].size += unionF[rIdx2].size;
        unionF[rIdx1].experience+= unionF[rIdx2].experience;
        unionF[rIdx2].parent = rIdx1;

    }
    else {
        NenAbility BigRoot = unionF[rIdx2].value->getNenAbility();
        NenAbility SmallRoot = unionF[rIdx1].value->getNenAbility();
        unionF[rIdx1].value->getNenAbility() = SmallRoot-BigRoot;

        unionF[rIdx1].value->setFights(unionF[rIdx1].value->getFights() - unionF[rIdx2].value->getFights());

        unionF[rIdx2].size += unionF[rIdx1].size;
        unionF[rIdx2].experience+= unionF[rIdx1].experience;
        unionF[rIdx1].parent = rIdx2;
    }
}

template <class T>
int Union<T>::fightsHad(int idx) {
    int p = find(idx);
    return unionF[p].value->getFights() + unionF[idx].value->getFights();
}
template<class T>
int Union<T>::get_exp(int idx) {
    int p = find(idx);
    return unionF[p].experience ;
}

template<class T>
void Union<T>::add_exp(int idx, int exp) {
    unionF[idx].experience += exp ;
}

template<class T>
NenAbility Union<T>::ability(int idx) {
    int p = find(idx);
    return unionF[p].value->getNenAbility()+ unionF[idx].value->getNenAbility();
}

#endif //UNION_H
