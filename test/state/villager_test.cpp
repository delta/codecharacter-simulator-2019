#include "constants/gold_manager.h"
#include "gmock/gmock.h"
#include "state/actor/factory.h"
#include "state/actor/soldier.h"
#include "state/actor/villager.h"
#include "state/gold_manager/gold_manager.h"
#include "gtest/gtest.h"

using namespace std;
using namespace state;
using namespace physics;
using namespace testing;

class VillagerTest : public Test {
  protected:
	array<int64_t, 2> player_gold;
	int64_t soldier_kill_reward_gold;
	int64_t villager_kill_reward_gold;
	int64_t factory_kill_reward_gold;
	int64_t max_gold;

	unique_ptr<GoldManager> gold_manager;
	unique_ptr<Villager> villager;

  public:
	std::unique_ptr<Villager> MakeTestVillager() {
		return std::move(std::make_unique<Villager>(
		    2, PlayerId::PLAYER2, ActorType::VILLAGER, 100, 100,
		    physics::Vector<int64_t>(15, 15), gold_manager.get(), 10, 10, 10,
		    10, 10, 10));
	}

	VillagerTest() {
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

		this->villager =
		    make_unique<Villager>(1, PlayerId::PLAYER1, ActorType::VILLAGER,
		                          100, 100, physics::Vector<int64_t>(10, 10),
		                          gold_manager.get(), 10, 10, 10, 10, 10, 10);
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

TEST_F(VillagerTest, TransitionToMineState) {
	this->villager->Mine(physics::Vector<int64_t>(10, 10));

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
	int64_t initial_hp = 100;
	int64_t attack_damage = 10;

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

TEST_F(VillagerTest, VillagerKillReward) {
	int64_t initial_gold = player_gold[0];

	auto target_villager = MakeTestVillager();

	this->villager->Attack(target_villager.get());

	while (target_villager->GetHp() != 0) {
		this->villager->Update();
		target_villager->Update();
		this->villager->LateUpdate();
		target_villager->LateUpdate();
	}

	ASSERT_EQ(gold_manager->GetBalance(PlayerId::PLAYER1),
	          (initial_gold + villager_kill_reward_gold));
}

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
	auto target_factory = std::make_unique<Factory>(
	    2, PlayerId::PLAYER2, ActorType::FACTORY_VILLAGER, 100, 100,
	    physics::Vector<int64_t>(15, 15), gold_manager.get(), 0, 100,
	    ProductionState::VILLAGER);

	this->villager->Build(target_factory.get());

	while (not target_factory->IsConstructionComplete()) {
		this->villager->Update();
		this->villager->LateUpdate();
	}

	ASSERT_EQ(target_factory->IsConstructionComplete(), true);
}
