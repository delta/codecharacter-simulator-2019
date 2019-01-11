#include "constants/gold_manager.h"
#include "constants/state.h"
#include "gmock/gmock.h"
#include "state/state.h"
#include "gtest/gtest.h"

using namespace std;
using namespace state;
using namespace physics;
using namespace testing;

const auto L = TerrainType::LAND;
const auto W = TerrainType::WATER;
const auto G = TerrainType::GOLD_MINE;

using Vec2D = physics::Vector<int64_t>;

class StateTest : public Test {
  protected:
	int64_t map_size = 5;
	int64_t element_size = 10;
	unique_ptr<Map> map;

	array<int64_t, 2> player_gold;
	int64_t soldier_kill_reward_gold;
	int64_t villager_kill_reward_gold;
	int64_t factory_kill_reward_gold;
	int64_t max_gold;

	std::vector<std::unique_ptr<Villager>> temp_villager_list;
	std::vector<std::unique_ptr<Soldier>> temp_soldier_list;

	unique_ptr<GoldManager> gold_manager;
	unique_ptr<PathPlanner> path_planner;
	Villager model_villager;
	Soldier model_soldier;
	Factory model_factory;

	std::array<std::vector<std::unique_ptr<Soldier>>, 2> soldiers;
	std::array<std::vector<std::unique_ptr<Villager>>, 2> villagers;
	std::array<std::vector<std::unique_ptr<Factory>>, 2> factories;

	unique_ptr<State> state;

  public:
	StateTest() {
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

		this->model_villager =
		    Villager(1, PlayerId::PLAYER1, ActorType::VILLAGER, 100, 100,
		             physics::Vector<int64_t>(10, 10), gold_manager.get(),
		             path_planner.get(), 10, 10, 10, 10, 10, 10);

		this->model_soldier =
		    Soldier(2, PlayerId::PLAYER2, ActorType::SOLDIER, 100, 100,
		            physics::Vector<int64_t>(10, 10), gold_manager.get(),
		            path_planner.get(), 10, 10, 10);

		int64_t villager_frequency = 5;
		int64_t soldier_frequency = 10;

		this->model_factory =
		    Factory(2, PlayerId::PLAYER2, ActorType::FACTORY_VILLAGER, 1, 100,
		            physics::Vector<int64_t>(15, 15), gold_manager.get(), 0,
		            100, ActorType::VILLAGER, villager_frequency,
		            soldier_frequency, UnitProductionCallback{});

		soldiers = {};
		villagers = {};
		factories = {};

		// Player1 has 2 villagers say..
		villagers[0].push_back(make_unique<Villager>(
		    1, PlayerId::PLAYER1, ActorType::VILLAGER, 100, 100,
		    physics::Vector<int64_t>(10, 10), gold_manager.get(),
		    path_planner.get(), 10, 10, 10, 10, 10, 10));
		villagers[0].push_back(make_unique<Villager>(
		    3, PlayerId::PLAYER1, ActorType::VILLAGER, 100, 100,
		    physics::Vector<int64_t>(10, 10), gold_manager.get(),
		    path_planner.get(), 10, 10, 10, 10, 10, 10));

		// Player2 has 1 soldier, say..
		soldiers[1].push_back(make_unique<Soldier>(
		    2, PlayerId::PLAYER2, ActorType::SOLDIER, 100, 100,
		    physics::Vector<int64_t>(10, 10), gold_manager.get(),
		    path_planner.get(), 10, 10, 10));

		Actor::SetActorIdIncrement(4);

		this->state = make_unique<State>(
		    move(map), move(gold_manager), move(path_planner), move(soldiers),
		    move(villagers), move(factories), move(model_villager),
		    move(model_soldier), move(model_factory));
	}
};

TEST_F(StateTest, MoveUnitTest) {
	state->MoveUnit(PlayerId::PLAYER1, 1, Vec2D(5, 5));
	auto curr_villagers = state->GetVillagers();
	auto curr_villager = curr_villagers[0].front();

	state->Update();

	ASSERT_EQ(curr_villager->GetState(), VillagerStateName::MOVE);
	ASSERT_TRUE(curr_villager->IsDestinationSet());
	ASSERT_EQ(curr_villager->GetDestination(), Vec2D(5, 5));
}

TEST_F(StateTest, MineLocationTest) {
	state->MineLocation(PlayerId::PLAYER1, 1, Vec2D(25, 25));
	auto curr_villagers = state->GetVillagers();
	auto curr_villager = curr_villagers[0].front();

	state->Update();

	ASSERT_TRUE(curr_villager->GetState() == VillagerStateName::MOVE_TO_MINE ||
	            curr_villager->GetState() == VillagerStateName::MINE);
	ASSERT_TRUE(curr_villager->IsMineTargetSet());
	ASSERT_EQ(curr_villager->GetMineTarget(), Vec2D(25, 25));
}

TEST_F(StateTest, VillagerAttackEnemyTest) {
	state->AttackActor(PlayerId::PLAYER1, 1, 2);
	auto curr_villagers = state->GetVillagers();
	auto curr_villager = curr_villagers[0].front();

	auto curr_soldiers = state->GetSoldiers();
	auto target_soldier = curr_soldiers[1].front();

	state->Update();

	ASSERT_TRUE(curr_villager->GetState() == VillagerStateName::PURSUIT ||
	            curr_villager->GetState() == VillagerStateName::ATTACK);
	ASSERT_TRUE(curr_villager->IsAttackTargetSet());
	ASSERT_EQ(curr_villager->GetAttackTarget(), target_soldier);
}

