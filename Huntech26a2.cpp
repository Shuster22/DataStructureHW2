// You can edit anything you want in this file.
// However, you need to implement all public Huntech functions, which are provided below as a template.

#include "Huntech26a2.h"


Huntech::Huntech::Huntech() {

}

Huntech::~Huntech() {

}

StatusType Huntech::add_squad(int squadId) {
    if(squadId <= 0) return StatusType::INVALID_INPUT;
    try {

        squadsTree.insert(squadId, make_unique<Squad>(squadId));
        AuraKey key(0, squadId);
        squadsAuraTree.insert(key, make_unique<Squad>(squadId));

    }
    catch(bad_alloc& e) {
        return StatusType::ALLOCATION_ERROR;
    }
    catch(StatusType e) {
        return e;
    }
    return StatusType::SUCCESS;
}

StatusType Huntech::remove_squad(int squadId) {
    if(squadId <= 0) return StatusType::INVALID_INPUT;
    try {
        int totalAura = squadsTree.search(squadId).totalAura;
        AuraKey key(totalAura, squadId);
        squadsAuraTree.del(key);
        squadsTree.del(squadId);
    }
    catch(bad_alloc& e) {
        return StatusType::ALLOCATION_ERROR;
    }
    catch(StatusType e) {
        return e;
    }
    return StatusType::SUCCESS;
}

StatusType Huntech::add_hunter(int hunterId,
                               int squadId,
                               const NenAbility &nenType,
                               int aura,
                               int fightsHad)
{
    if(squadId <= 0 || hunterId <= 0 || !nenType.isValid()||fightsHad < 0) return StatusType::INVALID_INPUT;
    try {
        if(hashTable.find(hunterId) != int()) return StatusType::FAILURE;
        Squad& squad = squadsTree.search(squadId);

        // update squad total aura and nen ability
        int totalAura = squad.totalAura;
        squad.totalAura += aura;
        squad.totalNenAbility += nenType.getEffectiveNenAbility();

        // update squad aura in squadsAuraTree
        AuraKey key(totalAura, squadId);
        squadsAuraTree.del(key);
        AuraKey newKey(squad.totalAura, squadId);
        squadsAuraTree.insert(newKey, make_unique<Squad>(squadId));

        int newIdx = huntersUnion.makeSet(Hunter(hunterId ,nenType,aura,fightsHad));
        hashTable.insert(hunterId,newIdx); // save hunter id, Union Index
        int uHeadIdx = squad.getUnionHead();
        if(uHeadIdx == -1) squad.setUnionHead(newIdx); //if squad has no head put new hunter has head of the squad
        else {
            huntersUnion.combine(newIdx,uHeadIdx); // add newHunter to his Squad
            squad.setUnionHead(huntersUnion.find(newIdx)); //set newUnionHead
        }
    }
    catch(bad_alloc& e) {
        return StatusType::ALLOCATION_ERROR;
    }
    catch(StatusType e) {
        return e;
    }
    return StatusType::SUCCESS;
}

output_t<int> Huntech::squad_duel(int squadId1, int squadId2) {

    if (squadId1 <= 0 || squadId2 <= 0 || squadId1 == squadId2)
        return output_t<int>(StatusType::INVALID_INPUT);

    int effective_aura_1, effective_aura_2;

    try {
        Squad& squad1 = squadsTree.search(squadId1);
        Squad& squad2 = squadsTree.search(squadId2);


        int root_1 = squad1.getUnionHead();
        int root_2 = squad2.getUnionHead();

        // one of the squads is empty
        if (root_1 == -1 || root_2 == -1)
            return output_t<int>(StatusType::FAILURE);

        int exp_1 = huntersUnion.get_exp(root_1);
        int exp_2 = huntersUnion.get_exp(root_2);

        NenAbility ab_1 = huntersUnion.ability(root_1);
        NenAbility ab_2 = huntersUnion.ability(root_2);

        // get aura
        int Aura_1 = squad1.totalAura;
        int Aura_2 = squad2.totalAura;

        effective_aura_1 = exp_1 + Aura_1;
        effective_aura_2 = exp_2 + Aura_2;

        if(effective_aura_1 > effective_aura_2) {
            huntersUnion.add_exp(root_1,3);
            return output_t<int>(1);

        }
        if(effective_aura_2 > effective_aura_1) {
            huntersUnion.add_exp(root_2,3);
            return output_t<int>(2);
        }

        if(ab_1 > ab_2) {
            huntersUnion.add_exp(root_1,3);
            return output_t<int>(3);
        }

         if(ab_2 > ab_1) {
            huntersUnion.add_exp(root_2,3);
            return output_t<int>(4);
        }

        // draw
        huntersUnion.add_exp(root_1,1);
        huntersUnion.add_exp(root_2,1);
        return output_t<int>(0);
    }
    catch(...) {
        return output_t<int>(StatusType::FAILURE);
    }

    }


