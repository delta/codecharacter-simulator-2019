#pragma once

#include "gmock/gmock.h"
#include "state/actor/factory.h"
#include "state/actor/soldier.h"
#include "state/actor/villager.h"
#include "state/interfaces/i_command_taker.h"
#include "state/interfaces/i_updatable.h"
#include "state/map/map.h"
#include <vector>

using namespace std;
using namespace state;
using namespace physics;

class StateMock : public ICommandTaker {
  public:
	MOCK_METHOD0(GetSoldiers, const std::array<std::vector<Soldier *>, 2>());
	MOCK_METHOD0(GetVillagers, const std::array<std::vector<Villager *>, 2>());
	MOCK_METHOD0(GetFactories, const std::array<std::vector<Factory *>, 2>());
	MOCK_METHOD0(GetMoney, const std::array<int64_t, 2>());
	MOCK_METHOD0(GetMap, const Map *());
	MOCK_METHOD0(GetScores, const std::array<int64_t, 2>());
	MOCK_METHOD3(MoveUnit, void(PlayerId, ActorId, physics::Vector<int64_t>));
	MOCK_METHOD3(MineLocation,
	             void(PlayerId, ActorId, physics::Vector<int64_t>));
	MOCK_METHOD3(AttackActor, void(PlayerId, ActorId, ActorId));
	MOCK_METHOD3(CreateFactory,
	             void(PlayerId, ActorId, physics::Vector<int64_t>));
	MOCK_METHOD3(BuildFactory, void(PlayerId, ActorId, ActorId));
	MOCK_METHOD3(SetFactoryProduction, void(PlayerId, ActorId, ActorType));
	MOCK_METHOD3(StopOrStartFactory, void(PlayerId, ActorId, bool));
	MOCK_METHOD0(Update, void());
	MOCK_METHOD0(GetGold, const std::array<int64_t, 2>());
};
