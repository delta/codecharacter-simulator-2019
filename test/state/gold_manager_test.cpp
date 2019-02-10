#include "state/gold_manager/gold_manager.h"
#include "state/map/map.h"
#include "state/mocks/command_giver_mock.h"
#include "state/mocks/command_taker_mock.h"
#include "state/mocks/gold_manager_mock.h"
#include "state/mocks/state_syncer_mock.h"
#include "gtest/gtest.h"
#include <memory>

using namespace std;
using namespace state;

class GoldManagerTest : public testing::Test {
  public:
	// Creating a path planner
	unique_ptr<PathPlanner> path_planner;
	unique_ptr<Map> map;

	// Gold manager's dependencies
	array<int64_t, 2> player_gold;
	int64_t max_gold;
	int64_t soldier_kill_amount;
	int64_t villager_kill_amount;
	int64_t factory_kill_amount;
	int64_t villager_cost;
	int64_t soldier_cost;
	int64_t factory_cost;
	int64_t mining_reward;
	vector<GoldMine *> gold_mines;

	// Creating an incrementing actor id
	int64_t actor_id;

	// Creating the gold manager
	unique_ptr<GoldManager> gold_manager;

	GoldManagerTest() {

		// Assigning the GoldManager constants
		this->player_gold = {1000, 1000};
		this->max_gold = 10000;
		this->soldier_kill_amount = 100;
		this->villager_kill_amount = 50;
		this->factory_kill_amount = 200;
		this->villager_cost = 100;
		this->soldier_cost = 200;
		this->factory_cost = 100;
		this->mining_reward = 10;
		int64_t map_size = 5;
		int64_t ele_size = 5;

		// Creating gold mines
		vector<unique_ptr<GoldMine>> uniq_gold_mines;
		auto uniq_gold_mine = make_unique<GoldMine>(Vec2D(2, 2), 10000);
		auto gold_mine = uniq_gold_mine.get();
		uniq_gold_mines.push_back(move(uniq_gold_mine));
		this->gold_mines.push_back(gold_mine);

		// Creating a rudimentary map
		vector<vector<TerrainType>> dummy_map = CreateMap(map_size);
		this->map = make_unique<Map>(dummy_map, map_size, ele_size);

		// Assinging the path planner and the gold manager
		this->path_planner = make_unique<PathPlanner>(this->map.get());
		this->gold_manager = make_unique<GoldManager>(
		    player_gold, max_gold, soldier_kill_amount, villager_kill_amount,
		    factory_kill_amount, villager_cost, soldier_cost, factory_cost,
		    mining_reward, move(uniq_gold_mines));
	}

	unique_ptr<Soldier> MakeStateSoldier(PlayerId player_id) {
		return make_unique<Soldier>(this->actor_id++, player_id,
		                            ActorType::SOLDIER, 100, 100,
		                            DoubleVec2D(0, 0), this->gold_manager.get(),
		                            this->path_planner.get(), 5, 5, 5);
	}

	unique_ptr<Villager> MakeStateVillager(PlayerId player_id) {
		return make_unique<Villager>(
		    this->actor_id++, player_id, ActorType::VILLAGER, 100, 100,
		    DoubleVec2D(0, 0), this->gold_manager.get(),
		    this->path_planner.get(), 5, 5, 5, 3, 3, 3);
	}

	unique_ptr<Factory> MakeStateFactory(PlayerId player_id) {
		return make_unique<Factory>(
		    this->actor_id++, player_id, ActorType::FACTORY, 100, 100,
		    DoubleVec2D(0, 0), this->gold_manager.get(), 100, 100,
		    ActorType::SOLDIER, 3, 3, UnitProductionCallback{});
	}

	vector<vector<TerrainType>> CreateMap(int64_t map_size) {
		/**
		 * Creating this map layout
		 *
		 * W L L L L
		 * L W L L L
		 * L L G L L
		 * L L L W L
		 * L L L L W
		 */

		vector<vector<TerrainType>> dummy_map;
		for (int i = 0; i < map_size; ++i) {
			vector<TerrainType> dummy_row;
			for (int j = 0; j < map_size; ++j) {
				if (i == j) {
					if (i == map_size / 2) {
						dummy_row.push_back(TerrainType::GOLD_MINE);
					} else {
						dummy_row.push_back(TerrainType::WATER);
					}
				} else {
					dummy_row.push_back(TerrainType::LAND);
				}
			}
			dummy_map.push_back(dummy_row);
		}
		return dummy_map;
	}
};