TEST_F(StateTest, SoldierAttackEnemyTest) {
	state->AttackActor(PlayerId::PLAYER2, 2, 1);
	auto curr_soldiers = state->GetSoldiers();
	auto curr_soldier = curr_soldiers[1].front();

	auto curr_villagers = state->GetVillagers();
	auto target_villager = curr_villagers[0].front();

	state->Update();

	ASSERT_TRUE(curr_soldier->GetState() == SoldierStateName::PURSUIT ||
	            curr_soldier->GetState() == SoldierStateName::ATTACK);
	ASSERT_TRUE(curr_soldier->IsAttackTargetSet());
	ASSERT_EQ(curr_soldier->GetAttackTarget(), target_villager);
}

TEST_F(StateTest, CreateFactoryTest) {
	state->CreateFactory(PlayerId::PLAYER1, 1, Vec2D(0, 0));
	auto curr_villagers = state->GetVillagers();
	auto curr_factories = state->GetFactories();

	state->Update();

	ASSERT_EQ(curr_villagers[0].front()->GetState(), VillagerStateName::BUILD);
	ASSERT_EQ(curr_factories[0].size(), 1);
	ASSERT_EQ(curr_factories[0].front()->GetState(), FactoryStateName::UNBUILT);

	auto gold_manager = curr_villagers[0].front()->GetGoldManager();
	ASSERT_EQ(gold_manager->GetBalance(PlayerId::PLAYER1), 5000 - FACTORY_COST);
}

TEST_F(StateTest, BuildFactoryTest) {
	// Repeat CreateFactory as above..
	state->CreateFactory(PlayerId::PLAYER1, 1, Vec2D(0, 0));
	auto curr_villagers = state->GetVillagers();
	auto curr_factories = state->GetFactories();

	state->Update();

	ASSERT_EQ(curr_villagers[0].front()->GetState(), VillagerStateName::BUILD);
	ASSERT_EQ(curr_factories[0].size(), 1);
	ASSERT_EQ(curr_factories[0].front()->GetState(), FactoryStateName::UNBUILT);

	// Test BuildFactory for the same villager..
	auto curr_factory = curr_factories[0].front();
	state->BuildFactory(PlayerId::PLAYER1, 1, curr_factory->GetActorId());

	state->Update();

	ASSERT_EQ(curr_villagers[0].front()->GetState(), VillagerStateName::BUILD);
	ASSERT_EQ(curr_factories[0].size(), 1);

	// Test BuildFactory for the other villager..
	state->BuildFactory(PlayerId::PLAYER1, 3, curr_factory->GetActorId());

	state->Update();

	ASSERT_EQ(curr_villagers[0][1]->GetState(), VillagerStateName::BUILD);
	ASSERT_EQ(curr_factories[0].size(), 1);
}

TEST_F(StateTest, FactoryProductionTest) {
	// Repeat CreateFactory as above..
	state->CreateFactory(PlayerId::PLAYER1, 1, Vec2D(0, 0));
	auto curr_villagers = state->GetVillagers();
	auto curr_factories = state->GetFactories();

	state->Update();

	ASSERT_EQ(curr_villagers[0].front()->GetState(), VillagerStateName::BUILD);
	ASSERT_EQ(curr_factories[0].size(), 1);
	ASSERT_EQ(curr_factories[0].front()->GetState(), FactoryStateName::UNBUILT);

	// Test SetFactoryProduction
	auto curr_factory = curr_factories[0].front();
	ASSERT_EQ(curr_factory->GetProductionState(), ActorType::VILLAGER);

	state->SetFactoryProduction(PlayerId::PLAYER1, curr_factory->GetActorId(),
	                            ActorType::SOLDIER);

	state->Update();

	ASSERT_EQ(curr_factory->GetProductionState(), ActorType::SOLDIER);
}

TEST_F(StateTest, FactoryDeathTest) {
	// Repeat CreateFactory as above..
	state->CreateFactory(PlayerId::PLAYER1, 1, Vec2D(0, 0));
	auto curr_villagers = state->GetVillagers();
	auto curr_factories = state->GetFactories();

	state->Update();

	ASSERT_EQ(curr_villagers[0].front()->GetState(), VillagerStateName::BUILD);
	ASSERT_EQ(curr_factories[0].size(), 1);
	ASSERT_EQ(curr_factories[0].front()->GetState(), FactoryStateName::UNBUILT);

	// Kill factory
	auto curr_factory = curr_factories[0].front();
	curr_factory->SetHp(0);
	state->Update();

	auto new_factories = state->GetFactories();
	ASSERT_EQ(new_factories[0].size(), 0);
}

TEST_F(StateTest, VillagerDeathTest) {
	auto curr_villagers = state->GetVillagers();
	ASSERT_EQ(curr_villagers[0].size(), 2);

	auto curr_villager = curr_villagers[0].front();
	curr_villager->SetHp(0);
	state->Update();

	auto new_villagers = state->GetVillagers();
	ASSERT_EQ(new_villagers[0].size(), 1);
}

TEST_F(StateTest, SoldierDeathTest) {
	auto curr_soldiers = state->GetSoldiers();
	ASSERT_EQ(curr_soldiers[1].size(), 1);

	auto curr_soldier = curr_soldiers[1].front();
	curr_soldier->SetHp(0);
	state->Update();

	auto new_soldiers = state->GetSoldiers();
	ASSERT_EQ(new_soldiers[1].size(), 0);
}
