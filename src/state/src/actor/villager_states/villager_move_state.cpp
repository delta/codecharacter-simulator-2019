/**
 * @file move_state.cpp
 * Defines the villager move state
 */

#include "state/actor/villager_states/villager_move_state.h"
#include "state/actor/villager.h"
#include "state/actor/villager_states/villager_attack_state.h"
#include "state/actor/villager_states/villager_build_state.h"
#include "state/actor/villager_states/villager_dead_state.h"
#include "state/actor/villager_states/villager_idle_state.h"
#include "state/actor/villager_states/villager_move_to_build_state.h"
#include "state/actor/villager_states/villager_pursuit_state.h"

namespace state {

VillagerMoveState::VillagerMoveState(Villager *villager)
    : VillagerState(VillagerStateName::MOVE, villager) {}

void VillagerMoveState::Enter() {}

std::unique_ptr<IActorState> VillagerMoveState::Update() {
	// Check if the villager is dead
	if (villager->GetHp() == 0) {
		return std::make_unique<VillagerDeadState>(villager);
	}

	// Check if the build target is set
	if (villager->IsBuildTargetSet()) {
		if (villager->IsBuildTargetInRange()) {
			return std::make_unique<VillagerBuildState>(villager);
		} else {
			return std::make_unique<VillagerMoveToBuildState>(villager);
		}
	}

	// Check if there's an attack target set
	if (villager->IsAttackTargetSet()) {
		if (villager->IsAttackTargetInRange()) {
			return std::make_unique<VillagerAttackState>(villager);
		} else {
			return std::make_unique<VillagerPursuitState>(villager);
		}
	}

	// Check if destination has been reached
	if (villager->GetPosition() == villager->GetDestination()) {
		return std::make_unique<VillagerIdleState>(villager);
	}

	// TODO: Move to next position of the villager

	return nullptr;
}

void VillagerMoveState::Exit() {
	// Unset the destination on exit
	villager->ClearDestination();
}
} // namespace state