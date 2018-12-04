/**
 * @file attack_state.cpp
 * Defines the villager attack state
 */

#include "state/actor/villager_states/villager_attack_state.h"
#include "state/actor/villager.h"
#include "state/actor/villager_states/villager_build_state.h"
#include "state/actor/villager_states/villager_dead_state.h"
#include "state/actor/villager_states/villager_idle_state.h"
#include "state/actor/villager_states/villager_move_state.h"
#include "state/actor/villager_states/villager_move_to_build_state.h"
#include "state/actor/villager_states/villager_pursuit_state.h"

namespace state {

VillagerAttackState::VillagerAttackState(Villager *villager)
    : VillagerState(VillagerStateName::ATTACK, villager) {}

void VillagerAttackState::Enter() {}

std::unique_ptr<IActorState> VillagerAttackState::Update() {
	// Check if the villager is dead
	if (villager->GetHp() == 0) {
		villager->SetBuildTarget(nullptr);
		return std::make_unique<VillagerDeadState>(villager);
	}

	// Check if the build target is set
	if (villager->IsBuildTargetSet()) {
		villager->SetAttackTarget(nullptr);
		if (villager->IsBuildTargetInRange()) {
			return std::make_unique<VillagerBuildState>(villager);
		} else {
			return std::make_unique<VillagerMoveToBuildState>(villager);
		}
	}

	// Check if the destination is set
	if (villager->IsDestinationSet()) {
		villager->SetAttackTarget(nullptr);
		return std::make_unique<VillagerMoveState>(villager);
	}

	// Check if the target is dead
	if (villager->GetAttackTarget()->GetLatestHp() == 0) {
		villager->SetAttackTarget(nullptr);
		return std::make_unique<VillagerIdleState>(villager);
	}

	// Check if the target is out of range
	if (not villager->IsAttackTargetInRange()) {
		return std::make_unique<VillagerPursuitState>(villager);
	}

	// Execute attack code
	// Inflict damage on opponent
	auto target = villager->GetAttackTarget();
	target->Damage(villager->GetAttackDamage());

	// Check if opponent is now dead
	if (target->GetLatestHp() == 0) {
		// Reward player for kill
		villager->GetGoldManager()->RewardKill(villager->GetAttackTarget());
	}

	return nullptr;
}

void VillagerAttackState::Exit() {}
} // namespace state