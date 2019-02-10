#include "constants/gold_manager.h"
#include "gmock/gmock.h"
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

class SoldierTest : public Test {
  protected:
	int64_t map_size = 5;
	int64_t element_size = 10;
	unique_ptr<Map> map;

	unique_ptr<GoldManagerMock> gold_manager;
	unique_ptr<PathPlanner> path_planner;
	unique_ptr<Soldier> soldier;

  public:
	SoldierTest() {
		auto map_matrix = vector<vector<TerrainType>>{{
		    {L, L, L, L, L},
		    {L, L, L, L, L},
		    {G, G, G, G, G},
		    {W, W, W, W, W},
		    {W, W, W, W, W},
		}};

		map = make_unique<Map>(map_matrix, map_size, element_size);
		gold_manager = make_unique<GoldManagerMock>();

		this->soldier =
		    make_unique<Soldier>(1, PlayerId::PLAYER1, ActorType::SOLDIER, 100,
		                         100, DoubleVec2D(10, 10), gold_manager.get(),
		                         path_planner.get(), 10, 10, 10);
	}
};

TEST_F(SoldierTest, MoveToAttackState) {
	auto *target_soldier = new Soldier(
	    2, PlayerId::PLAYER2, ActorType::SOLDIER, 100, 100, DoubleVec2D(15, 15),
	    gold_manager.get(), path_planner.get(), 10, 10, 10);

	this->soldier->Attack(target_soldier);

	this->soldier->Update();
	target_soldier->Update();
	this->soldier->LateUpdate();
	target_soldier->LateUpdate();

	ASSERT_EQ(this->soldier->GetState(), SoldierStateName::ATTACK);
}

TEST_F(SoldierTest, MoveToDestination) {
	this->soldier->SetDestination(Vec2D(30, 30));

	while (this->soldier->GetState() != SoldierStateName::IDLE) {
		this->soldier->Update();
		this->soldier->LateUpdate();
	}

	ASSERT_EQ(this->soldier->GetDestination(), Vec2D(30, 30));
}

TEST_F(SoldierTest, DecreaseHpOnAttack) {
	int64_t initial_hp = 100;
	int64_t attack_damage = 10;

	auto *target_soldier =
	    new Soldier(2, PlayerId::PLAYER2, ActorType::SOLDIER, initial_hp, 100,
	                DoubleVec2D(15, 15), gold_manager.get(), path_planner.get(),
	                10, 10, attack_damage);

	this->soldier->Attack(target_soldier);

	this->soldier->Update();
	target_soldier->Update();
	this->soldier->LateUpdate();
	target_soldier->LateUpdate();

	ASSERT_EQ(target_soldier->GetHp(), (initial_hp - attack_damage));
}

TEST_F(SoldierTest, MoveToDeadState) {
	int64_t initial_hp = 100;
	int64_t attack_damage = 10;

	auto *target_soldier =
	    new Soldier(2, PlayerId::PLAYER2, ActorType::SOLDIER, initial_hp, 100,
	                DoubleVec2D(15, 15), gold_manager.get(), path_planner.get(),
	                10, 10, attack_damage);

	// Making soldier attack the other soldier
	this->soldier->Attack(target_soldier);

	// Expect calls to GoldManager for killing unit
	EXPECT_CALL(*gold_manager, RewardKill(target_soldier));

	// Soldier continuously attacking the other soldier until he dies
	while (target_soldier->GetHp() != 0) {
		this->soldier->Update();
		target_soldier->Update();
		this->soldier->LateUpdate();
		target_soldier->LateUpdate();
	}

	ASSERT_EQ(target_soldier->GetState(), SoldierStateName::DEAD);
}

TEST_F(SoldierTest, SoldierKillReward) {
	int64_t initial_hp = 100;
	int64_t attack_damage = 10;
	int64_t initial_gold = 100;

	auto *target_soldier =
	    new Soldier(2, PlayerId::PLAYER2, ActorType::SOLDIER, initial_hp, 100,
	                DoubleVec2D(15, 15), gold_manager.get(), path_planner.get(),
	                10, 10, attack_damage);

	this->soldier->Attack(target_soldier);

	// Expecting call to gold manager to reward kill of soldier
	EXPECT_CALL(*gold_manager, RewardKill(target_soldier));

	while (target_soldier->GetHp() != 0) {
		this->soldier->Update();
		target_soldier->Update();
		this->soldier->LateUpdate();
		target_soldier->LateUpdate();
	}
}

TEST_F(SoldierTest, PursuitAndKill) {
	int64_t initial_hp = 100;
	int64_t attack_damage = 10;
	auto *target_soldier =
	    new Soldier(2, PlayerId::PLAYER2, ActorType::SOLDIER, initial_hp, 100,
	                DoubleVec2D(15, 15), gold_manager.get(), path_planner.get(),
	                10, 10, attack_damage);

	this->soldier->SetAttackTarget(target_soldier);

	// Expecting call to gold manager to reward the soldier for killing target
	// soldier
	EXPECT_CALL(*gold_manager, RewardKill(target_soldier));

	while (target_soldier->GetHp() != 0) {
		this->soldier->Update();
		target_soldier->Update();
		this->soldier->LateUpdate();
		target_soldier->LateUpdate();
	}

	ASSERT_EQ(target_soldier->GetState(), SoldierStateName::DEAD);
}

TEST_F(SoldierTest, SimultaneousKill) {
	auto *target_soldier =
	    new Soldier(2, PlayerId::PLAYER2, ActorType::SOLDIER, 100, 100,
	                DoubleVec2D(15, 15), gold_manager.get(), path_planner.get(),
	                10, 10, 10);

	// Both soldiers attacking each other
	soldier->Attack(target_soldier);
	target_soldier->Attack(soldier.get());


	// Expecting call to gold manager to reward gold to each soldier for killing the other
	EXPECT_CALL(*gold_manager, RewardKill(target_soldier));
	EXPECT_CALL(*gold_manager, RewardKill(soldier.get())); 

	while (target_soldier->GetHp() != 0) {
		this->soldier->Update();
		target_soldier->Update();
		this->soldier->LateUpdate();
		target_soldier->LateUpdate();
	}

	ASSERT_EQ(soldier->GetHp(), 0);
}
