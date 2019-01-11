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

class FactoryTest : public Test {
  protected:
	array<int64_t, 2> player_gold;
	int64_t soldier_kill_reward_gold;
	int64_t villager_kill_reward_gold;
	int64_t factory_kill_reward_gold;
	int64_t max_gold;

	Soldier model_soldier;
	Villager model_villager;

	unique_ptr<GoldManager> gold_manager;
	unique_ptr<PathPlanner> path_planner;
	unique_ptr<Factory> factory;

	std::vector<std::unique_ptr<Villager>> villager_list;
	std::vector<std::unique_ptr<Soldier>> soldier_list;

	int64_t villager_frequency = 5;
	int64_t soldier_frequency = 10;

	void ProduceUnitImpl(PlayerId player_id, ActorType actor_type,
	                     Vec2D position) {
		if (actor_type == ActorType::VILLAGER) {
			// Create and add the new villager
			auto new_villager = std::make_unique<Villager>();
			villager_list.push_back(std::move(new_villager));

		} else if (actor_type == ActorType::SOLDIER) {
			// Create and add the new soldier
			auto new_soldier = std::make_unique<Soldier>();
			soldier_list.push_back(std::move(new_soldier));

		} else {
			throw std::logic_error("Invalid actor_type passed");
		}
	}

  public:
	std::unique_ptr<Villager> MakeTestVillager() {
		return std::make_unique<Villager>(
		    2, PlayerId::PLAYER2, ActorType::VILLAGER, 100, 100,
		    physics::Vector<int64_t>(15, 15), gold_manager.get(),
		    path_planner.get(), 10, 10, 10, 10, 10, 10);
	}

	FactoryTest() {

		player_gold[0] = player_gold[1] = 5000;

		model_villager =
		    Villager(2, PlayerId::PLAYER2, ActorType::VILLAGER, 100, 100,
		             physics::Vector<int64_t>(15, 15), gold_manager.get(),
		             path_planner.get(), 10, 10, 10, 10, 10, 10);

		model_soldier =
		    Soldier(1, PlayerId::PLAYER1, ActorType::SOLDIER, 100, 100,
		            physics::Vector<int64_t>(10, 10), gold_manager.get(),
		            path_planner.get(), 10, 10, 10);

		this->max_gold = 10000;
		this->soldier_kill_reward_gold = SOLDIER_KILL_REWARD_AMOUNT;
		this->villager_kill_reward_gold = VILLAGER_KILL_REWARD_AMOUNT;
		this->factory_kill_reward_gold = FACTORY_KILL_REWARD_AMOUNT;

		this->gold_manager = make_unique<GoldManager>(
		    player_gold, max_gold, soldier_kill_reward_gold,
		    villager_kill_reward_gold, factory_kill_reward_gold,
		    FACTORY_SUICIDE_PENALTY, VILLAGER_COST, SOLDIER_COST, FACTORY_COST,
		    MINING_REWARD);

		villager_list = std::vector<std::unique_ptr<Villager>>{};
		soldier_list = std::vector<std::unique_ptr<Soldier>>{};

		using namespace std::placeholders;
		auto unit_production_callback =
		    std::bind(&FactoryTest::ProduceUnitImpl, this, _1, _2, _3);

		// We always initialize a factory with 1 HP
		this->factory = std::make_unique<Factory>(
		    2, PlayerId::PLAYER2, ActorType::FACTORY_VILLAGER, 1, 100,
		    physics::Vector<int64_t>(15, 15), gold_manager.get(), 0, 100,
		    ActorType::VILLAGER, villager_frequency, soldier_frequency,
		    unit_production_callback);
	}
};

TEST_F(FactoryTest, ConstructionTest) {
	ASSERT_EQ(factory->GetState(), FactoryStateName::UNBUILT);

	factory->Update();
	factory->LateUpdate();

	// Build the factory
	factory->IncrementConstructionCompletion(
	    factory->GetTotalConstructionCompletion());

	factory->Update();
	factory->LateUpdate();

	ASSERT_EQ(factory->GetState(), FactoryStateName::PRODUCTION);
	ASSERT_EQ(factory->GetProductionState(), ActorType::VILLAGER);
}

TEST_F(FactoryTest, UnitProductionTest) {
	ASSERT_EQ(factory->GetState(), FactoryStateName::UNBUILT);

	factory->Update();
	factory->LateUpdate();

	// Build the factory
	factory->IncrementConstructionCompletion(
	    factory->GetTotalConstructionCompletion());

	factory->SetProductionState(ActorType::VILLAGER);

	// After frequency no. of updates, a villager should have been produced
	for (int i = 0; i < villager_frequency; ++i) {
		factory->Update();
		factory->LateUpdate();
	}

	ASSERT_EQ(villager_list.size(), 1);

	// After 20 x frequency updates, 20 more villagers should have been produced
	for (int i = 0; i < 20 * villager_frequency; ++i) {
		factory->Update();
		factory->LateUpdate();
	}

	ASSERT_EQ(villager_list.size(), 1 + 20);
}

TEST_F(FactoryTest, SwitchUnitProductionTest) {
	ASSERT_EQ(factory->GetState(), FactoryStateName::UNBUILT);

	factory->Update();
	factory->LateUpdate();

	// Build the factory
	factory->IncrementConstructionCompletion(
	    factory->GetTotalConstructionCompletion());

	factory->SetProductionState(ActorType::VILLAGER);

	// After frequency no. of updates, a villager should have been produced
	for (int i = 0; i < villager_frequency; ++i) {
		factory->Update();
		factory->LateUpdate();
	}

	ASSERT_EQ(villager_list.size(), 1);

	// Let's swap to soldiers
	factory->SetProductionState(ActorType::SOLDIER);

	// After 20 x frequency updates, 20 soldiers should have been produced
	for (int i = 0; i < 20 * soldier_frequency; ++i) {
		factory->Update();
		factory->LateUpdate();
	}

	ASSERT_EQ(soldier_list.size(), 20);
}

TEST_F(FactoryTest, StopStartTest) {
	ASSERT_EQ(factory->GetState(), FactoryStateName::UNBUILT);

	factory->Update();
	factory->LateUpdate();

	// Build the factory and produce a unit
	factory->IncrementConstructionCompletion(
	    factory->GetTotalConstructionCompletion());

	factory->Update();
	factory->LateUpdate();

	ASSERT_EQ(factory->GetState(), FactoryStateName::PRODUCTION);

	// Produce a unit (- 1 since we ran 1 update just now)
	for (int i = 0; i < villager_frequency - 1; ++i) {
		factory->Update();
		factory->LateUpdate();
	}
	ASSERT_EQ(villager_list.size(), 1);

	// Stop Production
	factory->Stop();
	factory->Update();
	factory->LateUpdate();
	ASSERT_EQ(factory->GetState(), FactoryStateName::IDLE);

	// Resume Production
	factory->Start();
	factory->Update();
	factory->LateUpdate();
	ASSERT_EQ(factory->GetState(), FactoryStateName::PRODUCTION);
}

TEST_F(FactoryTest, DeathTest) {
	ASSERT_EQ(factory->GetState(), FactoryStateName::UNBUILT);

	factory->Update();
	factory->LateUpdate();

	// Build the factory
	factory->IncrementConstructionCompletion(
	    factory->GetTotalConstructionCompletion());

	factory->Update();
	factory->LateUpdate();

	ASSERT_EQ(factory->GetState(), FactoryStateName::PRODUCTION);

	// Kill it
	factory->SetHp(0);
	factory->Update();
	factory->LateUpdate();

	ASSERT_EQ(factory->GetState(), FactoryStateName::DEAD);
}
