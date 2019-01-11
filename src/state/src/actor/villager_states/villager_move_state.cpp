/**
 * @file move_state.cpp
 * Defines the villager move state
 */

#include "state/actor/villager_states/villager_move_state.h"
#include "state/actor/factory.h"
#include "state/actor/villager_states/villager_attack_state.h"
#include "state/actor/villager_states/villager_build_state.h"
#include "state/actor/villager_states/villager_dead_state.h"
#include "state/actor/villager_states/villager_idle_state.h"
#include "state/actor/villager_states/villager_mine_state.h"
#include "state/actor/villager_states/villager_move_to_build_state.h"
#include "state/actor/villager_states/villager_move_to_mine_state.h"
#include "state/actor/villager_states/villager_pursuit_state.h"
#include "state/actor/villager_states/villager_state.h"

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
		return std::make_unique<VillagerBuildState>(villager);
	}

	// Check if there's an attack target set
	if (villager->IsAttackTargetSet()) {
		return std::make_unique<VillagerAttackState>(villager);
	}

	// Check if there's a mine target set
	if (villager->IsMineTargetSet()) {
		return std::make_unique<VillagerMineState>(villager);
	}

	// Check if destination has been reached
	if (villager->GetPosition() == villager->GetDestination()) {
		return std::make_unique<VillagerIdleState>(villager);
	}

	auto path_planner = villager->GetPathPlanner();
	auto current_position = villager->GetPosition();
	auto destination = villager->GetDestination();
	auto speed = villager->GetSpeed();

	auto next_position =
	    path_planner->GetNextPosition(current_position, destination, speed);

	villager->SetNewPosition(next_position);

	return nullptr;
}

void VillagerMoveState::Exit() {
	// Unset the destination on exit
	villager->ClearDestination();
}
} // namespace state
