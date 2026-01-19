// 
// 234218 Data Structures 1.
// Semester: 2026A (Winter).
// Wet Exercise #2 - Huntech.
// 
// The following header file contains all methods we expect you to implement.
// You MAY add private methods and fields of your own.
// DO NOT erase or modify the signatures of the public methods.
// DO NOT modify the preprocessors in this file.
// DO NOT use the preprocessors in your other code files.
// 

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
    //
    // Here you may add anything you need to implement your Huntech class
    //
    struct AuraKey {
        int aura;
        int id;

        // The AVL tree uses this to decide which squad comes "first" (Rank 1)
        bool operator<(const AuraKey& other) const {
            if (this->aura != other.aura) {
                return this->aura < other.aura;
            }
            // If auras are equal,
            // the one with the LARGER ID is considered "smaller"
            // so that the one with the SMALLER ID is ranked higher.
            return this->id > other.id;
        }

        bool operator>(const AuraKey& other) const {
            return other < *this;
        }

        bool operator==(const AuraKey& other) const {
            return this->aura == other.aura && this->id == other.id;
        }
    };



    DoubleHashTable<int, int> hashTable;
    Union<Hunter> huntersUnion;
    AvlTree<int, unique_ptr<Squad>> squadsTree;
    AvlTree<AuraKey, Squad*> squadsAuraTree;

    Squad& find_winner_squad(int squadId1, int squadId2);
public:
    // <DO-NOT-MODIFY> {

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

    // } </DO-NOT-MODIFY>
};

#endif // HUNTECH26A2_H_
