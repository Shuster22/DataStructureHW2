#ifndef HUNTECH26A2_H_
#define HUNTECH26A2_H_

#include "wet2util.h"
#include "AvlTree.h"
#include "DoubleHashTable.h"
#include "Union.h"
#include "Hunter.h"
#include "Squad.h"

class Huntech {
private:
    struct AuraKey {
        int aura;
        int id;

        AuraKey() : aura(0), id(0) {}
        AuraKey(int a, int id) : aura(a), id(id) {}

        bool operator<(const AuraKey& other) const {
            if (aura != other.aura) return aura < other.aura;
            return id < other.id;
        }
        bool operator>(const AuraKey& other) const {
            return other < *this;
        }
        bool operator==(const AuraKey& other) const {
            return aura == other.aura && id == other.id;
        }
    };

    DoubleHashTable<int, int> hashTable;
    Union<Hunter> huntersUnion;
    AvlTree<int, unique_ptr<Squad>> squadsTree;
    AvlTree<AuraKey, int> squadsAuraTree;

    Squad& find_winner_squad(int squadId1, int squadId2);

public:
    Huntech();
    virtual ~Huntech();

    StatusType add_squad(int squadId);
    StatusType remove_squad(int squadId);

    StatusType add_hunter(int hunterId,
                          int squadId,
                          const NenAbility &nenType,
                          int aura,
                          int fightsHad);

    output_t<int> squad_duel(int squadId1, int squadId2);
    output_t<int> get_hunter_fights_number(int hunterId);
    output_t<int> get_squad_experience(int squadId);
    output_t<int> get_ith_collective_aura_squad(int i);
    output_t<NenAbility> get_partial_nen_ability(int hunterId);
    StatusType force_join(int forcingSquadId, int forcedSquadId);
};

#endif