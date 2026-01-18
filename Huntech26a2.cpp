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
        if(hashTable.find(hunterId) != nullptr) return StatusType::FAILURE;
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

    return 0;
}

output_t<int> Huntech::get_hunter_fights_number(int hunterId) {
    return 0;
}

output_t<int> Huntech::get_squad_experience(int squadId) {
    return 0;
}

output_t<int> Huntech::get_ith_collective_aura_squad(int i) {
    return 0;
}

output_t<NenAbility> Huntech::get_partial_nen_ability(int hunterId) {
    return NenAbility();
}

StatusType Huntech::force_join(int forcingSquadId, int forcedSquadId) {
    return StatusType::FAILURE;
}
