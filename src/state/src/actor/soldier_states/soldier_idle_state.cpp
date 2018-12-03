/**
 * @file idle_state.cpp
 * Defines the soldier idle state
 */

#include "state/actor/soldier_states/soldier_idle_state.h"
#include "state/actor/soldier.h"
#include "state/actor/soldier_states/soldier_attack_state.h"
#include "state/actor/soldier_states/soldier_dead_state.h"
#include "state/actor/soldier_states/soldier_move_state.h"
#include "state/actor/soldier_states/soldier_pursuit_state.h"

namespace state {

SoldierIdleState::SoldierIdleState(Soldier *soldier)
    : SoldierState(SoldierStateName::IDLE, soldier) {}

void SoldierIdleState::Enter() {}

std::unique_ptr<IActorState> SoldierIdleState::Update() {
	// Check if the soldier is dead
	if (soldier->GetHp() == 0) {
		return std::make_unique<SoldierDeadState>(soldier);
	}

	// Check if the destination is set
	if (soldier->IsDestinationSet()) {
		return std::make_unique<SoldierMoveState>(soldier);
	}

	// Check if there's an attack target set
	if (soldier->IsAttackTargetSet()) {
		if (not soldier->IsAttackTargetInRange()) {
			return std::make_unique<SoldierPursuitState>(soldier);
		} else {
			return std::make_unique<SoldierAttackState>(soldier);
		}
	}

	return nullptr;
}

void SoldierIdleState::Exit() {}
} // namespace state
