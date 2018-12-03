/**
 * @file build_state.cpp
 * Defines the villager build state
 */

#include "state/actor/villager_states/villager_build_state.h"
#include "state/actor/villager.h"
#include "state/actor/villager_states/villager_attack_state.h"
#include "state/actor/villager_states/villager_dead_state.h"
#include "state/actor/villager_states/villager_idle_state.h"
#include "state/actor/villager_states/villager_move_state.h"
#include "state/actor/villager_states/villager_pursuit_state.h"

namespace state {

VillagerBuildState::VillagerBuildState(Villager *villager)
    : VillagerState(VillagerStateName::BUILD, villager) {}

void VillagerBuildState::Enter() {}

std::unique_ptr<IActorState> VillagerBuildState::Update() {
	// Check if the villager is dead
	if (villager->GetHp() == 0) {
		villager->SetBuildTarget(nullptr);
		return std::make_unique<VillagerDeadState>(villager);
	}

	// Check if the destination is set
	if (villager->IsDestinationSet()) {
		villager->SetBuildTarget(nullptr);
		return std::make_unique<VillagerMoveState>(villager);
	}

	// Check if the attack target is set
	if (villager->IsAttackTargetSet()) {
		villager->SetBuildTarget(nullptr);
		if (not villager->IsAttackTargetInRange()) {
			return std::make_unique<VillagerPursuitState>(villager);
		} else {
			return std::make_unique<VillagerAttackState>(villager);
		}
	}

	// Check if the build target is completed
	if (villager->GetBuildTarget()->IsConstructionComplete()) {
		villager->SetBuildTarget(nullptr);
		return std::make_unique<VillagerIdleState>(villager);
	}

	// Check if the build target is out of range
	if (not villager->IsBuildTargetInRange()) {
		// TODO : Set position of Villager towards build target
	} else {
		// Build target
		// Add Effort to target factory construction
		auto target = villager->GetBuildTarget();
		target->IncrementConstructionCompletion(villager->GetBuildEffort());
	}

	return nullptr;
}

void VillagerBuildState::Exit() {}
} // namespace state
