/**
 * @file dead_state.cpp
 * Defines the villager dead state
 */

#include "state/actor/villager_states/villager_dead_state.h"
#include "state/actor/villager.h"

namespace state {

VillagerDeadState::VillagerDeadState(Villager *villager)
    : VillagerState(VillagerStateName::DEAD, villager) {}

void VillagerDeadState::Enter() {}

std::unique_ptr<IActorState> VillagerDeadState::Update() { return nullptr; }

void VillagerDeadState::Exit() {}
} // namespace state
