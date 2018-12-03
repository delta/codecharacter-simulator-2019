/**
 * @file pursuit_state.cpp
 * Defines the villager pursuit state
 */

#include "state/actor/villager_states/villager_pursuit_state.h"
#include "state/actor/villager.h"
#include "state/actor/villager_states/villager_attack_state.h"
#include "state/actor/villager_states/villager_build_state.h"
#include "state/actor/villager_states/villager_dead_state.h"
#include "state/actor/villager_states/villager_idle_state.h"
#include "state/actor/villager_states/villager_move_state.h"

namespace state {

VillagerPursuitState::VillagerPursuitState(Villager *villager)
    : VillagerState(VillagerStateName::PURSUIT, villager) {}

void VillagerPursuitState::Enter() {}

std::unique_ptr<IActorState> VillagerPursuitState::Update() {
	// Check if the villager is dead
	if (villager->GetHp() == 0) {
		villager->SetAttackTarget(nullptr);
		return std::make_unique<VillagerDeadState>(villager);
	}

	// Check if destination is set
	if (villager->IsDestinationSet()) {
		villager->SetAttackTarget(nullptr);
		return std::make_unique<VillagerMoveState>(villager);
	}

	// Check if the build target is set
	if (villager->IsBuildTargetSet()) {
		villager->SetAttackTarget(nullptr);
		return std::make_unique<VillagerBuildState>(villager);
	}

	// Check if the target is dead
	if (villager->GetAttackTarget()->GetLatestHp() == 0) {
		villager->SetAttackTarget(nullptr);
		return std::make_unique<VillagerIdleState>(villager);
	}

	// Check if target in range
	if (villager->IsAttackTargetInRange()) {
		return std::make_unique<VillagerAttackState>(villager);
	}

	// TODO: Set position of villager to next position

	return nullptr;
}

void VillagerPursuitState::Exit() {}
} // namespace state
