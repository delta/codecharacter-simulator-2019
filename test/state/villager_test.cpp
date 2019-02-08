#include "constants/gold_manager.h"
#include "gmock/gmock.h"
#include "state/actor/factory.h"
#include "state/actor/soldier.h"
#include "state/actor/villager.h"
#include "state/mocks/gold_manager_mock.h"
#include "gtest/gtest.h"

using namespace std;
using namespace state;
using namespace physics;
using namespace testing;

const auto L = TerrainType::LAND;
const auto W = TerrainType::WATER;
const auto G = TerrainType::GOLD_MINE;

class VillagerTest : public Test {
  protected:
	int64_t map_size = 5;
	int64_t element_size = 10;
	unique_ptr<Map> map;

	array<int64_t, 2> player_gold;
	int64_t soldier_kill_reward_gold;
	int64_t villager_kill_reward_gold;
	int64_t factory_kill_reward_gold;
	int64_t max_gold;

	unique_ptr<GoldManagerMock> gold_manager;
	unique_ptr<PathPlanner> path_planner;
	unique_ptr<Villager> villager;

  public:
	std::unique_ptr<Villager> MakeTestVillager() {
		return std::make_unique<Villager>(
		    2, PlayerId::PLAYER2, ActorType::VILLAGER, 100, 100,
		    DoubleVec2D(15, 15), gold_manager.get(), path_planner.get(), 10, 10,
		    10, 10, 10, 10);
	}

	VillagerTest() {
		auto map_matrix = vector<vector<TerrainType>>{{
		    {L, L, L, L, L},
		    {L, L, L, L, L},
		    {G, G, G, G, G},
		    {W, W, W, W, W},
		    {W, W, W, W, W},
		}};

		map = make_unique<Map>(map_matrix, map_size, element_size);
		for (int i = 0; i < (int)PlayerId::PLAYER_COUNT; ++i) {
			player_gold[i] = 5000; // Start balance
		}

		this->max_gold = 10000;
		this->soldier_kill_reward_gold = SOLDIER_KILL_REWARD_AMOUNT;
		this->villager_kill_reward_gold = VILLAGER_KILL_REWARD_AMOUNT;
		this->factory_kill_reward_gold = FACTORY_KILL_REWARD_AMOUNT;

		this->gold_manager = make_unique<GoldManagerMock>();

		this->path_planner = make_unique<PathPlanner>(map.get());

		this->villager = make_unique<Villager>(
		    1, PlayerId::PLAYER1, ActorType::VILLAGER, 100, 100,
		    DoubleVec2D(10, 10), gold_manager.get(), path_planner.get(), 10, 10,
		    10, 10, 10, 10);
	}
};

TEST_F(VillagerTest, TransitionToAttackState) {
	auto target_villager = MakeTestVillager();

	this->villager->Attack(target_villager.get());

	this->villager->Update();
	target_villager->Update();
	this->villager->LateUpdate();
	target_villager->LateUpdate();

	ASSERT_EQ(this->villager->GetState(), VillagerStateName::ATTACK);
}

TEST_F(VillagerTest, MoveToDestination) {
	this->villager->SetDestination(Vec2D(15, 15));

	this->villager->Update();
	while (this->villager->GetState() != VillagerStateName::IDLE) {
		this->villager->LateUpdate();
		this->villager->Update();
	}
	this->villager->LateUpdate();

	ASSERT_EQ(this->villager->GetPosition(), DoubleVec2D(15, 15));
}

TEST_F(VillagerTest, TransitionToMineState) {
	EXPECT_CALL(*this->gold_manager,
	            AddBuildRequest(PlayerId::PLAYER1, Vec2D(10, 10)));
	this->villager->Mine(Vec2D(10, 10));

	this->villager->Update();
	this->villager->LateUpdate();

	ASSERT_EQ(this->villager->GetState(), VillagerStateName::MINE);
}

TEST_F(VillagerTest, DecreaseHpOnAttack) {
	int64_t initial_hp = 100;
	int64_t attack_damage = 10;

	auto target_villager = MakeTestVillager();

	this->villager->Attack(target_villager.get());

	this->villager->Update();
	target_villager->Update();
	this->villager->LateUpdate();
	target_villager->LateUpdate();

	ASSERT_EQ(target_villager->GetHp(), (initial_hp - attack_damage));
}

TEST_F(VillagerTest, TransitionToDeadState) {
	auto target_villager = MakeTestVillager();

	this->villager->Attack(target_villager.get());

	while (target_villager->GetHp() != 0) {
		this->villager->Update();
		target_villager->Update();
		this->villager->LateUpdate();
		target_villager->LateUpdate();
	}

	ASSERT_EQ(target_villager->GetState(), VillagerStateName::DEAD);
}

TEST_F(VillagerTest, MoveToMine) {
	this->villager->SetMineTarget(Vec2D(20, 10));

	while (this->villager->GetState() != VillagerStateName::MINE) {
		this->villager->Update();
		this->villager->LateUpdate();
	}

	ASSERT_EQ(this->villager->IsMineTargetInRange(), true);
}
//This is a test for gold manager, not for villager
// TEST_F(VillagerTest, VillagerKillReward) {
// 	int64_t initial_gold = player_gold[0];

// 	auto target_villager = MakeTestVillager();

// 	EXPECT_CALL(*this->gold_manager, RewardKill(target_villager.get()));
// 	this->villager->Attack(target_villager.get());

// 	while (target_villager->GetHp() != 0) {
// 		this->villager->Update();
// 		target_villager->Update();
// 		this->villager->LateUpdate();
// 		target_villager->LateUpdate();
// 	}

// 	ASSERT_EQ(gold_manager->GetBalance(PlayerId::PLAYER1),
// 	          (initial_gold + villager_kill_reward_gold));
// }

TEST_F(VillagerTest, SimultaneousKill) {
	auto target_villager = MakeTestVillager();

	villager->Attack(target_villager.get());
	target_villager->Attack(villager.get());

	while (target_villager->GetHp() != 0) {
		this->villager->Update();
		target_villager->Update();
		this->villager->LateUpdate();
		target_villager->LateUpdate();
	}

	ASSERT_EQ(villager->GetHp(), 0);
}

TEST_F(VillagerTest, BuildFactory) {
	auto villager_list = std::vector<std::unique_ptr<Villager>>{};
	auto soldier_list = std::vector<std::unique_ptr<Soldier>>{};
	auto target_factory = std::make_unique<Factory>(
	    2, PlayerId::PLAYER1, ActorType::FACTORY, 100, 100, DoubleVec2D(10, 10),
	    gold_manager.get(), 0, 100, ActorType::VILLAGER, 5, 5,
	    UnitProductionCallback{});

	this->villager->Build(target_factory.get());

	while (not target_factory->IsConstructionComplete()) {
		this->villager->Update();
		this->villager->LateUpdate();
	}

	ASSERT_EQ(target_factory->IsConstructionComplete(), true);
}
