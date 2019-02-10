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

class FactoryTest : public Test {
  protected:
	array<int64_t, 2> player_gold;
	int64_t soldier_kill_reward_gold;
	int64_t villager_kill_reward_gold;
	int64_t factory_kill_reward_gold;
	int64_t max_gold;

	Soldier model_soldier;
	Villager model_villager;

	unique_ptr<GoldManagerMock> gold_manager;
	unique_ptr<PathPlanner> path_planner;
	unique_ptr<Factory> factory;

	std::vector<std::unique_ptr<Villager>> villager_list;
	std::vector<std::unique_ptr<Soldier>> soldier_list;

	int64_t villager_frequency = 5;
	int64_t soldier_frequency = 10;

	void ProduceUnitImpl(PlayerId player_id, ActorType actor_type,
	                     DoubleVec2D position) {
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
		    DoubleVec2D(15, 15), gold_manager.get(), path_planner.get(), 10, 10,
		    10, 10, 10, 10);
	}

	FactoryTest() {

		player_gold[0] = player_gold[1] = 5000;

		model_villager =
		    Villager(2, PlayerId::PLAYER2, ActorType::VILLAGER, 100, 100,
		             DoubleVec2D(15, 15), gold_manager.get(),
		             path_planner.get(), 10, 10, 10, 10, 10, 10);

		model_soldier = Soldier(1, PlayerId::PLAYER1, ActorType::SOLDIER, 100,
		                        100, DoubleVec2D(10, 10), gold_manager.get(),
		                        path_planner.get(), 10, 10, 10);

		this->max_gold = 10000;
		this->soldier_kill_reward_gold = SOLDIER_KILL_REWARD_AMOUNT;
		this->villager_kill_reward_gold = VILLAGER_KILL_REWARD_AMOUNT;
		this->factory_kill_reward_gold = FACTORY_KILL_REWARD_AMOUNT;

		std::vector<std::unique_ptr<GoldMine>> gold_mines;

		this->gold_manager = make_unique<GoldManagerMock>();

		villager_list = std::vector<std::unique_ptr<Villager>>{};
		soldier_list = std::vector<std::unique_ptr<Soldier>>{};

		using namespace std::placeholders;
		auto unit_production_callback =
		    std::bind(&FactoryTest::ProduceUnitImpl, this, _1, _2, _3);

		// We always initialize a factory with 1 HP
		this->factory = std::make_unique<Factory>(
		    2, PlayerId::PLAYER2, ActorType::FACTORY, 1, 100,
		    DoubleVec2D(15, 15), gold_manager.get(), 0, 100,
		    ActorType::VILLAGER, villager_frequency, soldier_frequency,
		    unit_production_callback);
	}
};

TEST_F(FactoryTest, ConstructionTest) {
	ASSERT_EQ(factory->GetState(), FactoryStateName::UNBUILT);

	// Build the factory
	factory->IncrementConstructionCompletion(
	    factory->GetTotalConstructionCompletion());

	// Expecting calls to gold manager
	EXPECT_CALL(*gold_manager, GetBalance(factory->GetPlayerId()))
	    .Times(2)
	    .WillRepeatedly(Return(1000));
	EXPECT_CALL(*gold_manager, GetCreateUnitCost(factory->GetProductionState()))
	    .Times(2)
	    .WillRepeatedly(Return(1000));

	factory->Update();
	factory->LateUpdate();

	// Factory is now in PRODUCTION state
	ASSERT_EQ(factory->GetState(), FactoryStateName::PRODUCTION);
	ASSERT_EQ(factory->GetProductionState(), ActorType::VILLAGER);
}

TEST_F(FactoryTest, UnitProductionTest) {
	ASSERT_EQ(factory->GetState(), FactoryStateName::UNBUILT);

	// Build the factory
	factory->IncrementConstructionCompletion(
	    factory->GetTotalConstructionCompletion());

	// Expecting gold manager calls when factory transitions from UNBUILD state
	// to PRODUCTION state
	EXPECT_CALL(*gold_manager, GetCreateUnitCost)
	    .Times(2)
	    .WillRepeatedly(Return(1000));
	EXPECT_CALL(*gold_manager, GetBalance(factory->GetPlayerId()))
	    .Times(2)
	    .WillRepeatedly(Return(1000));

	// Updating the factory state
	factory->Update();
	factory->LateUpdate();

	// Factory is now in PRODUCTION state
	ASSERT_EQ(factory->GetState(), FactoryStateName::PRODUCTION);

	// Producing villagers now
	factory->SetProductionState(ActorType::VILLAGER);

	// Expecting gold manager calls
	EXPECT_CALL(*gold_manager, GetCreateUnitCost)
	    .Times(21 * villager_frequency)
	    .WillRepeatedly(Return(1000));
	EXPECT_CALL(*gold_manager, GetBalance(factory->GetPlayerId()))
	    .Times(21 * villager_frequency)
	    .WillRepeatedly(Return(1000));

	// The factory creates a soldier in the first tick itself
	ASSERT_EQ(villager_list.size(), 1);

	// After frequency no. of updates, a villager should have been produced
	for (int i = 0; i < villager_frequency; ++i) {
		factory->Update();
		factory->LateUpdate();
	}

	ASSERT_EQ(villager_list.size(), 2);

	// After 20 x frequency updates, 20 more villagers should have been produced
	for (int i = 0; i < 20 * villager_frequency; ++i) {
		factory->Update();
		factory->LateUpdate();
	}

	ASSERT_EQ(villager_list.size(), 2 + 20);
}

