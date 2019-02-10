/**
 * @file villager_mine_state.cpp
 * Defines the villager mine state
 */

#include "state/actor/villager_states/villager_mine_state.h"
#include "state/actor/factory.h"
#include "state/actor/villager_states/villager_attack_state.h"
#include "state/actor/villager_states/villager_build_state.h"
#include "state/actor/villager_states/villager_dead_state.h"
#include "state/actor/villager_states/villager_idle_state.h"
#include "state/actor/villager_states/villager_move_state.h"
#include "state/actor/villager_states/villager_move_to_build_state.h"
#include "state/actor/villager_states/villager_move_to_mine_state.h"
#include "state/actor/villager_states/villager_pursuit_state.h"
#include "state/actor/villager_states/villager_state.h"

namespace state {

VillagerMineState::VillagerMineState(Villager *villager)
    : VillagerState(VillagerStateName::MINE, villager) {}

void VillagerMineState::Enter() {}

std::unique_ptr<IActorState> VillagerMineState::Update() {
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
	if (not villager->IsMineTargetInRange()) {
		return std::make_unique<VillagerMoveToMineState>(villager);
	}

	// Mine gold
	villager->GetGoldManager()->AddMineRequest(villager->GetPlayerId(),
	                                           villager->GetMineTarget());

	return nullptr;
}

void VillagerMineState::Exit() {}
} // namespace state
