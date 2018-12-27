#pragma once

#include "gmock/gmock.h"
#include "state/interfaces/i_command_taker.h"

using namespace state;
using Vec2D = physics::Vector<int64_t>;

class CommandTakerMock : public ICommandTaker {
  public:
	MOCK_METHOD3(MoveUnit,
	             void(PlayerId player_id, ActorId actor_id, Vec2D position));
	MOCK_METHOD3(MineLocation,
	             void(PlayerId player_id, ActorId actor_id, Vec2D position));
	MOCK_METHOD3(AttackActor, void(PlayerId player_id, ActorId unit_id,
	                               PlayerId enemy_actor_id));
	MOCK_METHOD3(CreateFactory,
	             void(PlayerId player_id, ActorId actor_id, Vec2D offset));
	MOCK_METHOD3(BuildFactory, void(PlayerId player_id, ActorId actor_id,
	                                ActorType production_type));
	MOCK_METHOD3(SetFactoryProduction,
	             void(PlayerId player_id, ActorId factory_id,
	                  ActorType production_type));
	MOCK_METHOD3(StopOrStartProduction,
	             void(PlayerId player_id, ActorId factory_id,
	                  bool should_stop));
	MOCK_METHOD0(GetAllSoldiers, std::array<std::vector<Soldier *>, 2>());
	MOCK_METHOD0(GetAllVillagers, std::array<std::vector<Villager *>, 2>());
	MOCK_METHOD0(GetGold, std::array<int64_t, 2>());
	MOCK_METHOD0(GetMap, Map *());
	MOCK_METHOD0(GetScores, std : array<int64_t, 2>());
};
