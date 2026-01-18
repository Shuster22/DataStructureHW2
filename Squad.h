//
// Created by bensh on 17/01/2026.
//

#ifndef SQUAD_H
#define SQUAD_H



class Squad {
    int id;
    int uHeadIdx; // union Head Index
public:
    explicit Squad(int id) : id(id) , uHeadIdx(-1){}
    int getUnionHead();
    void setUnionHead(int uHead);
};



#endif //SQUAD_H
