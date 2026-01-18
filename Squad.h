//
// Created by bensh on 17/01/2026.
//

#ifndef SQUAD_H
#define SQUAD_H



class Squad {
    int id;
    int uHeadIdx; // union Head Index
public:
    int totalAura;
    int totalNenAbility;
    int Experience;


    explicit Squad(int id) : id(id) , uHeadIdx(-1), totalAura(0),
    totalNenAbility(0), Experience(0){}
    int getUnionHead();
    void setUnionHead(int uHead);
};

#endif //SQUAD_H