Squad& Huntech::find_winner_squad(int squadId1, int squadId2) {
    int effective_aura_1, effective_aura_2;

    Squad& squad1 = squadsTree.search(squadId1);
    Squad& squad2 = squadsTree.search(squadId1);

    int root_1 = squad1.getUnionHead();
    int root_2 = squad2.getUnionHead();

    int exp_1 = huntersUnion.exp(root_1);
    int exp_2 = huntersUnion.exp(root_2);

    int ab_1 = huntersUnion.ability(root_1).getEffectiveNenAbility();
    int ab_2 = huntersUnion.ability(root_2).getEffectiveNenAbility();

    // get aura
    int Aura_1 = squad1.totalAura;
    int Aura_2 = squad2.totalAura;

    effective_aura_1 = exp_1 + Aura_1;
    effective_aura_2 = exp_2 + Aura_2;
    if(effective_aura_1 > effective_aura_2) {
        Squad& winner = squad1;
    }
    else if(effective_aura_2 > effective_aura_1) {
        Squad& winner= squad2;
    }
    else {
        if(ab_1 > ab_2) {
            Squad& winner = squad1;
        }
        else if(ab_2 > ab_1) {
            Squad& winner = squad2;
        }
        else {
            return output_t<int>(0); // draw
        }
    }

    return 0;
}

output_t<int> Huntech::get_hunter_fights_number(int hunterId) {
    int fights = 0;
    if(hunterId <= 0) return output_t<int>(StatusType::INVALID_INPUT);
    try {
        int uIdx = hashTable.find(hunterId);
        fights = huntersUnion.fightsHad(uIdx); // log*(n)

    }
    catch(bad_alloc& e) {
        return StatusType::ALLOCATION_ERROR;
    }
    catch(StatusType e) {
        return e;
    }
    return output_t<int>(fights);
}

output_t<int> Huntech::get_squad_experience(int squadId) {
    int exp = 0;
    if(squadId <= 0) return output_t<int>(StatusType::INVALID_INPUT);
    try {
        Squad& squad = squadsTree.search(squadId);
        exp = huntersUnion.exp(squad.getUnionHead());
    }
    catch(bad_alloc& e) {
        return StatusType::ALLOCATION_ERROR;
    }
    catch(StatusType e) {
        return e;
    }
    return output_t<int>(exp);
}

output_t<int> Huntech::get_ith_collective_aura_squad(int i) {
    return 0;
}

output_t<NenAbility> Huntech::get_partial_nen_ability(int hunterId) {
    NenAbility ability;
    if(hunterId <= 0) return output_t<NenAbility>(StatusType::INVALID_INPUT);
    try {
        int uIdx = hashTable.find(hunterId);
        ability = huntersUnion.ability(uIdx); // log*(n)
    }
    catch(bad_alloc& e) {
        return StatusType::ALLOCATION_ERROR;
    }
    catch(StatusType e) {
        return e;
    }
    return output_t<NenAbility>(ability);
}

StatusType Huntech::force_join(int forcingSquadId, int forcedSquadId) {
    return StatusType::FAILURE;
}
