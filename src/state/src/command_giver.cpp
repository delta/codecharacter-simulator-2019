/**
 * @file command_giver.cpp
 * Defines the CommandGiver class
 */

#include <algorithm>
#include "state/command_giver.h"

using Vec2d = physics::Vector<int64_t>;

namespace state {

CommandGiver::CommandGiver() {}

CommandGiver::CommandGiver(ICommandTaker * state):state(state){}

void CommandGiver::AttackActor(PlayerId player_id, ActorId unit_id, ActorId enemy_actor_id){
	state->AttackActor(player_id, unit_id, enemy_actor_id);
}

void CommandGiver::CreateFactory(PlayerId player_id, ActorId villager_id, physics::Vector<int64_t> offset){
	state->CreateFactory(player_id, villager_id, offset);
}

void CommandGiver::BuildFactory(PlayerId player_id, ActorId villager_id, ActorId factory_id){
	state->BuildFactory(player_id, villager_id, factory_id);
}

void CommandGiver::MoveUnit(PlayerId player_id, ActorId actor_id, physics::Vector<int64_t> position) {
	state->MoveUnit(player_id, actor_id, position);
}

void CommandGiver::RunCommands(ICommandTaker *state, const std::array<player_state::State, 2> &player_states) {

	//Getting all the soldiers, factory and money
	auto state_soldiers  = state->GetSoldiers();
	auto state_villagers = state->GetVillagers();
	auto state_factories = state->GetFactories();

	//Iterating through the soldiers and assigning soldier tasks
	for(int player_id = 0; player_id < player_states.size(); ++player_id)
	{
		for(int64_t soldier_index = 0; soldier_index < player_states[player_id].soldiers.size(); ++soldier_index)
		{
		    //Getting soldier data from the player state
			auto const &soldiers = player_states[player_id].soldiers[soldier_index];

			bool is_attacking = soldiers.target != -1;
			bool is_moving    = (soldiers.destination != Vec2d(-1, -1));

			std::vector<bool> flags{is_attacking, is_moving};
			if(std::count(flags.begin(), flags.end(), true) > 1){
				//Need to log errors here for assigning 2 tasks to a soldier at a time
			}
			else
			{
				if(is_attacking) {
                    AttackActor(static_cast<PlayerId>(player_id), soldiers.id, soldiers.target);
                }
				else if (is_moving){
					MoveUnit(static_cast<PlayerId>(player_id), soldiers.id, soldiers.destination);
				}
			}
		}	
	}

	//Iterating through the villagers and assigning tasks
	for(int player_id = 0; player_id < player_states.size(); ++player_id){
		for(int64_t villager_index = 0; villager_index < state_villagers.size(); ++villager_index){
			//Checking for build targets
			auto const &villagers = player_states[player_id].villagers[villager_index];

			//Checking if a villager wants to build a new factory or wants to continue building a factory that already exists
			bool build_factory = villagers.build_position != Vec2d(-1, -1);
            bool new_factory   = false;
			int64_t factory_target = villagers.target_factory_id;

            //Iterating through the factories to check whether the factory already exists
			for(int64_t factory_index = 0; factory_index < state_factories.size(); ++factory_index){
				//Checking if the factory target id already exists. If it exists, then break and continue building factor. Else Create new factory
				if(state_factories[player_id][factory_index]->GetActorId() == factory_target){
					new_factory = true;
					break;
				}
			}

			//If villager is creating a new factory
			if(build_factory && new_factory){
				CreateFactory(static_cast<PlayerId>(player_id), villagers.id, villagers.build_position);
			}
			//If villager is building an existing factory
			else if(build_factory){
				BuildFactory(static_cast<PlayerId>(player_id), villagers.id, villagers.target_factory_id);
			}
		}
	}
}

} // namespace state

