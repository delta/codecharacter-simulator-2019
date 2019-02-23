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
	unique_ptr<ScoreManager> score_manager;
	unique_ptr<PathPlanner> path_planner;
	Villager model_villager;
	Soldier model_soldier;
	Factory model_factory;
	int64_t interest_threshold;

	std::array<std::vector<std::unique_ptr<Soldier>>, 2> soldiers;
	std::array<std::vector<std::unique_ptr<Villager>>, 2> villagers;
	std::array<std::vector<std::unique_ptr<Factory>>, 2> factories;

	unique_ptr<State> state;

  public:
	StateTest() {
		auto map_matrix = vector<vector<TerrainType>>{{
		    {L, L, L, W, W},
		    {L, L, L, L, W},
		    {L, L, L, L, L},
		    {W, L, L, L, L},
		    {W, W, L, L, L},
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

		this->score_manager = make_unique<ScoreManager>();

		this->path_planner = make_unique<PathPlanner>(map.get());

		this->model_villager = Villager(
		    1, PlayerId::PLAYER1, ActorType::VILLAGER, 100, 100,
		    DoubleVec2D(10, 10), gold_manager.get(), score_manager.get(),
		    path_planner.get(), 10, 10, 10, 10, 10, 10);

		this->model_soldier =
		    Soldier(2, PlayerId::PLAYER2, ActorType::SOLDIER, 100, 100,
		            DoubleVec2D(10, 10), gold_manager.get(),
		            score_manager.get(), path_planner.get(), 10, 10, 10);

		int64_t villager_frequency = 5;
		int64_t soldier_frequency = 10;
		int64_t interest_threshold = 0;

		this->model_factory = Factory(
		    2, PlayerId::PLAYER2, ActorType::FACTORY, 1, 100,
		    DoubleVec2D(15, 15), gold_manager.get(), score_manager.get(), 0,
		    100, ActorType::VILLAGER, villager_frequency, soldier_frequency,
		    UnitProductionCallback{});

		soldiers = {};
		villagers = {};
		factories = {};

		// Player1 has 2 villagers say..
		villagers[0].push_back(make_unique<Villager>(
		    1, PlayerId::PLAYER1, ActorType::VILLAGER, 100, 100,
		    DoubleVec2D(0, 0), gold_manager.get(), score_manager.get(),
		    path_planner.get(), 10, 10, 10, 10, 10, 10));
		villagers[0].push_back(make_unique<Villager>(
		    3, PlayerId::PLAYER1, ActorType::VILLAGER, 100, 100,
		    DoubleVec2D(0, 0), gold_manager.get(), score_manager.get(),
		    path_planner.get(), 10, 10, 10, 10, 10, 10));

		// Player 2 has 1 villager
		villagers[1].push_back(make_unique<Villager>(
		    4, PlayerId::PLAYER1, ActorType::VILLAGER, 100, 100,
		    DoubleVec2D(49, 49), gold_manager.get(), score_manager.get(),
		    path_planner.get(), 10, 10, 10, 10, 10, 10));

		// Player2 has 1 soldier, say..
		soldiers[1].push_back(make_unique<Soldier>(
		    2, PlayerId::PLAYER2, ActorType::SOLDIER, 100, 100,
		    DoubleVec2D(10, 10), gold_manager.get(), score_manager.get(),
		    path_planner.get(), 10, 10, 10));

		Actor::SetActorIdIncrement(4);

		this->state = make_unique<State>(
		    move(map), move(gold_manager), move(score_manager),
		    move(path_planner), move(soldiers), move(villagers),
		    move(factories), move(model_villager), move(model_soldier),
		    move(model_factory), interest_threshold);
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
	state->CreateFactory(PlayerId::PLAYER1, 1, Vec2D(0, 0), ActorType::SOLDIER);
	auto curr_villagers = state->GetVillagers();
	auto curr_factories = state->GetFactories();
	auto villager = curr_villagers[0].front();

	// Updating the state and soldier
	while (villager->GetState() != VillagerStateName::BUILD) {
		state->Update();
	}

	// Checking if the villager has transitioned into the BUILD state
	ASSERT_EQ(villager->GetState(), VillagerStateName::BUILD);

	// Checking if the number of state factories increased and the factory is in
	// UNBUILT state
	curr_factories = state->GetFactories();
	auto factory = curr_factories[0].front();
	ASSERT_EQ(curr_factories[0].size(), 1);
	ASSERT_EQ(factory->GetState(), FactoryStateName::UNBUILT);

	auto gold_manager = villager->GetGoldManager();
	ASSERT_EQ(gold_manager->GetBalance(PlayerId::PLAYER1), 5000 - FACTORY_COST);
}

TEST_F(StateTest, BuildFactoryTest) {
	// Repeat CreateFactory as above..
	state->CreateFactory(PlayerId::PLAYER1, 1, Vec2D(0, 0), ActorType::SOLDIER);
	auto curr_villagers = state->GetVillagers();
	auto curr_factories = state->GetFactories();
	auto villager = curr_villagers[0].front();

	// Updating the villager's state
	while (villager->GetState() != VillagerStateName::BUILD) {
		state->Update();
	}

	ASSERT_EQ(villager->GetState(), VillagerStateName::BUILD);

	// Getting the updated factories from state
	curr_factories = state->GetFactories();
	auto factory = curr_factories[0].front();
	ASSERT_EQ(curr_factories[0].size(), 1);
	ASSERT_EQ(factory->GetState(), FactoryStateName::UNBUILT);

	// Test BuildFactory for the same villager..
	state->BuildFactory(PlayerId::PLAYER1, 1, factory->GetActorId());

	// Updating state again
	state->Update();

	ASSERT_EQ(curr_villagers[0].front()->GetState(), VillagerStateName::BUILD);
	ASSERT_EQ(curr_factories[0].size(), 1);

	// Test BuildFactory for the other villager..
	state->BuildFactory(PlayerId::PLAYER1, 3, factory->GetActorId());

	state->Update();

	ASSERT_EQ(curr_villagers[0][1]->GetState(), VillagerStateName::BUILD);
	ASSERT_EQ(curr_factories[0].size(), 1);
}

TEST_F(StateTest, FactoryProductionTest) {
	// Repeat CreateFactory as above..
	state->CreateFactory(PlayerId::PLAYER1, 1, Vec2D(0, 0), ActorType::SOLDIER);
	auto curr_villagers = state->GetVillagers();
	auto curr_factories = state->GetFactories();
	auto villager = curr_villagers[0].front();

	// Updating the villager's state
	while (villager->GetState() != VillagerStateName::BUILD) {
		state->Update();
	}

	ASSERT_EQ(villager->GetState(), VillagerStateName::BUILD);

	// Getting the updated state factories
	curr_factories = state->GetFactories();
	auto factory = curr_factories[0].front();
	ASSERT_EQ(curr_factories[0].size(), 1);
	ASSERT_EQ(factory->GetState(), FactoryStateName::UNBUILT);

	// Test SetFactoryProduction
	ASSERT_EQ(factory->GetProductionState(), ActorType::SOLDIER);

	state->SetFactoryProduction(PlayerId::PLAYER1, factory->GetActorId(),
	                            ActorType::SOLDIER);

	state->Update();

	ASSERT_EQ(factory->GetProductionState(), ActorType::SOLDIER);
}

TEST_F(StateTest, FactoryDeathTest) {
	// Repeat CreateFactory as above..
	state->CreateFactory(PlayerId::PLAYER1, 1, Vec2D(0, 0), ActorType::SOLDIER);
	auto curr_villagers = state->GetVillagers();
	auto curr_factories = state->GetFactories();
	ASSERT_EQ(curr_factories[0].size(), 0);
	auto villager = curr_villagers[0].front();

	// Updating the villager's state
	while (villager->GetState() != VillagerStateName::BUILD) {
		state->Update();
	}

	ASSERT_EQ(villager->GetState(), VillagerStateName::BUILD);

	// Updating current factories from the state
	curr_factories = state->GetFactories();
	auto factory = curr_factories[0].front();
	ASSERT_EQ(curr_factories[0].size(), 1);
	ASSERT_EQ(factory->GetState(), FactoryStateName::UNBUILT);

	// Killing the factory
	factory->SetHp(0);
	factory->Update();
	factory->LateUpdate();

	// Checking if the factory has transitioned to dead state
	ASSERT_EQ(factory->GetState(), FactoryStateName::DEAD);
	ASSERT_EQ(factory->GetHp(), 0);

	// Updating the state
	state->Update();

	auto new_facs = state->GetFactories();
	ASSERT_EQ(new_facs[0].size(), 0);
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

TEST_F(StateTest, SimultaneousBuild) {
	// Making both the villager try and build a factory at the same time
	auto villagers = state->GetVillagers();
	auto villager1 = villagers[0].front();
	auto villager2 = villagers[1].front();

	// Checking the initial number of factories in state
	auto factories = state->GetFactories();
	ASSERT_EQ(factories[0].size(), 0);
	ASSERT_EQ(factories[1].size(), 0);

	// Making both the villagers try and build at the same position
	state->CreateFactory(PlayerId::PLAYER1, villager1->GetActorId(),
	                     Vec2D(2, 0), ActorType::SOLDIER);
	state->CreateFactory(PlayerId::PLAYER2, villager2->GetActorId(),
	                     Vec2D(2, 0), ActorType::SOLDIER);

	// Calling state update to check if the build requests have been assigned
	// properly
	state->Update();

	// Checking to make sure factory didn't get added into state
	factories = state->GetFactories();
	ASSERT_EQ(factories[0].size(), 0);
	ASSERT_EQ(factories[1].size(), 0);
}

TEST_F(StateTest, SimultanousGameOverByDeathMatch) {
	// Kill all soldiers and villagers
	auto current_soldiers = state->GetSoldiers();
	for (auto &player_soldiers : current_soldiers)
		for (auto &soldier : player_soldiers)
			soldier->SetHp(0);

	auto current_villagers = state->GetVillagers();
	for (auto &player_villagers : current_villagers)
		for (auto &villager : player_villagers)
			villager->SetHp(0);

	// Call update
	state->Update();

	PlayerId winner;
	auto is_game_over = state->IsGameOver(winner);

	ASSERT_TRUE(is_game_over);
	ASSERT_EQ(winner, PlayerId::PLAYER_NULL);
}

TEST_F(StateTest, Player1GameOverByDeathMatch) {
	// Kill all of Player2's soldiers and villagers
	auto player2_soldiers = state->GetSoldiers()[1];
	for (auto &soldier : player2_soldiers)
		soldier->SetHp(0);

	auto player2_villagers = state->GetVillagers()[1];
	for (auto &villager : player2_villagers)
		villager->SetHp(0);

	// Call update
	state->Update();

	PlayerId winner;
	auto is_game_over = state->IsGameOver(winner);

	ASSERT_TRUE(is_game_over);
	ASSERT_EQ(winner, PlayerId::PLAYER1);
}

TEST_F(StateTest, Player2GameOverByDeathMatch) {
	// Kill all of Player2's soldiers and villagers
	auto player2_soldiers = state->GetSoldiers()[0];
	for (auto &soldier : player2_soldiers)
		soldier->SetHp(0);

	auto player2_villagers = state->GetVillagers()[0];
	for (auto &villager : player2_villagers)
		villager->SetHp(0);

	// Call update
	state->Update();

	PlayerId winner;
	auto is_game_over = state->IsGameOver(winner);

	ASSERT_TRUE(is_game_over);
	ASSERT_EQ(winner, PlayerId::PLAYER2);
}

TEST_F(StateTest, SimultaneousAttackAndGameOver) {
	// Kill the second villager in player1 and the soldier in player2
	auto soldiers = state->GetSoldiers();
	auto villagers = state->GetVillagers();

	villagers[0][1]->SetHp(0);
	soldiers[1][0]->SetHp(0);

	state->Update();

	soldiers = state->GetSoldiers();
	villagers = state->GetVillagers();

	// Now we should just have a villager on each team
	ASSERT_EQ(soldiers[0].size(), 0);
	ASSERT_EQ(soldiers[1].size(), 0);
	ASSERT_EQ(villagers[0].size(), 1);
	ASSERT_EQ(villagers[1].size(), 1);

	// Make them attack each other
	auto villager1 = villagers[0][0];
	auto villager2 = villagers[1][0];

	villager1->SetAttackTarget(villager2);
	villager2->SetAttackTarget(villager1);
	state->Update();

	// Ensure that both villagers snap into pursuit and move towards each other
	while (villager1->GetState() == VillagerStateName::PURSUIT) {
		ASSERT_EQ(villager2->GetState(), VillagerStateName::PURSUIT);
		state->Update();
	}

	// Now, both villagers should have switched to attack
	ASSERT_EQ(villager1->GetState(), VillagerStateName::ATTACK);
	ASSERT_EQ(villager2->GetState(), VillagerStateName::ATTACK);

	while (villager1->GetState() == VillagerStateName::ATTACK) {
		ASSERT_EQ(villager2->GetState(), VillagerStateName::ATTACK);
		state->Update();

		// If at any point they die,
		villagers = state->GetVillagers();
		if (!(villagers[0].size() || villagers[1].size())) {
			break;
		}
	}

	// Now, both villagers should be dead
	villagers = state->GetVillagers();
	ASSERT_EQ(villagers[0].size(), 0);
	ASSERT_EQ(villagers[1].size(), 0);

	// The game should be over
	PlayerId winner;
	auto is_game_over = state->IsGameOver(winner);

	ASSERT_TRUE(is_game_over);
	ASSERT_EQ(winner, PlayerId::PLAYER_NULL);
}
