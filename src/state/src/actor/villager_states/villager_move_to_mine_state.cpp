/**
 * @file soldier_move_to_mine_state.cpp
 * Defines the villager move_to_mine state
 */

#include "state/actor/villager_states/villager_move_to_mine_state.h"
#include "state/actor/factory.h"
#include "state/actor/villager_states/villager_attack_state.h"
#include "state/actor/villager_states/villager_build_state.h"
#include "state/actor/villager_states/villager_dead_state.h"
#include "state/actor/villager_states/villager_idle_state.h"
#include "state/actor/villager_states/villager_mine_state.h"
#include "state/actor/villager_states/villager_move_state.h"
#include "state/actor/villager_states/villager_move_to_build_state.h"
#include "state/actor/villager_states/villager_pursuit_state.h"
#include "state/actor/villager_states/villager_state.h"

namespace state {

VillagerMoveToMineState::VillagerMoveToMineState(Villager *villager)
    : VillagerState(VillagerStateName::MOVE_TO_MINE, villager) {}

void VillagerMoveToMineState::Enter() {}

std::unique_ptr<IActorState> VillagerMoveToMineState::Update() {
	// Check if the villager is dead
	if (villager->GetHp() == 0) {
		villager->ClearMineTarget();
		return std::make_unique<VillagerDeadState>(villager);
	}

	// Check if the destination is set
	if (villager->IsDestinationSet()) {
		villager->ClearMineTarget();
		return std::make_unique<VillagerMoveState>(villager);
	}

	// Check if the attack target is set
	if (villager->IsAttackTargetSet()) {
		villager->ClearMineTarget();
		return std::make_unique<VillagerAttackState>(villager);
	}

	// Check if the build target is set
	if (villager->IsBuildTargetSet()) {
		villager->ClearMineTarget();
		return std::make_unique<VillagerBuildState>(villager);
	}

	// Check if the mine target is in range
	if (villager->IsMineTargetInRange()) {
		return std::make_unique<VillagerMineState>(villager);
	}

	auto path_planner = villager->GetPathPlanner();
	auto current_position = villager->GetPosition();
	auto destination = villager->GetMineTarget().to_double();
	auto speed = villager->GetSpeed();

	auto next_position =
	    path_planner->GetNextPosition(current_position, destination, speed);

	villager->SetNewPosition(next_position);

	return nullptr;
}

void VillagerMoveToMineState::Exit() {}
} // namespace state
