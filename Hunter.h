#ifndef HUNTER_H
#define HUNTER_H

#include "wet2util.h"

class Hunter {
    int id;
    NenAbility ability;
    int aura;
    int fightsHad;
public:
    Hunter(int id , const NenAbility& hAbility ,int aura, int fightsHad) : id(id) ,
    ability(hAbility) ,aura(aura), fightsHad(fightsHad){}
    int getFights();
    void setFights(int fights);
    NenAbility& getNenAbility();
    NenAbility& getNenAbility() const;
};



#endif //HUNTER_H
