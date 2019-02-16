/**
 * @file state_helpers.cpp
 * Defines the helper methods in State
 */

#include "state/state.h"

namespace state {

/**
 * Get the Tile Position From the given offset
 *
 * @param offset
 * @param element_size
 * @return Vec2D
 */
inline Vec2D GetTilePositionFromOffset(Vec2D offset, int64_t element_size) {

	auto position = Vec2D{};
	position.x = offset.x * element_size + (element_size / 2);
	position.y = offset.y * element_size + (element_size / 2);
	return position;
}

std::unique_ptr<Factory> State::FactoryBuilder(PlayerId p_player_id,
                                               Vec2D offset,
                                               ActorType produce_unit) {
	// Convert the given offset into a position centered at that offset
	auto position =
	    GetTilePositionFromOffset(offset, map->GetElementSize()).to_double();

	// Generate callback to ProduceUnit method
	using namespace std::placeholders;
	auto unit_production_callback =
	    std::bind(&State::ProduceUnit, this, _1, _2, _3);

	// Create a new Factory, and set the right parameters
	auto factory = std::make_unique<Factory>(
	    Actor::GetNextActorId(), p_player_id, model_factory.GetActorType(),
	    model_factory.GetHp(), model_factory.GetMaxHp(), position,
	    gold_manager.get(), model_factory.GetConstructionCompletion(),
	    model_factory.GetTotalConstructionCompletion(), produce_unit,
	    model_factory.GetVillagerFrequency(),
	    model_factory.GetSoldierFrequency(), unit_production_callback);

	return factory;
}

std::unique_ptr<Villager> State::VillagerBuilder(PlayerId p_player_id,
                                                 DoubleVec2D position) {
	auto new_villager = std::make_unique<Villager>(
	    Actor::GetNextActorId(), p_player_id, model_villager.GetActorType(),
	    model_villager.GetHp(), model_villager.GetMaxHp(), position,
	    gold_manager.get(), path_planner.get(), model_villager.GetSpeed(),
	    model_villager.GetAttackRange(), model_villager.GetAttackDamage(),
	    model_villager.GetBuildEffort(), model_villager.GetBuildRange(),
	    model_villager.GetMineRange());

	return new_villager;
}

std::unique_ptr<Soldier> State::SoldierBuilder(PlayerId p_player_id,
                                               DoubleVec2D position) {
	auto new_soldier = std::make_unique<Soldier>(
	    Actor::GetNextActorId(), p_player_id, model_soldier.GetActorType(),
	    model_soldier.GetHp(), model_soldier.GetMaxHp(), position,
	    gold_manager.get(), path_planner.get(), model_soldier.GetSpeed(),
	    model_soldier.GetAttackRange(), model_soldier.GetAttackDamage());

	return new_soldier;
}

Actor *State::FindActorById(PlayerId p_player_id, ActorId actor_id) {
	auto player_id = static_cast<int64_t>(p_player_id);

	// TODO: Actor search can be made faster with a lookup table
	// Search soldiers
	for (auto &soldier : soldiers[player_id]) {
		if (soldier->GetActorId() == actor_id) {
			return soldier.get();
		}
	}

	// Search Villagers
	for (auto &villager : villagers[player_id]) {
		if (villager->GetActorId() == actor_id) {
			return villager.get();
		}
	}

	// Search factories
	for (auto &factory : factories[player_id]) {
		if (factory->GetActorId() == actor_id) {
			return factory.get();
		}
	}

	return nullptr;
}

Factory *State::FindFactoryByOffset(PlayerId p_player_id, Vec2D offset) {
	auto player_id = static_cast<int64_t>(p_player_id);
	auto position = GetTilePositionFromOffset(offset, map->GetElementSize());

	for (auto &factory : factories[player_id]) {
		if (factory->GetPosition().to_int() == position) {
			return factory.get();
		}
	}

	return nullptr;
}

} // namespace state
