/**
 * @file idle_state.cpp
 * Defines the villager idle state
 */

#include "state/actor/villager_states/villager_idle_state.h"
#include "state/actor/villager.h"
#include "state/actor/villager_states/villager_attack_state.h"
#include "state/actor/villager_states/villager_build_state.h"
#include "state/actor/villager_states/villager_dead_state.h"
#include "state/actor/villager_states/villager_move_state.h"
#include "state/actor/villager_states/villager_pursuit_state.h"

namespace state {

VillagerIdleState::VillagerIdleState(Villager *villager)
    : VillagerState(VillagerStateName::IDLE, villager) {}

void VillagerIdleState::Enter() {}

std::unique_ptr<IActorState> VillagerIdleState::Update() {
	// Check if the villager is dead
	if (villager->GetHp() == 0) {
		return std::make_unique<VillagerDeadState>(villager);
	}

	// Check if the destination is set
	if (villager->IsDestinationSet()) {
		return std::make_unique<VillagerMoveState>(villager);
	}

	// Check if the build target is set
	if (villager->IsBuildTargetSet()) {
		return std::make_unique<VillagerBuildState>(villager);
	}

	// Check if there's an attack target set
	if (villager->IsAttackTargetSet()) {
		if (not villager->IsAttackTargetInRange()) {
			return std::make_unique<VillagerPursuitState>(villager);
		} else {
			return std::make_unique<VillagerAttackState>(villager);
		}
	}

	return nullptr;
}

void VillagerIdleState::Exit() {}
} // namespace state
