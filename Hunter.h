#ifndef HUNTER_H
#define HUNTER_H

#include "wet2util.h"

class Hunter {
    int id;
    NenAbility ability;
    int aura;
    int fightsHad;
    bool alive;
public:
    Hunter(int id , const NenAbility& hAbility ,int aura, int fightsHad) : id(id) ,
    ability(hAbility) ,aura(aura), fightsHad(fightsHad) ,alive(true) {}
    int getFights();
    NenAbility& getNenAbility();
    void setFights(int fights);
    bool isAlive() const;
    void setAlive(bool value);
};



#endif //HUNTER_H
