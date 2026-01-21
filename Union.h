
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
    void addFight(int idx1,int idx2);
    int get_exp(int idx);
    void add_exp(int idx, int exp);
    void kill(int idx);
    bool is_alive(int idx);
    NenAbility ability(int idx);
    void combine(int idx1 , int idx2,int order);
};


template <class T>
int Union<T>::makeSet(T value) {
    int idx = unionF.push_back(move(value));
    unionF[idx].selfNen = unionF[idx].value->getNenAbility();
    return idx;
}

template <class T>
int Union<T>::find(int idx) {

    if(unionF[idx].parent == idx) return idx;
    int next = idx; //given index
    int tempP = 0; //parent
    int fights = 0;
    NenAbility ability;
    NenAbility tempAbility;
    int tempFights = 0;
    while(unionF[idx].parent != idx) {
        fights +=unionF[idx].value->getFights();
        ability += unionF[idx].selfNen;
        idx= unionF[idx].parent;
    }
    fights +=unionF[idx].value->getFights();
    tempP = idx;
    idx = next;
    while(unionF[idx].parent != idx) {
        tempFights = unionF[idx].value->getFights();
        unionF[idx].value->setFights(fights-unionF[tempP].value->getFights());
        fights-= tempFights;

        tempAbility = unionF[idx].selfNen;
        unionF[idx].selfNen = ability;
        ability-= tempAbility;

        next = unionF[idx].parent;
        unionF[idx].parent = tempP;
        idx = next;
    }
    return tempP;
}

template <class T>
void Union<T>::combine(int idx1 , int idx2,int order) {
    int rIdx1 = find(idx1);
    int rIdx2 = find(idx2);
    if(rIdx1 == rIdx2) throw StatusType::FAILURE;
    else if(unionF[rIdx1].size >= unionF[rIdx2].size) {
        unionF[rIdx1].groupNen +=unionF[rIdx2].selfNen;
        unionF[rIdx2].selfNen =unionF[rIdx1].groupNen;
        unionF[rIdx1].groupNen+=unionF[rIdx2].groupNen;

        unionF[rIdx2].value->setFights(unionF[rIdx2].value->getFights() - unionF[rIdx1].value->getFights());

        unionF[rIdx1].size += unionF[rIdx2].size;
        unionF[rIdx1].experience+= unionF[rIdx2].experience;
        unionF[rIdx2].parent = rIdx1;

    }
    else {

        //called from force_join
        if(order == 1) {
            unionF[rIdx2].selfNen+=unionF[rIdx1].selfNen+unionF[rIdx1].groupNen;
            unionF[rIdx1].selfNen -= unionF[rIdx2].selfNen;
        }
        else {
            unionF[rIdx2].groupNen += unionF[rIdx2].selfNen;
            unionF[rIdx1].selfNen = unionF[rIdx1].groupNen;
            unionF[rIdx1].groupNen+= unionF[rIdx2].groupNen;
        }
        unionF[rIdx1].value->setFights(unionF[rIdx1].value->getFights() - unionF[rIdx2].value->getFights());

        unionF[rIdx2].size += unionF[rIdx1].size;
        unionF[rIdx2].experience+= unionF[rIdx1].experience;
        unionF[rIdx1].parent = rIdx2;
    }
}

template <class T>
int Union<T>::fightsHad(int idx) {
    int p = find(idx);
    if(p == idx) return unionF[p].value->getFights();
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
    if(p == idx) return unionF[p].selfNen;
    return unionF[p].selfNen + unionF[idx].selfNen;
}
template<class T>
void Union<T>::kill(int idx) {
    if(idx == -1) throw StatusType::FAILURE;
    unionF[idx].value->setAlive(false);
}
template<class T>
bool Union<T>::is_alive(int idx) {
    int p = find(idx);
    return unionF[p].value->isAlive();
}
template<class T>
void Union<T>::addFight(int idx1, int idx2) {
    unionF[idx1].value->setFights(unionF[idx1].value->getFights()+1);
    unionF[idx2].value->setFights(unionF[idx2].value->getFights()+1);
}
#endif //UNION_H
