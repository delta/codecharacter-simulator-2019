/**
 * @file state.cpp
 * Definitions for functions of the State class
 */

#include "state/state.h"

namespace state {

State::State(std::unique_ptr<Map> map,
             std::unique_ptr<GoldManager> gold_manager,
             std::array<std::vector<std::unique_ptr<Soldier>>, 2> soldiers,
             std::array<std::vector<std::unique_ptr<Villager>>, 2> villagers,
             std::array<std::vector<std::unique_ptr<Factory>>, 2> factories)
    : map(std::move(map)), gold_manager(std::move(gold_manager)),
      soldiers(std::move(soldiers)), villagers(std::move(villagers)),
      factories(std::move(factories)) {}

void State::MoveUnit(PlayerId player_id, ActorId actor_id,
                     physics::Vector<int64_t> position) {
	// TODO
}

void State::MineLocation(PlayerId player_id, ActorId villager_id,
                         physics::Vector<int64_t> mine_location) {
	// TODO
}

void State::AttackActor(PlayerId player_id, ActorId unit_id,
                        ActorId enemy_actor_id) {
	// TODO
}

void State::CreateFactory(PlayerId player_id, ActorId villager_id,
                         physics::Vector<int64_t> offset) {
	// TODO
}

void State::BuildFactory(PlayerId player_id, ActorId villager_id,
                         ActorId factory_id) {
	// TODO
}

void State::SetFactoryProduction(PlayerId player_id, ActorId factory_id,
                                 ActorType production) {
	// TODO
}

void State::StopOrStartFactory(PlayerId player_id, ActorId factory_id,
                               bool should_stop) {
	// TODO
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

const std::array<int64_t, 2> State::GetMoney() {
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
	// TODO
}

} // namespace state
