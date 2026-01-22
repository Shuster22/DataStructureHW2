#include "Huntech26a2.h"

Huntech::Huntech() = default;
Huntech::~Huntech() = default;

StatusType Huntech::add_squad(int squadId) {
    if(squadId <= 0) return StatusType::INVALID_INPUT;
    try {
        squadsTree.insert(squadId, make_unique<Squad>(squadId));
        try {
            AuraKey key(0, squadId);
            squadsAuraTree.insert(key, &squadsTree.search(squadId));
        }
        // make sure to rollback if second insertion fails
        catch(...) {
            squadsTree.del(squadId);
            throw;
        }

    }
    catch(bad_alloc&) {
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
        Squad& squad = squadsTree.search(squadId);

        AuraKey key(squad.totalAura, squadId);
        squadsAuraTree.del(key);

        int head = squad.getUnionHead();
        if(head != -1) {
            huntersUnion.kill(head);
        }

        squadsTree.del(squadId);
    }
    catch(bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
    catch(...) {
        return StatusType::FAILURE;
    }
    return StatusType::SUCCESS;
}

StatusType Huntech::add_hunter(int hunterId,
                               int squadId,
                               const NenAbility &nenType,
                               int aura,
                               int fightsHad)
{
    // FIX: aura < 0 must be INVALID_INPUT (per wet2 spec)
    if(squadId <= 0 || hunterId <= 0 || !nenType.isValid() || aura < 0 || fightsHad < 0) return StatusType::INVALID_INPUT;
    try {
        if(hashTable.find(hunterId) != -1) return StatusType::FAILURE;
        Squad& squad = squadsTree.search(squadId);

        int oldAura = squad.totalAura;
        int newAura = oldAura + aura;
        int oldNen = squad.totalNenAbility;

        AuraKey oldKey(oldAura, squadId);
        AuraKey newKey(newAura, squadId);

        squadsAuraTree.del(oldKey);

        try {
            squadsAuraTree.insert(newKey, &squad);
        }
        catch (...) {
            // rollback on failure
            squadsAuraTree.insert(oldKey, &squad);
            throw;
        }

        squad.totalAura = newAura;
        squad.totalNenAbility = oldNen;

        try {
            int newIdx = huntersUnion.makeSet(Hunter(hunterId, nenType, aura, fightsHad));
            hashTable.insert(hunterId, newIdx);
            int uHeadIdx = squad.getUnionHead();
            if(uHeadIdx == -1)
                squad.setUnionHead(newIdx);
            else {
                huntersUnion.combine(uHeadIdx, newIdx, 0);
            }
        }
        catch (...) {
            // rollback on failure
            squadsAuraTree.del(newKey);
            squadsAuraTree.insert(oldKey, &squad);
            squad.totalAura = oldAura;
            squad.totalNenAbility = oldNen;
            throw;
        }

    }
    catch(bad_alloc&) {
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

        if (root_1 == -1 || root_2 == -1)
            return output_t<int>(StatusType::FAILURE);

        huntersUnion.addFight(root_1, root_2);
        int exp_1 = huntersUnion.get_exp(root_1);
        int exp_2 = huntersUnion.get_exp(root_2);

        NenAbility ab_1 = huntersUnion.partialAbility(huntersUnion.lastChrono(root_1));
        NenAbility ab_2 = huntersUnion.partialAbility(huntersUnion.lastChrono(root_2));

        int Aura_1 = squad1.totalAura;
        int Aura_2 = squad2.totalAura;

        effective_aura_1 = exp_1 + Aura_1;
        effective_aura_2 = exp_2 + Aura_2;

        if(effective_aura_1 > effective_aura_2) {
            huntersUnion.add_exp(root_1, 3);
            return output_t<int>(1);
        }
        if(effective_aura_2 > effective_aura_1) {
            huntersUnion.add_exp(root_2, 3);
            return output_t<int>(3);
        }

        if(ab_1 > ab_2) {
            huntersUnion.add_exp(root_1, 3);
            return output_t<int>(2);
        }

        if(ab_2 > ab_1) {
            huntersUnion.add_exp(root_2, 3);
            return output_t<int>(4);
        }

        huntersUnion.add_exp(root_1, 1);
        huntersUnion.add_exp(root_2, 1);
        return output_t<int>(0);
    }
    catch(bad_alloc&) {
        return output_t<int>(StatusType::ALLOCATION_ERROR);
    }
    catch(...) {
        return output_t<int>(StatusType::FAILURE);
    }
}

output_t<int> Huntech::get_hunter_fights_number(int hunterId) {
    int fights = 0;
    if(hunterId <= 0) return output_t<int>(StatusType::INVALID_INPUT);
    try {
        int uIdx = hashTable.find(hunterId);
        if(uIdx == -1) return output_t<int>(StatusType::FAILURE);
        fights = huntersUnion.fightsHad(uIdx);
    }
    catch(bad_alloc&) {
        return output_t<int>(StatusType::ALLOCATION_ERROR);
    }
    catch(StatusType e) {
        return output_t<int>(e);
    }
    return output_t<int>(fights);
}

output_t<int> Huntech::get_squad_experience(int squadId) {
    int exp = 0;
    if(squadId <= 0) return output_t<int>(StatusType::INVALID_INPUT);
    try {
        Squad& squad = squadsTree.search(squadId);
        int head = squad.getUnionHead();
        if(head == -1) return output_t<int>(0);
        exp = huntersUnion.get_exp(head);
    }
    catch(bad_alloc&) {
        return output_t<int>(StatusType::ALLOCATION_ERROR);
    }
    catch(StatusType e) {
        return output_t<int>(e);
    }
    return output_t<int>(exp);
}

output_t<int> Huntech::get_ith_collective_aura_squad(int i) {
    if(i < 0) return output_t<int>(StatusType::FAILURE);
    try {
        AuraKey key = squadsAuraTree.get_ith_id(i);
        return output_t<int>(key.id);
    }
    catch(bad_alloc&) {
        return output_t<int>(StatusType::ALLOCATION_ERROR);
    }
    catch (...) {
        return output_t<int>(StatusType::FAILURE);
    }
}

output_t<NenAbility> Huntech::get_partial_nen_ability(int hunterId) {
    NenAbility ability;
    if(hunterId <= 0) return output_t<NenAbility>(StatusType::INVALID_INPUT);
    try {
        int uIdx = hashTable.find(hunterId);
        if(uIdx == -1) return output_t<NenAbility>(StatusType::FAILURE);
        if(!huntersUnion.is_alive(uIdx)) return output_t<NenAbility>(StatusType::FAILURE);
        ability = huntersUnion.partialAbility(uIdx);
    }
    catch(bad_alloc&) {
        return output_t<NenAbility>(StatusType::ALLOCATION_ERROR);
    }
    catch(StatusType e) {
        return output_t<NenAbility>(e);
    }
    return output_t<NenAbility>(ability);
}

StatusType Huntech::force_join(int forcingSquadId, int forcedSquadId) {
    int squadId1 = forcingSquadId;
    int squadId2 = forcedSquadId;
    if (squadId1 <= 0 || squadId2 <= 0 || squadId1 == squadId2)
        return StatusType::INVALID_INPUT;

    try {
        Squad& squad1 = squadsTree.search(squadId1);
        Squad& squad2 = squadsTree.search(squadId2);

        int root_1 = squad1.getUnionHead();
        int root_2 = squad2.getUnionHead();

        if (root_1 == -1)
            return StatusType::FAILURE;

        int Aura_1 = squad1.totalAura;

        if (root_2 != -1) {
            int exp_1 = huntersUnion.get_exp(root_1);
            int exp_2 = huntersUnion.get_exp(root_2);

            NenAbility ab_1 = huntersUnion.partialAbility(huntersUnion.lastChrono(root_1));
            NenAbility ab_2 = huntersUnion.partialAbility(huntersUnion.lastChrono(root_2));

            if ((long long)(exp_1 + squad1.totalAura + ab_1.getEffectiveNenAbility()) <=
                (long long)(exp_2 + squad2.totalAura + ab_2.getEffectiveNenAbility())) {
                return StatusType::FAILURE;
            }



            AuraKey key1(Aura_1, squadId1);
            squadsAuraTree.del(key1);
            squad1.totalAura += squad2.totalAura;
            squad1.totalNenAbility += squad2.totalNenAbility;

            try {
                AuraKey newKey1(squad1.totalAura, squadId1);
                squadsAuraTree.insert(newKey1, &squad1);
            }
            catch (...) {
                // rollback on failure
                squad1.totalAura -= squad2.totalAura;
                squad1.totalNenAbility -= squad2.totalNenAbility;
                squadsAuraTree.insert(key1, &squad1);
                throw;
            }


            huntersUnion.combine(root_1, root_2, 1);
            squad1.setUnionHead(huntersUnion.find(root_1));
        }

        AuraKey key2(squad2.totalAura, squadId2);
        squadsAuraTree.del(key2);
        squadsTree.del(squadId2);

        return StatusType::SUCCESS;
    }
    catch (bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
    catch(...) {
        return StatusType::FAILURE;
    }
}