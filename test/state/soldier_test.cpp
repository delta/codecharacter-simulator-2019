#include "constants/gold_manager.h"
#include "gmock/gmock.h"
#include "state/actor/soldier.h"
#include "state/actor/villager.h"
#include "state/gold_manager/gold_manager.h"
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

	array<int64_t, 2> player_gold;
	int64_t soldier_kill_reward_gold;
	int64_t villager_kill_reward_gold;
	int64_t factory_kill_reward_gold;
	int64_t max_gold;

	unique_ptr<GoldManager> gold_manager;
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

		for (int i = 0; i < (int)PlayerId::PLAYER_COUNT; ++i) {
			player_gold[i] = 5000; // Start balance
		}

		this->max_gold = 10000;
		this->soldier_kill_reward_gold = SOLDIER_KILL_REWARD_AMOUNT;
		this->villager_kill_reward_gold = VILLAGER_KILL_REWARD_AMOUNT;
		this->factory_kill_reward_gold = FACTORY_KILL_REWARD_AMOUNT;

		this->gold_manager = make_unique<GoldManager>(
		    player_gold, max_gold, soldier_kill_reward_gold,
		    villager_kill_reward_gold, factory_kill_reward_gold,
		    FACTORY_SUICIDE_PENALTY, VILLAGER_COST, SOLDIER_COST, FACTORY_COST,
		    MINING_REWARD);
		this->path_planner = make_unique<PathPlanner>(map.get());

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

	this->soldier->Attack(target_soldier);

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
	int64_t initial_gold = player_gold[0];

	auto *target_soldier =
	    new Soldier(2, PlayerId::PLAYER2, ActorType::SOLDIER, initial_hp, 100,
	                DoubleVec2D(15, 15), gold_manager.get(), path_planner.get(),
	                10, 10, attack_damage);

	this->soldier->Attack(target_soldier);

	while (target_soldier->GetHp() != 0) {
		this->soldier->Update();
		target_soldier->Update();
		this->soldier->LateUpdate();
		target_soldier->LateUpdate();
	}

	ASSERT_EQ(gold_manager->GetBalance(PlayerId::PLAYER1),
	          (initial_gold + soldier_kill_reward_gold));
}

TEST_F(SoldierTest, PursuitAndKill) {
	int64_t initial_hp = 100;
	int64_t attack_damage = 10;

	auto *target_soldier =
	    new Soldier(2, PlayerId::PLAYER2, ActorType::SOLDIER, initial_hp, 100,
	                DoubleVec2D(10, 40), gold_manager.get(), path_planner.get(),
	                10, 10, attack_damage);

	this->soldier->SetAttackTarget(target_soldier);

	while (target_soldier->GetHp() != 0) {
		this->soldier->Update();
		target_soldier->Update();
		this->soldier->LateUpdate();
		target_soldier->LateUpdate();
	}

	ASSERT_EQ(target_soldier->GetState(), SoldierStateName::DEAD);
}

TEST_F(SoldierTest, SimultaneousKill) {
	auto target_soldier = make_unique<Soldier>(
	    2, PlayerId::PLAYER2, ActorType::SOLDIER, 100, 100, DoubleVec2D(35, 45),
	    gold_manager.get(), path_planner.get(), 10, 10, 10);

	soldier->Attack(target_soldier.get());
	target_soldier->Attack(soldier.get());

	while (target_soldier->GetHp() != 0) {
		this->soldier->Update();
		target_soldier->Update();
		this->soldier->LateUpdate();
		target_soldier->LateUpdate();
	}

	ASSERT_EQ(soldier->GetHp(), 0);
}
