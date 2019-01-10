/**
 * @file logger.cpp
 * Defines the logger class
 */

#include "logger/logger.h"
#include "state/interfaces/i_command_taker.h"

#include <string>

using namespace state;

namespace logger {

Logger::Logger(ICommandTaker *state, int64_t player_instruction_limit_turn,
               int64_t player_instruction_limit_game, int64_t soldier_max_hp,
               int64_t villager_max_hp, int64_t factory_max_hp)
    : state(state), turn_count(0), factory_logs(),
      logs(std::make_unique<proto::Game>()),
      instruction_counts(std::vector<int64_t>((int)PlayerId::PLAYER_COUNT, 0)),
      error_map(std::unordered_map<std::string, int64_t>()),
      current_error_code(0), errors(std::array<std::vector<int64_t>, 2>()),
      player_instruction_limit_turn(player_instruction_limit_turn),
      player_instruction_limit_game(player_instruction_limit_game),
      soldier_max_hp(soldier_max_hp), villager_max_hp(villager_max_hp),
      factory_max_hp(factory_max_hp) {}

proto::FactoryState GetProtoFactoryState(FactoryStateName factory_state,
                                         ActorType production_state) {
	proto::FactoryState curr_factory_state;
	switch (factory_state) {
	case FactoryStateName::UNBUILT:
		curr_factory_state = proto::FACTORY_UNBUILT;
		break;
	case FactoryStateName::IDLE:
		curr_factory_state = proto::FACTORY_IDLE;
		break;
	case FactoryStateName::PRODUCTION:
		if (production_state == ActorType::VILLAGER)
			curr_factory_state = proto::FACTORY_VILLAGER_PRODUCTION;
		else
			curr_factory_state = proto::FACTORY_SOLDIER_PRODUCTION;
		break;
	case FactoryStateName::DEAD:
		curr_factory_state = proto::FACTORY_DESTROYED;
		break;
	}
	return curr_factory_state;
}

FactoryState GetFactoryState(FactoryStateName factory_state,
                             ActorType production_state) {
	FactoryState curr_factory_state;
	switch (factory_state) {
	case FactoryStateName::UNBUILT:
		curr_factory_state = FACTORY_UNBUILT;
		break;
	case FactoryStateName::IDLE:
		curr_factory_state = FACTORY_IDLE;
		break;
	case FactoryStateName::PRODUCTION:
		if (production_state == ActorType::VILLAGER)
			curr_factory_state = FACTORY_VILLAGER_PRODUCTION;
		else
			curr_factory_state = FACTORY_SOLDIER_PRODUCTION;
		break;
	case FactoryStateName::DEAD:
		curr_factory_state = FACTORY_DESTROYED;
		break;
	}
	return curr_factory_state;
}

proto::TerrainType GetProtoTerrainType(TerrainType terrain) {
	proto::TerrainType terrain_proto;
	switch (terrain) {
	case TerrainType::LAND:
		terrain_proto = proto::LAND;
		break;
	case TerrainType::WATER:
		terrain_proto = proto::WATER;
		break;
	case TerrainType::GOLD_MINE:
		terrain_proto = proto::GOLD_MINE;
		break;
	}
	return terrain_proto;
}

void Logger::LogState() {
	turn_count++;
	auto *game_state = logs->add_states();

	auto soldiers = state->GetSoldiers();
	auto villagers = state->GetVillagers();
	auto factories = state->GetFactories();
	auto money = state->GetGold();

	// Things set only during the first turn.
	if (turn_count == 1) {
		// Set map properties
		auto const *map = state->GetMap();
		auto map_size = map->GetSize();
		auto map_element_size = map->GetElementSize();

		logs->set_map_size(map_size);
		logs->set_map_element_size(map_element_size);
		std::vector<proto::TerrainType> map_terrain;
		for (int i = 0; i < map_size; ++i) {
			for (int j = 0; j < map_size; ++j) {
				logs->add_map_elements(
				    GetProtoTerrainType(map->GetTerrainTypeByOffset(i, j)));
			}
		}

		// Set max HPs of actors
		logs->set_soldier_max_hp(this->soldier_max_hp);
		logs->set_villager_max_hp(this->villager_max_hp);
		logs->set_factory_max_hp(this->factory_max_hp);

		// Set factories and add actor log entires
		for (int i = 0; i < (int)PlayerId::PLAYER_COUNT; i++) {

			// For factories
			std::vector<FactoryLogEntry> player_factory_log_entry;
			for (auto const &factory : factories[i]) {
				proto::Factory *t_factory = game_state->add_factories();
				t_factory->set_id((int)factory->GetActorId());
				t_factory->set_player_id((int)factory->GetPlayerId());
				t_factory->set_hp(factory->GetHp());
				t_factory->set_x((int)factory->GetPosition().x);
				t_factory->set_y((int)factory->GetPosition().y);
				t_factory->set_state(GetProtoFactoryState(
				    factory->GetState(), factory->GetProductionState()));
				t_factory->set_build_percent(
				    (int)factory->GetConstructionCompletion());
				player_factory_log_entry.push_back(
				    {(int)factory->GetActorId(), factory->GetHp(),
				     (int)factory->GetConstructionCompletion(),
				     GetFactoryState(factory->GetState(),
				                     factory->GetProductionState())});
			}
			factory_logs[i] = player_factory_log_entry;
		}

		// Set instruction limit constants
		logs->set_inst_limit_turn(this->player_instruction_limit_turn);
		logs->set_inst_limit_game(this->player_instruction_limit_game);
	} else {
		// Things done in subsequent turns
		for (int i = 0; i < (int)PlayerId::PLAYER_COUNT; ++i) {
			int log_ptr = 0;
			int factory_ptr = 0;

			// Iterate through a player's factory list
			while (log_ptr < factory_logs[i].size() &&
			       factory_ptr < factories[i].size()) {

				auto curr_log = factory_logs[i][log_ptr];
				auto curr_factory = factories[i][factory_ptr];

				// Check if a factory exists in both lists
				if (curr_log.id == (int)curr_factory->GetActorId()) {

					FactoryState curr_factory_state =
					    GetFactoryState(curr_factory->GetState(),
					                    curr_factory->GetProductionState());

					// If it does, check if it's stats have changed
					if (curr_log.hp != curr_factory->GetHp() ||
					    curr_log.state != curr_factory_state ||
					    curr_log.build_percent !=
					        (int)curr_factory->GetConstructionCompletion()) {

						// An existing factory's stats have changed. Log it
						auto *t_factory = game_state->add_factories();
						t_factory->set_id((int)curr_factory->GetActorId());
						t_factory->set_player_id(i);
						t_factory->set_hp(curr_factory->GetHp());
						t_factory->set_build_percent(
						    (int)curr_factory->GetConstructionCompletion());
						t_factory->set_state(GetProtoFactoryState(
						    curr_factory->GetState(),
						    curr_factory->GetProductionState()));

						// Update the factory log as well
						factory_logs[i][log_ptr].hp = curr_factory->GetHp();
						factory_logs[i][log_ptr].state = curr_factory_state;
						factory_logs[i][log_ptr].build_percent =
						    (int)curr_factory->GetConstructionCompletion();
					}

					log_ptr++;
					factory_ptr++;

				} else if (curr_log.id < curr_factory->GetActorId()) {
					// If a factory dies, the main factory id will be greater

					// A factory was destroyed. Log this factory from the log.
					auto *t_factory = game_state->add_factories();
					t_factory->set_id(curr_log.id);
					t_factory->set_state(proto::FACTORY_DESTROYED);

					// Remove it from the factory log
					factory_logs[i].erase(factory_logs[i].begin() + log_ptr);

					// Don't log_ptr++, size of factory_logs already reduced

				} else {
					// The factory hasn't changed. Don't log anything.
					log_ptr++;
					factory_ptr++;
				}
			}

			// Remaining factories in the log list have been destroyed
			while (log_ptr < factory_logs[i].size()) {
				auto curr_log = factory_logs[i][log_ptr];

				// A factory was destroyed. Log it.
				auto *t_factory = game_state->add_factories();
				t_factory->set_id(curr_log.id);
				t_factory->set_state(proto::FACTORY_DESTROYED);

				// Remove it from the factory log
				factory_logs[i].erase(factory_logs[i].begin() + log_ptr);

				// Don't log_ptr++, size of factory_logs already reduced
			}

			// Remaining factories in the main list are newly built
			while (factory_ptr < factories[i].size()) {
				auto *curr_factory = factories[i][factory_ptr];

				// A newly built factory. Log it.
				auto *t_factory = game_state->add_factories();
				t_factory->set_id((int)curr_factory->GetActorId());
				t_factory->set_player_id((int)curr_factory->GetPlayerId());
				t_factory->set_hp(curr_factory->GetHp());
				t_factory->set_x((int)curr_factory->GetPosition().x);
				t_factory->set_y((int)curr_factory->GetPosition().y);
				t_factory->set_state(
				    GetProtoFactoryState(curr_factory->GetState(),
				                         curr_factory->GetProductionState()));
				t_factory->set_build_percent(
				    (int)curr_factory->GetConstructionCompletion());
				factory_logs[i].push_back(
				    {(int)curr_factory->GetActorId(), curr_factory->GetHp(),
				     (int)curr_factory->GetConstructionCompletion(),
				     GetFactoryState(curr_factory->GetState(),
				                     curr_factory->GetProductionState())});
				factory_ptr++;
			}
		}
	}

	// Things to be logged in all turns

	for (auto const &player_soldiers : soldiers) {
		for (auto const &soldier : player_soldiers) {
			proto::Soldier *t_soldier = game_state->add_soldiers();
			t_soldier->set_id((int)soldier->GetActorId());
			t_soldier->set_player_id((int)soldier->GetPlayerId());
			t_soldier->set_hp(soldier->GetHp());
			t_soldier->set_x((int)soldier->GetPosition().x);
			t_soldier->set_y((int)soldier->GetPosition().y);
			switch (soldier->GetState()) {
			case SoldierStateName::IDLE:
				t_soldier->set_state(proto::SOLDIER_IDLE);
				break;
			case SoldierStateName::MOVE:
			case SoldierStateName::PURSUIT:
				t_soldier->set_state(proto::SOLDIER_MOVE);
				break;
			case SoldierStateName::ATTACK:
				t_soldier->set_state(proto::SOLDIER_ATTACK);
				break;
			case SoldierStateName::DEAD:
				t_soldier->set_state(proto::SOLDIER_DEAD);
				break;
			}
			if (soldier->IsAttackTargetSet()) {
				auto target_pos = soldier->GetAttackTarget()->GetPosition();
				t_soldier->set_target_x(target_pos.x);
				t_soldier->set_target_y(target_pos.y);
			} else {
				t_soldier->set_target_x(-1);
				t_soldier->set_target_y(-1);
			}
		}
	}

	for (auto const &player_villagers : villagers) {
		for (auto const &villager : player_villagers) {
			proto::Villager *t_villager = game_state->add_villagers();
			t_villager->set_id((int)villager->GetActorId());
			t_villager->set_player_id((int)villager->GetPlayerId());
			t_villager->set_hp(villager->GetHp());
			t_villager->set_x((int)villager->GetPosition().x);
			t_villager->set_y((int)villager->GetPosition().y);
			switch (villager->GetState()) {
			case VillagerStateName::IDLE:
				t_villager->set_state(proto::VILLAGER_IDLE);
				break;
			case VillagerStateName::MOVE:
			case VillagerStateName::MOVE_TO_BUILD:
			case VillagerStateName::MOVE_TO_MINE:
			case VillagerStateName::PURSUIT:
				t_villager->set_state(proto::VILLAGER_MOVE);
				break;
			case VillagerStateName::ATTACK:
				t_villager->set_state(proto::VILLAGER_ATTACK);
				break;
			case VillagerStateName::MINE:
				t_villager->set_state(proto::VILLAGER_MINE);
				break;
			case VillagerStateName::BUILD:
				t_villager->set_state(proto::VILLAGER_BUILD);
				break;
			case VillagerStateName::DEAD:
				t_villager->set_state(proto::VILLAGER_DEAD);
				break;
			}
			if (villager->IsAttackTargetSet()) {
				auto target_pos = villager->GetAttackTarget()->GetPosition();
				t_villager->set_target_x(target_pos.x);
				t_villager->set_target_y(target_pos.y);
			} else if (villager->IsMineTargetSet()) {
				auto target_pos = villager->GetMineTarget();
				t_villager->set_target_x(target_pos.x);
				t_villager->set_target_y(target_pos.y);
			} else if (villager->IsBuildTargetSet()) {
				auto target_factory_pos =
				    villager->GetBuildTarget()->GetPosition();
				t_villager->set_target_x(target_factory_pos.x);
				t_villager->set_target_y(target_factory_pos.y);
			} else {
				t_villager->set_target_x(-1);
				t_villager->set_target_y(-1);
			}
		}
	}

	// Log player money
	for (auto player_money : money) {
		game_state->add_gold(player_money);
	}

	// Log instruction counts and reset temp counts to 0
	for (auto &inst_count : instruction_counts) {
		game_state->add_instruction_counts(inst_count);
		inst_count = 0;
	}

	// Log the errors, clear the error vectors
	for (auto &player_errors : errors) {
		auto player_error_struct = game_state->add_player_errors();
		for (auto error_code : player_errors) {
			player_error_struct->add_errors(error_code);
		}
		player_errors.clear();
	}
}

void Logger::LogInstructionCount(PlayerId player_id, int64_t count) {
	this->instruction_counts[(int)player_id] = count;
}

void Logger::LogError(state::PlayerId player_id, ErrorType error_type,
                      std::string message) {
	int64_t error_code;

	// Encode the message as "ERROR_TYPE: <message_string>"
	std::string full_message = ErrorTypeName[(int)error_type] + ": " + message;

	// If the message doesn't exist in the map, add an entry in the map and
	// increment the counter
	if (error_map.find(full_message) == error_map.end()) {
		error_code = current_error_code++;
		error_map[full_message] = error_code;
	} else {
		error_code = error_map[full_message];
	}

	errors[(int)player_id].push_back(error_code);
}

void Logger::LogFinalGameParams() {
	// Write the error mapping to logs
	// Flip the mapping, int error_code -> string message
	for (auto element : error_map) {
		(*logs->mutable_error_map())[element.second] = element.first;
	}
}

void Logger::WriteGame(std::ostream &write_stream) {
	logs->SerializeToOstream(&write_stream);
}
} // namespace logger
