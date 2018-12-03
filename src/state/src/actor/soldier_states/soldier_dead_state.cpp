/**
 * @file dead_state.cpp
 * Defines the soldier dead state
 */

#include "state/actor/soldier_states/soldier_dead_state.h"
#include "state/actor/soldier.h"

namespace state {

SoldierDeadState::SoldierDeadState(Soldier *soldier)
    : SoldierState(SoldierStateName::DEAD, soldier) {}

void SoldierDeadState::Enter() {}

std::unique_ptr<IActorState> SoldierDeadState::Update() { return nullptr; }

void SoldierDeadState::Exit() {}
} // namespace state
