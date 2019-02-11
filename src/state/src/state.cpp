/**
 * @file state.cpp
 * Definitions for functions of the State class
 */

#include "state/state.h"

#include <algorithm>

namespace state {

State::State(std::unique_ptr<Map> map,
             std::unique_ptr<GoldManager> gold_manager,
             std::unique_ptr<PathPlanner> path_planner,
             std::array<std::vector<std::unique_ptr<Soldier>>, 2> soldiers,
             std::array<std::vector<std::unique_ptr<Villager>>, 2> villagers,
             std::array<std::vector<std::unique_ptr<Factory>>, 2> factories,
             Villager model_villager, Soldier model_soldier,
             Factory model_factory)
    : map(std::move(map)), gold_manager(std::move(gold_manager)),
      path_planner(std::move(path_planner)), soldiers(std::move(soldiers)),
      villagers(std::move(villagers)), factories(std::move(factories)),
      model_villager(std::move(model_villager)),
      model_soldier(std::move(model_soldier)),
      model_factory(std::move(model_factory)) {}

/**
 * Helper function to get the enemy player id
 *
 * @param player_id
 * @return enemy player_id
 */
inline PlayerId GetEnemyPlayerId(PlayerId player_id) {
	if (player_id == PlayerId::PLAYER1) {
		return PlayerId::PLAYER2;
	} else {
		return PlayerId::PLAYER1;
	}
}

void State::ProduceUnit(PlayerId player_id, ActorType actor_type,
                        DoubleVec2D position) {
	auto player_id_index = static_cast<int>(player_id);

	if (actor_type == ActorType::VILLAGER) {
		// If number of villagers would exceed MAX_NUM_VILLAGERS, stop
		if (this->villagers[player_id_index].size() == MAX_NUM_VILLAGERS) {
			return;
		}

		// Create and add the new villager
		auto new_villager = VillagerBuilder(player_id, position);
		this->villagers[player_id_index].push_back(std::move(new_villager));

		// Deduct Villager production cost
		auto villager = this->villagers[player_id_index].back().get();
		gold_manager->DeductUnitCreateCost(player_id, villager);

	} else if (actor_type == ActorType::SOLDIER) {
		// If number of soldiers would exceed MAX_NUM_SOLDIERS, stop
		if (this->soldiers[player_id_index].size() == MAX_NUM_SOLDIERS) {
			return;
		}

		// Create and add the new soldier
		auto new_soldier = SoldierBuilder(player_id, position);
		this->soldiers[player_id_index].push_back(std::move(new_soldier));

		// Deduct Soldier production cost
		auto soldier = this->soldiers[player_id_index].back().get();
		gold_manager->DeductUnitCreateCost(player_id, soldier);

	} else {
		throw std::logic_error("Invalid actor_type passed");
	}
}

void State::MoveUnit(PlayerId player_id, ActorId actor_id, Vec2D position) {
	auto actor = FindActorById(player_id, actor_id);
	auto actor_as_unit = static_cast<Unit *>(actor);

	actor_as_unit->Move(position);
}

void State::MineLocation(PlayerId player_id, ActorId villager_id,
                         Vec2D mine_location) {
	auto actor = FindActorById(player_id, villager_id);
	auto actor_as_villager = static_cast<Villager *>(actor);

	actor_as_villager->Mine(mine_location);
}

void State::AttackActor(PlayerId player_id, ActorId unit_id,
                        ActorId enemy_actor_id) {
	auto actor = FindActorById(player_id, unit_id);
	auto unit = static_cast<Unit *>(actor);

	auto enemy_actor =
	    FindActorById(GetEnemyPlayerId(player_id), enemy_actor_id);

	unit->Attack(enemy_actor);
}

void State::MakeFactory(PlayerId p_player_id, ActorId villager_id,
                        Vec2D offset) {
	auto player_id = static_cast<int64_t>(p_player_id);
	auto factory = FindFactoryByOffset(p_player_id, offset);

	// If the factory doesn't exist, create it
	if (factory == nullptr) {
		auto new_factory = FactoryBuilder(p_player_id, offset);
		factories[player_id].push_back(std::move(new_factory));

		factory = factories[player_id].back().get();

		// Deduct Factory build cost
		gold_manager->DeductUnitCreateCost(p_player_id, factory);
	}

	// Set the villager to build the factory
	auto actor = FindActorById(p_player_id, villager_id);
	auto villager = static_cast<Villager *>(actor);
	villager->Build(factory);
}

void State::CreateFactory(PlayerId player_id, ActorId villager_id,
                          Vec2D offset) {
	int64_t id = static_cast<int64_t>(player_id);

	// Creating the (Vec2d, villager_id) pair
	std::pair<Vec2D, int64_t> new_entry = {offset, villager_id};

	// Adding a new build request
	auto &build_reqs = this->build_requests[id];
	build_reqs.push_back(new_entry);
}

bool State::IsPositionTaken(Vec2D position, int64_t enemy_id) {
	auto build_reqs = this->build_requests[enemy_id];
	for (int64_t req_no = 0; req_no < build_reqs.size(); ++req_no) {
		auto request = build_reqs[req_no];
		if (request.first == position) {
			return true;
		}
	}
	return false;
}

void State::HandleBuildRequests() {
	// Iterating through the players requests
	for (int64_t id = 0; id < 2; ++id) {
		auto &build_reqs = this->build_requests[id];
		int64_t enemy_id =
		    (id + 1) % static_cast<int64_t>(PlayerId::PLAYER_COUNT);

		// Iterating through each player's build requests
		for (int64_t req_no = 0; req_no < build_reqs.size(); ++req_no) {
			auto build_req = build_reqs[req_no];
			Vec2D build_pos = build_req.first;
			int64_t villager_id = build_req.second;

			bool is_pos_taken = IsPositionTaken(build_pos, enemy_id);

			// NOTE! : If both players simultaneously try to build a factory on
			// a position, then none of the request will be processed

			// If the enemy has not issued a build request at the same position
			if (not is_pos_taken) {
				// Calling MakeFactory for the given player as there are no
				// build collisions
				PlayerId player_id = static_cast<PlayerId>(id);
				MakeFactory(player_id, villager_id, build_pos);
			}
		}
	}

	for (auto &player_build_requests : this->build_requests) {
		player_build_requests.clear();
	}
}

void State::BuildFactory(PlayerId p_player_id, ActorId villager_id,
                         ActorId factory_id) {
	auto actor = FindActorById(p_player_id, factory_id);
	auto factory = static_cast<Factory *>(actor);

	if (factory == nullptr) {
		// This shouldn't happen
		throw std::logic_error("Factory with that ID does not exist");
	}

	// Set the villager to build the factory
	auto villager_actor = FindActorById(p_player_id, villager_id);
	auto villager = static_cast<Villager *>(villager_actor);
	villager->Build(factory);
}

void State::SetFactoryProduction(PlayerId p_player_id, ActorId factory_id,
                                 ActorType production) {
	auto actor = FindActorById(p_player_id, factory_id);
	auto factory = static_cast<Factory *>(actor);

	factory->SetProductionState(production);
}

void State::StopOrStartFactory(PlayerId p_player_id, ActorId factory_id,
                               bool should_stop) {
	auto actor = FindActorById(p_player_id, factory_id);
	auto factory = static_cast<Factory *>(actor);

	if (should_stop) {
		factory->Stop();
	} else {
		factory->Start();
	}
}

/**
 * Helper function to get raw pointers from state's actors lists
 *
 * @tparam T Actor type (Soldier, Villager, or Factory)
 * @param actors (Array of Vector of UniquePtrs to Actors)
 * @return const std::array<std::vector<T *>, 2> List of Raw Pointers
 */
template <typename T>
const std::array<std::vector<T *>, 2> GetRawPtrsFromUniquePtrs(
    std::array<std::vector<std::unique_ptr<T>>, 2> &actors) {
	// Let's get the raw pointers from unique pointers
	auto ret_actors = std::array<std::vector<T *>, 2>{};

	// For each player,
	for (int i = 0; i < 2; ++i) {
		auto player_actors = std::vector<T *>{};
		player_actors.reserve(actors[i].size());

		// For each actor,
		for (int j = 0; j < actors[i].size(); ++j) {
			player_actors.push_back(actors[i][j].get());
		}
		ret_actors[i] = player_actors;
	}

	return ret_actors;
}

const std::array<std::vector<Soldier *>, 2> State::GetSoldiers() {
	return GetRawPtrsFromUniquePtrs(soldiers);
}

const std::array<std::vector<Villager *>, 2> State::GetVillagers() {
	return GetRawPtrsFromUniquePtrs(villagers);
}

const std::array<std::vector<Factory *>, 2> State::GetFactories() {
	return GetRawPtrsFromUniquePtrs(factories);
}

const Map *State::GetMap() { return map.get(); }

const std::array<int64_t, 2> State::GetGold() {
	auto gold = std::array<int64_t, 2>{};

	gold[0] = gold_manager->GetBalance(PlayerId::PLAYER1);
	gold[1] = gold_manager->GetBalance(PlayerId::PLAYER2);

	return gold;
}

const std::array<int64_t, 2> State::GetScores() {
	// TODO: Implement scores
	return std::array<int64_t, 2>{0, 0};
}

void State::Update() {
	// Update Actors
	for (auto &player_soldiers : soldiers) {
		for (auto &soldier : player_soldiers) {
			soldier->Update();
		}
	}

	for (auto &player_villagers : villagers) {
		for (auto &villager : player_villagers) {
			villager->Update();
		}
	}

	for (auto &player_factories : factories) {
		for (auto &factory : player_factories) {
			factory->Update();
		}
	}

	// Late Update Actors
	for (auto &player_soldiers : soldiers) {
		for (auto &soldier : player_soldiers) {
			soldier->LateUpdate();
		}
	}

	for (auto &player_villagers : villagers) {
		for (auto &villager : player_villagers) {
			villager->LateUpdate();
		}
	}

	for (auto &player_factories : factories) {
		for (auto &factory : player_factories) {
			factory->LateUpdate();
		}
	}

	// Remove dead actors
	for (auto &player_soldiers : soldiers) {
		auto last_valid_elem = std::remove_if(
		    player_soldiers.begin(), player_soldiers.end(),
		    [](std::unique_ptr<Soldier> &s) { return s->GetHp() == 0; });

		player_soldiers.erase(last_valid_elem, player_soldiers.end());
	}

	for (auto &player_villagers : villagers) {
		auto last_valid_elem = std::remove_if(
		    player_villagers.begin(), player_villagers.end(),
		    [](std::unique_ptr<Villager> &v) { return v->GetHp() == 0; });

		player_villagers.erase(last_valid_elem, player_villagers.end());
	}

	for (auto &player_factories : factories) {
		auto last_valid_elem = std::remove_if(
		    player_factories.begin(), player_factories.end(),
		    [](std::unique_ptr<Factory> &f) { return f->GetHp() == 0; });

		player_factories.erase(last_valid_elem, player_factories.end());
	}

	// Handling build requests by villagers
	HandleBuildRequests();
}

bool State::IsGameOver(PlayerId &winner) {
	bool is_game_over = false;
	auto is_player_dead = std::vector<bool>{false, false};

	// For each player...
	for (int i = 0; i < 2; ++i) {
		auto &player_villagers = villagers[i];
		auto &player_soldiers = soldiers[i];
		auto &player_factories = factories[i];

		// If all player units are dead...
		if (player_villagers.empty() && player_soldiers.empty() &&
		    player_factories.empty()) {

			// ...then the game is over, and this player has died.
			is_player_dead[i] = true;
			is_game_over = true;
		}
	}

	if (is_player_dead[0] && is_player_dead[1]) {
		winner = PlayerId::PLAYER_NULL;
	} else if (is_player_dead[0]) {
		winner = PlayerId::PLAYER2;
	} else if (is_player_dead[1]) {
		winner = PlayerId::PLAYER1;
	}

	// If no one died, is_game_over is false by default, leave winner alone
	return is_game_over;
}

} // namespace state
