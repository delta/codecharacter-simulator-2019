#pragma once

#include "gmock/gmock.h"
#include "state/interfaces/i_gold_manager.h"

using namespace state;

class GoldManagerMock : public IGoldManager {
  public:
	MOCK_METHOD2(Increase, void(PlayerId, int64_t));
	MOCK_METHOD2(Decrease, void(PlayerId, int64_t));
	MOCK_METHOD1(RewardKill, void(Actor *));
	MOCK_METHOD1(GetCreateUnitCost, int64_t(ActorType));
	MOCK_METHOD2(DeductUnitCreateCost, void(PlayerId, Actor *));
	MOCK_METHOD1(GetBalance, int64_t(PlayerId));
	MOCK_METHOD0(GetMaxGold, int64_t());
	MOCK_METHOD1(DeductFactorySuicidePenalty, void(PlayerId));
	MOCK_METHOD3(RewardMineGold, void(PlayerId, GoldMine *, int64_t));
	MOCK_METHOD2(AddMineRequest, void(PlayerId, Vec2D));
	MOCK_METHOD0(AssignGold, void());
};