// Starting tests

// Gets called when a villager tries to mine a gold mine
TEST_F(GoldManagerTest, AddMineRequest) {
	auto villager = MakeStateVillager(PlayerId::PLAYER1);
	auto gold_mine = this->gold_mines[0];
	gold_mine->SetGoldReserve(100);
	int64_t player_id = static_cast<int64_t>(PlayerId::PLAYER1);
	villager->SetMineTarget(Vec2D(2, 2));

	// Updating the villager's state fror IDLE to MINE
	// A mine request will be sent to the GoldManager when the soldier
	// transitions into MINE state

	while (villager->GetState() != VillagerStateName::MINE) {
		villager->Update();
		villager->LateUpdate();
	}

	// Villager is now in the mining state
	ASSERT_EQ(villager->GetState(), VillagerStateName::MINE);

	// Updating gold manager to check if player 1's gold increases
	gold_manager->Update();

	ASSERT_EQ(gold_manager->GetBalance(villager->GetPlayerId()),
	          this->player_gold[0] + this->mining_reward);
}

TEST_F(GoldManagerTest, SplitGoldTest) {
	// Reassigning the GoldMine gold to have 16 gold units
	auto gold_mine = this->gold_mines[0];
	gold_mine->SetGoldReserve(16);

	// Making a villager each for player 1 and player 2
	// Since both villagers start at the same position, they will reach the mine
	// at the same time
	auto villager1 = MakeStateVillager(PlayerId::PLAYER1);
	auto villager2 = MakeStateVillager(PlayerId::PLAYER2);
	villager1->SetMineTarget(Vec2D(2, 2));
	villager2->SetMineTarget(Vec2D(2, 2));

	// Making villager transition to MINE state
	while (villager1->GetState() != VillagerStateName::MINE) {
		villager1->Update();
		villager2->Update();
		villager1->LateUpdate();
		villager2->LateUpdate();
	}

	// Once in MINE state, call the gold manager update to check if the gold is
	// divided in the right ratio
	gold_manager->Update();

	ASSERT_EQ(gold_manager->GetBalance(PlayerId::PLAYER1),
	          this->player_gold[0] + 8);
	ASSERT_EQ(gold_manager->GetBalance(PlayerId::PLAYER2),
	          this->player_gold[1] + 8);
	ASSERT_EQ(gold_mine->GetGoldReserve(), 0);

	// Updating the player gold in the test
	this->player_gold[0] += 8;
	this->player_gold[1] += 8;

	// Reassigning gold manager to have an odd number and checking if the
	// remaining gold is removed
	gold_mine->SetGoldReserve(28);

	// Creating an additional player 2 villager and resetting the first 2
	// villagers
	villager1 = MakeStateVillager(PlayerId::PLAYER1);
	villager2 = MakeStateVillager(PlayerId::PLAYER2);
	auto villager3 = MakeStateVillager(PlayerId::PLAYER2);

	// Setting the villagers' mine targets
	villager1->SetMineTarget(Vec2D(2, 2));
	villager2->SetMineTarget(Vec2D(2, 2));
	villager3->SetMineTarget(Vec2D(2, 2));

	// Since all the villagers have been reset, they all are in the same
	// position
	while (villager1->GetState() != VillagerStateName::MINE) {
		villager1->Update();
		villager1->LateUpdate();
		villager2->Update();
		villager2->LateUpdate();
		villager3->Update();
		villager3->LateUpdate();
	}

	// Once in MINE state, call the gold manager update to check if the gold is
	// divided in the right ratio
	gold_manager->Update();

	// Checking if the players got the right ratio of gold
	ASSERT_EQ(gold_manager->GetBalance(PlayerId::PLAYER1),
	          this->player_gold[0] + 9);
	ASSERT_EQ(gold_manager->GetBalance(PlayerId::PLAYER2),
	          this->player_gold[1] + 18);

	// Checking if the extra 1 gold coin is removed
	ASSERT_EQ(gold_mine->GetGoldReserve(), 0);
}
