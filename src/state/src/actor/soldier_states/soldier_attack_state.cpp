/**
 * @file attack_state.cpp
 * Defines the soldier attack state
 */

#include "state/actor/soldier_states/soldier_attack_state.h"
#include "state/actor/soldier.h"
#include "state/actor/soldier_states/soldier_dead_state.h"
#include "state/actor/soldier_states/soldier_idle_state.h"
#include "state/actor/soldier_states/soldier_move_state.h"
#include "state/actor/soldier_states/soldier_pursuit_state.h"

namespace state {

SoldierAttackState::SoldierAttackState(Soldier *soldier)
    : SoldierState(SoldierStateName::ATTACK, soldier) {}

void SoldierAttackState::Enter() {}

std::unique_ptr<IActorState> SoldierAttackState::Update() {
	// Check if the soldier is dead
	if (soldier->GetHp() == 0) {
		soldier->SetAttackTarget(nullptr);
		return std::make_unique<SoldierDeadState>(soldier);
	}

	// Check if the destination is set
	if (soldier->IsDestinationSet()) {
		soldier->SetAttackTarget(nullptr);
		return std::make_unique<SoldierMoveState>(soldier);
	}

	// Check if the target is dead
	auto target = soldier->GetAttackTarget();
	if (target == nullptr || target->GetLatestHp() == 0) {
		soldier->SetAttackTarget(nullptr);
		return std::make_unique<SoldierIdleState>(soldier);
	}

	// Check if the target is out of range
	if (not soldier->IsAttackTargetInRange()) {
		return std::make_unique<SoldierPursuitState>(soldier);
	}

	// Execute attack code
	// Inflict damage on opponent
	target->Damage(soldier->GetAttackDamage());

	// Check if opponent is now dead
	if (target->GetLatestHp() == 0) {
		// Reward player for kill
		soldier->GetGoldManager()->RewardKill(target);
	}

	return nullptr;
} // namespace state

void SoldierAttackState::Exit() {}
} // namespace state
