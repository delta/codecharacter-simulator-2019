/**
 * @file villager_state.cpp
 * Defines the base state that VillagerStates inherit from
 */

#include "state/actor/villager_states/villager_state.h"
#include "state/actor/villager.h"

namespace state {

VillagerState::VillagerState(VillagerStateName state_name, Villager *villager)
    : state_name(state_name), villager(villager) {}

VillagerStateName VillagerState::GetName() { return state_name; }
} // namespace state