TEST_F(FactoryTest, SwitchUnitProductionTest) {
	ASSERT_EQ(factory->GetState(), FactoryStateName::UNBUILT);

	// Build the factory
	factory->IncrementConstructionCompletion(
	    factory->GetTotalConstructionCompletion());

	factory->SetProductionState(ActorType::VILLAGER);

	// Gold manager expecting calls
	EXPECT_CALL(*gold_manager, GetBalance(factory->GetPlayerId()))
	    .Times(2)
	    .WillRepeatedly(Return(1000));
	EXPECT_CALL(*gold_manager, GetCreateUnitCost(factory->GetProductionState()))
	    .Times(2)
	    .WillRepeatedly(Return(1000));

	// Transitioning the villager from the UNBUILT state to the PRODUCTION
	factory->Update();
	factory->LateUpdate();

	ASSERT_EQ(factory->GetState(), FactoryStateName::PRODUCTION);

	// Gold manager expecting calls
	// Need to check why
	EXPECT_CALL(*gold_manager, GetBalance(factory->GetPlayerId()))
	    .Times(villager_frequency - 1)
	    .WillRepeatedly(Return(1000));
	EXPECT_CALL(*gold_manager, GetCreateUnitCost(factory->GetProductionState()))
	    .Times(villager_frequency - 1)
	    .WillRepeatedly(Return(1000));

	// After frequency no. of updates, a villager should have been produced
	for (int i = 0; i < villager_frequency - 1; ++i) {
		factory->Update();
		factory->LateUpdate();
	}

	ASSERT_EQ(villager_list.size(), 1);

	// Let's swap to soldiers
	factory->SetProductionState(ActorType::SOLDIER);

	// After 20 x frequency updates, 20 soldiers should have been produced
	EXPECT_CALL(*gold_manager, GetBalance(factory->GetPlayerId()))
	    .Times(20 * soldier_frequency)
	    .WillRepeatedly(Return(1000));
	EXPECT_CALL(*gold_manager, GetCreateUnitCost(factory->GetProductionState()))
	    .Times(20 * soldier_frequency)
	    .WillRepeatedly(Return(1000));

	for (int i = 0; i < 20 * soldier_frequency; ++i) {
		factory->Update();
		factory->LateUpdate();
	}

	ASSERT_EQ(soldier_list.size(), 20);
}

TEST_F(FactoryTest, StopStartTest) {
	ASSERT_EQ(factory->GetState(), FactoryStateName::UNBUILT);

	// Setting the production state to villager
	factory->SetProductionState(ActorType::SOLDIER);

	// Build the factory and produce a unit
	factory->IncrementConstructionCompletion(
	    factory->GetTotalConstructionCompletion());

	// Expecting calls to gold manager
	EXPECT_CALL(*gold_manager, GetBalance(factory->GetPlayerId()))
	    .Times(2)
	    .WillRepeatedly(Return(1000));
	EXPECT_CALL(*gold_manager, GetCreateUnitCost(factory->GetProductionState()))
	    .Times(2)
	    .WillRepeatedly(Return(1000));

	factory->Update();
	factory->LateUpdate();

	// The factory has transitioned from unbuilt state to production state
	ASSERT_EQ(factory->GetState(), FactoryStateName::PRODUCTION);

	// Expect calls to GoldManager for getting balance and unit creation cost
	EXPECT_CALL(*gold_manager, GetBalance(factory->GetPlayerId()))
	    .Times(soldier_frequency - 1)
	    .WillRepeatedly(Return(1000));
	EXPECT_CALL(*gold_manager, GetCreateUnitCost(factory->GetProductionState()))
	    .Times(soldier_frequency - 1)
	    .WillRepeatedly(Return(1000));

	// Produce a unit (- 1 since we ran 1 update just now)
	for (int i = 0; i < soldier_frequency - 1; ++i) {
		factory->Update();
		factory->LateUpdate();
	}

	// Expecting 2 calls to gold manager for transisioning from the PRODUCTION
	// state to the idle state
	EXPECT_CALL(*gold_manager, GetBalance(factory->GetPlayerId()))
	    .Times(2)
	    .WillRepeatedly(Return(1000));
	EXPECT_CALL(*gold_manager, GetCreateUnitCost)
	    .Times(2)
	    .WillRepeatedly(Return(1000));

	// Stop Production
	factory->Stop();
	factory->Update();
	factory->LateUpdate();
	ASSERT_EQ(factory->GetState(), FactoryStateName::IDLE);

	// NOTE: Factory is now in idle state
	// Hence, it will call the GetBalance and the GetCreateUnitCost function
	// every turn

	// Expecting 2 more calls to gold manager for resuming production
	EXPECT_CALL(*gold_manager, GetBalance(factory->GetPlayerId()))
	    .Times(2)
	    .WillRepeatedly(Return(1000));
	EXPECT_CALL(*gold_manager, GetCreateUnitCost)
	    .Times(2)
	    .WillRepeatedly(Return(1000));

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

	// The factory is now in the IDLE state
	// Build the factory
	factory->IncrementConstructionCompletion(
	    factory->GetTotalConstructionCompletion());

	// Calls are made in the idle state to GetCreateUnitCost and GetBalance
	EXPECT_CALL(*gold_manager, GetBalance(factory->GetPlayerId()))
	    .Times(2)
	    .WillRepeatedly(Return(1000));
	EXPECT_CALL(*gold_manager, GetCreateUnitCost(factory->GetProductionState()))
	    .Times(2)
	    .WillRepeatedly(Return(1000));
	factory->Update();
	factory->LateUpdate();

	// Factory is now in production state
	ASSERT_EQ(factory->GetState(), FactoryStateName::PRODUCTION);

	// Kill it
	factory->SetHp(0);
	factory->Update();
	factory->LateUpdate();
	ASSERT_EQ(factory->GetState(), FactoryStateName::DEAD);
}
