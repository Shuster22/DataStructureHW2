#ifndef HUNTER_H
#define HUNTER_H

#include "wet2util.h"

class Hunter {
    int id;
    NenAbility ability;
public:
    Hunter(int id , NenAbility& hAbility) : id(id) , ability(hAbility){}
};



#endif //HUNTER_H
