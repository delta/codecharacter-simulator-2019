/**
 * @file state_syncer.cpp
 * Defines the StateSyncer class
 */

#include "state/state_syncer.h"

#include <algorithm>
#include <cmath>

namespace state {

StateSyncer::StateSyncer(std::unique_ptr<ICommandGiver> command_giver,
                         std::unique_ptr<ICommandTaker> state,
                         logger::ILogger *logger)
    : command_giver(std::move(command_giver)), state(std::move(state)),
      logger(logger) {}

void StateSyncer::UpdateMainState(
    const std::array<player_state::State, 2> &player_states,
    std::array<bool, 2> skip_player_turn) {

	// Call the CommandGiver
	command_giver->RunCommands(player_states, skip_player_turn);

	// Update main state
	state->Update();
}

// Note: id incicates the player's id whose units are being manipulated
// player_id indicates the player whose state is being updated
int64_t StateSyncer::GetPlayerId(int player_id, bool is_enemy) {
	int64_t new_player_id;
	if (is_enemy) {
		new_player_id =
		    (player_id + 1) % static_cast<int64_t>(PlayerId::PLAYER_COUNT);
	} else {
		new_player_id = static_cast<int64_t>(player_id);
	}
	return new_player_id;
}

void StateSyncer::UpdatePlayerStates(
    std::array<player_state::State, 2> &player_states) {

	// Getting all information from the main state
	auto state_soldiers = state->GetSoldiers();
	auto state_villagers = state->GetVillagers();
	auto state_factories = state->GetFactories();
	auto state_money = state->GetGold();
	auto *map = state->GetMap();

	// Iterating through the players
	for (int64_t player_id = 0; player_id < player_states.size(); ++player_id) {
		// Changing map elements from type state::TerrainType to
		// player_state::TerrainType
		std::array<std::array<player_state::TerrainType, MAP_SIZE>, MAP_SIZE>
		    new_map;
		// Storing the gold mine locations
		std::vector<Vec2D> gold_mine_offsets;
		for (int i = 0; i < map->GetSize(); ++i) {
			for (int j = 0; j < map->GetSize(); ++j) {
				auto &new_map_element = new_map[i][j];
				switch (map->GetTerrainTypeByOffset(i, j)) {
				case TerrainType::LAND:
					new_map_element = player_state::TerrainType::LAND;
					break;
				case TerrainType::WATER:
					new_map_element = player_state::TerrainType::WATER;
					break;
				// Saving the gold mine locations in a vector to add into player
				// state
				case TerrainType::GOLD_MINE:
					new_map_element = player_state::TerrainType::GOLD_MINE;
					Vec2D gold_mine;
					if (static_cast<PlayerId>(player_id) != PlayerId::PLAYER1) {
						gold_mine = Vec2D(map->GetSize() - 1 - i,
						                  map->GetSize() - 1 - j);
					} else {
						gold_mine = Vec2D(i, j);
					}
					gold_mine_offsets.push_back(gold_mine);
					break;
				}
			}
		}

		// Creating the enemy id
		int64_t enemy_id = GetPlayerId(player_id, true);

		// Assinging the default values and positions of the new player states
		AssignSoldierAttributes(player_id, player_states[player_id].soldiers,
		                        false);
		AssignSoldierAttributes(enemy_id,
		                        player_states[player_id].enemy_soldiers, true);
		AssignVillagerAttributes(player_id, player_states[player_id].villagers,
		                         false);
		AssignVillagerAttributes(
		    enemy_id, player_states[player_id].enemy_villagers, true);
		AssignFactoryAttributes(player_id, player_states[player_id].factories,
		                        false);
		AssignFactoryAttributes(enemy_id,
		                        player_states[player_id].enemy_factories, true);
		// Assigning the gold for each player
		player_states[player_id].gold = state_money[player_id];
		// Assigning the map to the player states
		// Copying data for player 1
		if (static_cast<PlayerId>(player_id) == PlayerId::PLAYER1) {
			for (int i = 0; i < map->GetSize(); ++i) {
				std::copy(new_map[i].begin(), new_map[i].end(),
				          player_states[player_id].map[i].begin());
			}
		}
		// Moving data the map into player 2's state to save space
		else {
			// Flipping the map
			for (int i = 0; i < map->GetSize(); ++i) {
				for (int j = 0; j < map->GetSize(); ++j) {
					player_states[player_id].map[i][j] =
					    new_map[map->GetSize() - 1 - i][map->GetSize() - 1 - j];
				}
			}
		}

		// Assigning the gold mine_locations to the player state
		if (static_cast<PlayerId>(player_id) == PlayerId::PLAYER1) {
			player_states[player_id].gold_mine_offsets =
			    std::move(gold_mine_offsets);
		} else {
			// Reverse the list for Player2
			std::reverse(gold_mine_offsets.begin(), gold_mine_offsets.end());
			player_states[player_id].gold_mine_offsets =
			    std::move(gold_mine_offsets);
		}
	}

	// Log the current state
	logger->LogState();
}

std::array<int64_t, 2> StateSyncer::GetScores(bool game_over) {
	return state->GetScores(game_over);
}

int64_t StateSyncer::GetInterestingness() {
	return state->GetInterestingness();
}

DoubleVec2D StateSyncer::FlipPosition(const Map *map, DoubleVec2D position) {
	auto map_size = map->GetSize();
	auto map_element_size = map->GetElementSize();
	return DoubleVec2D(map_size * map_element_size - 1 - position.x,
	                   map_size * map_element_size - 1 - position.y);
}

void StateSyncer::AssignSoldierAttributes(
    int64_t id, std::vector<player_state::Soldier> &player_soldiers,
    bool is_enemy) {
	auto state_soldiers = state->GetSoldiers();
	int64_t player_id = id;
	const auto *map = state->GetMap();
	std::vector<player_state::Soldier> new_soldiers;

	for (int i = 0; i < state_soldiers[id].size(); ++i) {
		// Reassinging id to all the soliders
		player_state::Soldier new_soldier;
		new_soldier.id = state_soldiers[id][i]->GetActorId();

		// Defaulting all targets
		new_soldier.destination = Vec2D::null;
		new_soldier.target = -1;
		new_soldier.hp = state_soldiers[id][i]->GetHp();
		new_soldier.age = state_soldiers[id][i]->GetAge();

		// Assigning the soldier state
		switch (state_soldiers[id][i]->GetState()) {
		case SoldierStateName::IDLE:
			new_soldier.state = player_state::SoldierState::IDLE;
			break;
		case SoldierStateName::MOVE:
		case SoldierStateName::PURSUIT:
			new_soldier.state = player_state::SoldierState::MOVE;
			break;
		case SoldierStateName::ATTACK:
			new_soldier.state = player_state::SoldierState::ATTACK;
			break;
		case SoldierStateName::DEAD:
			break;
		}

		// If is_enemy is true, then player_id is the opposite of the id
		player_id = GetPlayerId(id, is_enemy);

		// Player 1 is by default in the right orientation
		if (PlayerId::PLAYER1 == static_cast<PlayerId>(player_id)) {
			new_soldier.position =
			    state_soldiers[id][i]->GetPosition().to_int();
		}
		// Player 2 has a flipped orientation, so his positions need to be
		// flipped
		else {
			new_soldier.position =
			    FlipPosition(map, state_soldiers[id][i]->GetPosition())
			        .to_int();
		}

		// Adding new soldier to the vector new_soldiers
		new_soldiers.push_back(new_soldier);
	}

	// Moving new_soldiers into player_soldiers
	player_soldiers.clear();

	for (auto soldier : new_soldiers) {
		player_soldiers.push_back(soldier);
	}
}

void StateSyncer::AssignVillagerAttributes(
    int64_t id, std::vector<player_state::Villager> &player_villagers,
    bool is_enemy) {
	// Getting all state villagers
	auto state_villagers = state->GetVillagers();
	int64_t player_id = GetPlayerId(id, is_enemy);
	const auto *map = state->GetMap();
	std::vector<player_state::Villager> new_villagers;

	// Reassiging the villagers in player states
	for (int i = 0; i < state_villagers[id].size(); ++i) {
		// Reassigning the villager's basic attribites
		player_state::Villager new_villager;
		new_villager.id = state_villagers[id][i]->GetActorId();
		new_villager.hp = state_villagers[id][i]->GetHp();
		new_villager.age = state_villagers[id][i]->GetAge();

		// Assigning default values for other attributes
		new_villager.target = -1;
		new_villager.destination = Vec2D::null;
		new_villager.target_factory_id = -1;
		new_villager.mine_target = Vec2D::null;
		new_villager.build_offset = Vec2D::null;
		new_villager.build_factory_type =
		    player_state::FactoryProduction::VILLAGER;

		// Reassinging the villager state
		switch (state_villagers[id][i]->GetState()) {
		case VillagerStateName::IDLE:
			new_villager.state = player_state::VillagerState::IDLE;
			break;
		case VillagerStateName::PURSUIT:
		case VillagerStateName::MOVE:
		case VillagerStateName::MOVE_TO_BUILD:
		case VillagerStateName::MOVE_TO_MINE:
			new_villager.state = player_state::VillagerState::MOVE;
			break;
		case VillagerStateName::ATTACK:
			new_villager.state = player_state::VillagerState::ATTACK;
			break;
		case VillagerStateName::BUILD:
			new_villager.state = player_state::VillagerState::BUILD;
			break;
		case VillagerStateName::MINE:
			new_villager.state = player_state::VillagerState::MINE;
			break;
		case VillagerStateName::DEAD:
			break;
		}

		// For player1, positions are correct
		if (static_cast<PlayerId>(player_id) == PlayerId::PLAYER1) {
			new_villager.position =
			    state_villagers[id][i]->GetPosition().to_int();
		}
		// For player 2, we must flip the position
		else {
			new_villager.position =
			    FlipPosition(map, state_villagers[id][i]->GetPosition())
			        .to_int();
		}

		// Pushing new_villager into new_villagers
		new_villagers.push_back(new_villager);
	}

	// Moving new_villagers into player_villagers
	player_villagers.clear();
	for (auto villager : new_villagers) {
		player_villagers.push_back(villager);
	}
}

void StateSyncer::AssignFactoryAttributes(
    int64_t id, std::vector<player_state::Factory> &player_factories,
    bool is_enemy) {
	// Getting all the factories for the state
	auto state_factories = state->GetFactories();
	int64_t player_id = GetPlayerId(id, is_enemy);
	const auto *map = state->GetMap();
	std::vector<player_state::Factory> new_factories;

	for (int64_t i = 0; i < state_factories[id].size(); ++i) {
		auto state_factory = state_factories[id][i];

		// Reassinging basic attributes
		player_state::Factory new_factory;
		new_factory.id = state_factory->GetActorId();
		new_factory.hp = state_factory->GetHp();
		new_factory.age = state_factory->GetAge();

		// Get construction details
		auto current = (double)state_factory->GetConstructionCompletion();
		auto total = (double)state_factory->GetTotalConstructionCompletion();
		new_factory.build_percent = floor((current / total) * 100);
		new_factory.built = current == total;

		// Get production details
		new_factory.stopped = state_factory->IsStopped();

		// Checking if the state is dead
		auto factory_state = state_factory->GetState();
		auto production_state = state_factory->GetProductionState();

		// Reassinging the factory state
		switch (factory_state) {
		case FactoryStateName::DEAD:
			break;
		case FactoryStateName::UNBUILT:
			new_factory.state = player_state::FactoryState::UNBUILT;
			break;
		case FactoryStateName::IDLE:
			new_factory.state = player_state::FactoryState::IDLE;
			break;
		case FactoryStateName::PRODUCTION:
			new_factory.state = player_state::FactoryState::PRODUCTION;
			break;
		}

		// Reassigning production state
		switch (production_state) {
		case ActorType::SOLDIER:
			new_factory.production_state =
			    player_state::FactoryProduction::SOLDIER;
			break;
		case ActorType::VILLAGER:
			new_factory.production_state =
			    player_state::FactoryProduction::VILLAGER;
			break;
		}

		// Assigning the position
		if (static_cast<PlayerId>(player_id) == PlayerId::PLAYER1) {
			new_factory.position = state_factory->GetPosition().to_int();
		} else {
			// First flipping the offset and then assinging position
			Vec2D old_position = state_factory->GetPosition().to_int();
			auto element_size = map->GetElementSize();
			auto offset_x = old_position.x / element_size;
			auto offset_y = old_position.y / element_size;

			// Flipping the offset
			offset_x = map->GetSize() - 1 - offset_x;
			offset_y = map->GetSize() - 1 - offset_y;

			// Converting the offset to a position again
			auto new_pos_x = offset_x * element_size + (element_size / 2);
			auto new_pos_y = offset_y * element_size + (element_size / 2);
			new_factory.position = Vec2D(new_pos_x, new_pos_y);
		}

		// Pushing the factory into the new_factories vector
		new_factories.push_back(new_factory);
	}

	// Moving new_factories into player_factories
	player_factories.clear();
	for (auto factory : new_factories) {
		player_factories.push_back(factory);
	}
}

bool StateSyncer::IsGameOver(PlayerId &winner) {
	return state->IsGameOver(winner);
}

} // namespace state
