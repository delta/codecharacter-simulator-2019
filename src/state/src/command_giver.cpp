/**
 * @file command_giver.cpp
 * Defines the CommandGiver class
 */

#include "state/command_giver.h"
#include <algorithm>

using Vec2d = physics::Vector<int64_t>;

namespace state {

CommandGiver::CommandGiver() {}

CommandGiver::CommandGiver(ICommandTaker *state) : state(state) {}

void CommandGiver::AttackActor(PlayerId player_id, ActorId unit_id,
                               ActorId enemy_actor_id) {
	state->AttackActor(player_id, unit_id, enemy_actor_id);
}

void CommandGiver::CreateFactory(PlayerId player_id, ActorId villager_id,
                                 physics::Vector<int64_t> offset) {
	state->CreateFactory(player_id, villager_id, offset);
}

void CommandGiver::BuildFactory(PlayerId player_id, ActorId villager_id,
                                ActorId factory_id) {
	state->BuildFactory(player_id, villager_id, factory_id);
}

void CommandGiver::MoveUnit(PlayerId player_id, ActorId actor_id,
                            physics::Vector<int64_t> position) {
	state->MoveUnit(player_id, actor_id, position);
}

void CommandGiver::RunCommands(
    ICommandTaker *state,
    const std::array<player_state::State, 2> &player_states) {

	// Getting all the soldiers, factory and villagers
	auto state_soldiers = state->GetSoldiers();
	auto state_villagers = state->GetVillagers();
	auto state_factories = state->GetFactories();

	// Iterating through the soldiers and assigning soldier tasks
	for (int player_id = 0; player_id < player_states.size(); ++player_id) {
		for (auto &soldier : player_states[player_id].soldiers) {
			bool is_attacking = soldier.target != -1;
			bool is_moving = (soldier.destination != Vec2d(-1, -1));

			if (is_attacking && is_moving) {
				// TODO: Need to log errors here for assigning 2 tasks to a
				// soldier at a time
			} else {
				if (is_attacking) {
					AttackActor(static_cast<PlayerId>(player_id), soldier.id,
					            soldier.target);
				} else if (is_moving) {
					MoveUnit(static_cast<PlayerId>(player_id), soldier.id,
					         soldier.destination);
				}
			}
		}
	}

	// Iterating through the villagers and assigning tasks
	for (int player_id = 0; player_id < player_states.size(); ++player_id) {
		for (auto &villager : player_states[player_id].villagers) {
			// Checking if a villager wants to build a new factory or wants to
			// continue building a factory that already exists
			bool build_factory = villager.build_position != Vec2d(-1, -1);
			bool targetting_factory = villager.target_factory_id != -1;
			bool is_moving = villager.destination != Vec2d(-1, -1);
			bool is_attacking = villager.target != -1;

			std::vector<bool> checks{build_factory, targetting_factory,
			                         is_moving, is_attacking};

			if (std::count(checks.begin(), checks.end(), true) > 1) {
				// TODO: Need to log error because the villager is trying to do
				// 2 things at the same time
			}

			bool new_factory = true;
			int64_t factory_target = villager.target_factory_id;

			// Iterating through the factories to check whether the factory
			// already exists
			for (auto &factory : state_factories[player_id]) {
				if (factory->GetActorId() == factory_target) {
					new_factory = false;
					break;
				}
			}

			// If villager is creating a new factory
			if (build_factory && new_factory) {
				CreateFactory(static_cast<PlayerId>(player_id), villager.id,
				              villager.build_position);
			}

			// If villager is building an existing factory
			else if (build_factory) {
				BuildFactory(static_cast<PlayerId>(player_id), villager.id,
				             villager.target_factory_id);
			}

			// If the villager is moving to another destination
			else if (is_moving) {
				MoveUnit(static_cast<PlayerId>(player_id), villager.id,
				         villager.destination);
			}

			// If the villager is attacking another unit
			else if (is_attacking) {
				AttackActor(static_cast<PlayerId>(player_id), villager.id,
				            villager.target);
			}
		}
	}
}

} // namespace state
