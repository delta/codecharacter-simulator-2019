/**
 * @file soldier_move_to_build_state.cpp
 * Defines the villager move_to_build state
 */

#include "state/actor/villager_states/villager_move_to_build_state.h"
#include "state/actor/villager.h"
#include "state/actor/villager_states/villager_attack_state.h"
#include "state/actor/villager_states/villager_build_state.h"
#include "state/actor/villager_states/villager_dead_state.h"
#include "state/actor/villager_states/villager_idle_state.h"
#include "state/actor/villager_states/villager_mine_state.h"
#include "state/actor/villager_states/villager_move_state.h"
#include "state/actor/villager_states/villager_move_to_mine_state.h"
#include "state/actor/villager_states/villager_pursuit_state.h"

namespace state {

VillagerMoveToBuildState::VillagerMoveToBuildState(Villager *villager)
    : VillagerState(VillagerStateName::MOVE_TO_BUILD, villager) {}

void VillagerMoveToBuildState::Enter() {}

std::unique_ptr<IActorState> VillagerMoveToBuildState::Update() {
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
		return std::make_unique<VillagerAttackState>(villager);
	}

	// Check if the mine target is set
	if (villager->IsMineTargetSet()) {
		villager->SetBuildTarget(nullptr);
		return std::make_unique<VillagerMineState>(villager);
	}

	// Check if the build target is completed
	if (villager->GetBuildTarget()->IsConstructionComplete()) {
		villager->SetBuildTarget(nullptr);
		return std::make_unique<VillagerIdleState>(villager);
	}

	// Check if the build target is out of range
	if (villager->IsBuildTargetInRange()) {
		return std::make_unique<VillagerBuildState>(villager);
	}

	// TODO : Set position of Villager towards build target

	return nullptr;
}

void VillagerMoveToBuildState::Exit() {}
} // namespace state
