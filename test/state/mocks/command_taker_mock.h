#pragma once

#include "gmock/gmock.h"
#include "state/interfaces/i_command_taker.h"

using namespace state;

class CommandTakerMock : public ICommandTaker {
  public:
	MOCK_METHOD3(MoveUnit,
	             void(PlayerId player_id, ActorId actor_id, Vec2D position));
	MOCK_METHOD3(MineLocation,
	             void(PlayerId player_id, ActorId actor_id, Vec2D position));
	MOCK_METHOD3(AttackActor, void(PlayerId player_id, ActorId unit_id,
	                               PlayerId enemy_actor_id));
	MOCK_METHOD4(CreateFactory, void(PlayerId player_id, ActorId actor_id,
	                                 Vec2D offset, ActorType unit_type));
	MOCK_METHOD3(BuildFactory, void(PlayerId player_id, ActorId actor_id,
	                                ActorId factory_id));
	MOCK_METHOD3(SetFactoryProduction,
	             void(PlayerId player_id, ActorId factory_id,
	                  ActorType production_type));
	MOCK_METHOD3(StopOrStartProduction,
	             void(PlayerId player_id, ActorId factory_id,
	                  bool should_stop));
	MOCK_METHOD0(GetGold, const std::array<int64_t, 2>());
	MOCK_METHOD0(GetMap, Map *());
	MOCK_METHOD0(GetScores, const std::array<int64_t, 2>());
	MOCK_METHOD0(GetInterestingness, int64_t());
	MOCK_METHOD1(IsGameOver, bool(PlayerId &player_id));
	MOCK_METHOD3(AttackActor, void(PlayerId player_id, ActorId unit_id,
	                               ActorId enemy_actor_id));
	MOCK_METHOD0(Update, void());
	MOCK_METHOD0(GetSoldiers, const std::array<std::vector<Soldier *>, 2>());
	MOCK_METHOD0(GetVillagers, const std::array<std::vector<Villager *>, 2>());
	MOCK_METHOD0(GetFactories, const std::array<std::vector<Factory *>, 2>());
	MOCK_METHOD3(StopOrStartFactory,
	             void(PlayerId player_id, ActorId factory_id,
	                  bool should_stop));
	MOCK_METHOD2(FindActorById, Actor *(PlayerId player_id, ActorId actor_id));
};
