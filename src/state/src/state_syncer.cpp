/**
 * @file state_syncer.cpp
 * Defines the StateSyncer class
 */

#include "state/state_syncer.h"

using Vec2D = physics::Vector<int64_t>;

namespace state {

StateSyncer::StateSyncer(ICommandGiver *command_giver, ICommandTaker *state)
    : command_giver(command_giver), state(state) {}

void StateSyncer::UpdateMainState(
    const std::array<player_state::State, 2> &player_states) {

	// Call the CommandGiver
	command_giver->RunCommands(this->state, player_states);

	// Update main state
	state->Update();
}

void StateSyncer::UpdatePlayerStates(
    std::array<player_state::State, 2> &player_states) {

	//Getting all information from the main state
	auto state_soldiers  = state->GetSoldiers();
	auto state_villagers = state->GetVillagers();
	auto state_factories = state->GetFactories();
	auto state_money     = state->GetGold();
	auto *map            = state->GetMap();

	//Creating the player map
	std::array<std::array<TerrainType, MAP_SIZE>, MAP_SIZE> player_map;
	for(int x = 0; x < map->GetSize(); ++x){
		std::array<TerrainType, MAP_SIZE> map_row;
		for(int y = 0; y < map->GetSize(); ++y){
			//Initializing the map element
			TerrainType map_element = map->GetTerrainTypeByOffset(x, y);
			map_row[y] = map_element;
		}
		std::move(map_row.begin(), map_row.end(), player_map[x].begin());
	}

	//Iterrating throught the players
	for(int64_t player_id = 0; player_id < player_states.size(); ++player_id){
		//Flipping the map if player 2 is playing 
		if(static_cast<PlayerId>(player_id) == PlayerId::PLAYER2){
			FlipMap(player_map);
		}

		//Changing map elements from type state::TerrainType to player_state::TerrainType
		std::array<std::array<player_state::TerrainType, MAP_SIZE>, MAP_SIZE> new_map;
		for(int i = 0; i < map->GetSize(); ++i){
			std::array<player_state::TerrainType, MAP_SIZE> &new_row = new_map[i];
			for(int j = 0; j < map->GetSize(); ++j){
				player_state::TerrainType &new_map_element = new_row[j];
				switch(player_map[i][j]){
					case TerrainType::LAND:
						new_map_element = player_state::TerrainType::LAND;
					break;
					case TerrainType::WATER:
						new_map_element = player_state::TerrainType::WATER;
					break;
					case TerrainType::GOLD_MINE:
						new_map_element = player_state::TerrainType::GOLD_MINE;
					break;
				}
			}
		}

		//Creating the enemy id
		int64_t enemy_id = (player_id + 1) % static_cast<int>(PlayerId::PLAYER_COUNT);

		//Assinging the default values and positions of the new player states
		AssignSoldierAttributes(player_id, player_states[player_id].soldiers, false);
		AssignSoldierAttributes(enemy_id, player_states[player_id].enemy_soldiers, true);
		AssignVillagerAttributes(player_id, player_states[player_id].villagers, false);
		AssignVillagerAttributes(enemy_id, player_states[player_id].enemy_villagers, true);
		AssignFactoryAttributes(player_id, player_states[player_id].factories, false);
		AssignFactoryAttributes(enemy_id, player_states[player_id].enemy_factories, true);

		//Assigning the gold for each player
		player_states[player_id].gold = state_money[player_id];

		//Assigning the map to the player states
		//Copying data for player 1
		if(static_cast<PlayerId>(player_id) == PlayerId::PLAYER1){
			for(int i = 0; i < map->GetSize(); ++i){
				std::copy(new_map[i].begin(), new_map[i].end(), player_states[player_id].map[i].begin());	
			}
		}
		//Moving data the map into player 2's state to save space
		else{
			for(int i = 0; i < map->GetSize(); ++i){
				std::move(new_map[i].begin(), new_map[i].end(), player_states[player_id].map[i].begin());
			}
		}

		//TODO: Assing gold mine locations in player states
	}
}

std::array<int64_t, 2> StateSyncer::GetScores() { return state->GetScores(); }

void StateSyncer::FlipMap(
    std::array<std::array<TerrainType, MAP_SIZE>, MAP_SIZE> &player_map) {
	// Flipping the map
	auto map_size = MAP_SIZE;
	for (int i = 0; i < map_size / 2; ++i) {
		for (int j = 0; j < map_size; ++j) {
			std::swap(player_map[i][j],
			          player_map[map_size - 1 - i][map_size - 1 - j]);
		}
	}
	// If the map size is odd, flipping only the middle row
	if (map_size % 2) {
		int i = int(map_size / 2);
		for (int j = 0; j < int(map_size / 2); ++j) {
			std::swap(player_map[i][j],
			          player_map[map_size - 1 - i][map_size - 1 - j]);
		}
	}
}

Vec2D StateSyncer::FlipPosition(const Map *map, Vec2D position){
    auto map_size = map->GetSize();
    auto map_element_size = map->GetElementSize();
	return Vec2D(map_size * map_element_size - 1 - position.x,
	             map_size * map_element_size - 1 - position.y);
}

void StateSyncer::AssignSoldierAttributes(
    int64_t id,
    std::vector<player_state::Soldier> &player_soldiers,
    bool is_enemy) {
	auto state_soldiers = state->GetSoldiers();
	int64_t player_id = id;
	const auto *map = state->GetMap();

	for (int i = 0; i < state_soldiers.size(); ++i) {
		// Reassinging id to all the soliders
		player_soldiers[i].id = state_soldiers[id][i]->GetActorId();

		// Defaulting all targets
		player_soldiers[i].destination = Vec2D(-1, -1);
		player_soldiers[i].target = -1;
		player_soldiers[i].hp = state_soldiers[id][i]->GetHp();

		// Assigning the soldier state
		switch (state_soldiers[id][i]->GetState()) {
		case SoldierStateName::IDLE:
			player_soldiers[i].state = player_state::SoldierState::IDLE;
			break;
		case SoldierStateName::MOVE:
		case SoldierStateName::PURSUIT:
			player_soldiers[i].state = player_state::SoldierState::MOVE;
			break;
		case SoldierStateName::ATTACK:
			player_soldiers[i].state = player_state::SoldierState::ATTACK;
			break;
		case SoldierStateName::DEAD:
			// Review this
			--i;
			continue;
			// Review this
		}

		// If is_enemy is true, then player_id is the opposite of the id
		if (is_enemy) {
			player_id = (id + 1) % static_cast<int>(PlayerId::PLAYER_COUNT);
		}

		// Player 1 is by default in the right orientation
		if (PlayerId::PLAYER1 == static_cast<PlayerId>(player_id)) {
			player_soldiers[i].position = state_soldiers[id][i]->GetPosition();
		}
		// Player 2 has a flipped orientation, so his positions need to be
		// flipped
		else {
			player_soldiers[i].position =
			    FlipPosition(map, state_soldiers[id][i]->GetPosition());
		}
	}
}

void StateSyncer::AssignVillagerAttributes(
    int64_t id,
    std::vector<player_state::Villager> &player_villagers,
    bool is_enemy) {
	// Getting all state villagers
	auto state_villagers = state->GetVillagers();
	int64_t player_id = id;
	const auto *map = state->GetMap();

	// Reassiging the villagers in player states
	for (int i = 0; i < state_villagers[id].size(); ++i) {
		// Reassigning the villager's basic attribites
		player_villagers[i].id = state_villagers[id][i]->GetActorId();
		player_villagers[i].hp = state_villagers[id][i]->GetHp();

		// Assigning default values for other attributes
		player_villagers[i].target = -1;
		player_villagers[i].destination = Vec2D(-1, -1);
		player_villagers[i].target_factory_id = -1;
		player_villagers[i].mine_target = Vec2D(-1, -1);
		player_villagers[i].build_position = Vec2D(-1, -1);
		player_villagers[i].build_factory_type =
		    player_state::FactoryProduction::VILLAGER;

		// Reassinging the villager state
		switch (state_villagers[id][i]->GetState()) {
		case VillagerStateName::IDLE:
			player_villagers[i].state = player_state::VillagerState::IDLE;
			break;
		case VillagerStateName::PURSUIT:
		case VillagerStateName::MOVE:
		case VillagerStateName::MOVE_TO_BUILD:
		case VillagerStateName::MOVE_TO_MINE:
			player_villagers[i].state = player_state::VillagerState::MOVE;
			break;
		case VillagerStateName::ATTACK:
			player_villagers[i].state = player_state::VillagerState::ATTACK;
			break;
		case VillagerStateName::BUILD:
			player_villagers[i].state = player_state::VillagerState::BUILD;
			break;
		case VillagerStateName::DEAD:
			// Review this
			--i;
			continue;
			// Review this
		}

		// PlayerId is the opposite of id if is_enemy
		if (is_enemy) {
			player_id = (id + 1) % static_cast<int>(PlayerId::PLAYER_COUNT);
		}

		// For player1, positions are correct
		if (static_cast<PlayerId>(player_id) == PlayerId::PLAYER1) {
			player_villagers[i].position =
			    state_villagers[id][i]->GetPosition();
		}
		// For player 2, we must flip the position
		else {
			player_villagers[i].position =
			    FlipPosition(map, state_villagers[id][i]->GetPosition());
		}
	}
}

void StateSyncer::AssignFactoryAttributes(
    int64_t id,
    std::vector<player_state::Factory> &player_factories,
    bool is_enemy) {
	// Getting all the factories for the state
	auto state_factories = state->GetFactories();
	int64_t player_id = id;
	const auto *map = state->GetMap();

	for (int64_t i = 0; i < state_factories[id].size(); ++i) {
		// Reassinging basic attributes
		player_factories[i].id = state_factories[id][i]->GetActorId();
		player_factories[i].hp = state_factories[id][i]->GetHp();

		// Assigning default values to all other quantities
		player_factories[i].build_percent = 0;
		player_factories[i].built = false;
		player_factories[i].stopped = false;
		player_factories[i]._suicide = false;
		player_factories[i].production_state =
		        player_state::FactoryProduction::VILLAGER;

		//Checking if the state is dead
        auto factory_state = state_factories[id][i]->GetState();
        if(factory_state == FactoryStateName::DEAD){
            //Review this
            --i;
            continue;
            //Review this
        }

		// Reassinging the factory state
		switch (factory_state) {
		case FactoryStateName::UNBUILT:
			player_factories[i].state = player_state::FactoryState::UNBUILT;
			break;
		case FactoryStateName::IDLE:
			player_factories[i].state = player_state::FactoryState::IDLE;
			break;
		case FactoryStateName::PRODUCTION:
			// Finding whether the factory produces
			auto production_state =
			    state_factories[id][i]->GetProductionState();
			if (production_state == ActorType::FACTORY_SOLDIER) {
				player_factories[i].state =
				    player_state::FactoryState::VILLAGER_PRODUCTION;
			} else {
				player_factories[i].state =
				    player_state::FactoryState::SOLDIER_PRODUCTION;
			}
			break;
		}

		// Switching the player if is_enemy
		if (is_enemy) {
			player_id = (id + 1) % static_cast<int>(PlayerId::PLAYER_COUNT);
		}

		// Assinging the position
		if (static_cast<PlayerId>(player_id) == PlayerId::PLAYER1) {
			player_factories[i].position =
			    state_factories[id][i]->GetPosition();
		} else {
			player_factories[i].position =
			    FlipPosition(map, state_factories[id][i]->GetPosition());
		}
	}
}

} // namespace state
