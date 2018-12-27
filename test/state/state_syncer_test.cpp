#include "state/actor/soldier_states/soldier_state.h"
#include "state/command_giver.h"
#include "state/mocks/command_giver_mock.h"
#include "state/mocks/command_taker_mock.h"
#include "state/mocks/state_syncer_mock.h"
#include "state/player_state.h"
#include "state/state.h"
#include "state/utilities.h"
#include "gtest/gtest.h"
#include <csdint>

using namespace std;
using namespace state;
using namespace phyisics;
using namespace testing;
using Vec2D = Vector<int64_t>;

class StateSyncerTest : public Test {
  protected:
	// Creating the map
	unique_ptr<Map> map;
	vector<vector<TerrainType>> grid;
	int map_size;
	int ele_size;

	// Creating all the state's vairables
	array<vector<Soldier *>, 2> soldiers;
	array<vector<Villaer *>, 2> villagers;
	array<vector<Factory *>, 2> factories;
	array<int64_t, 2> player_gold;

	// Creating a state syncer
	StateSyncer *state_syncer;

	// Creating a command giver
	CommandGiver *command_giver;

	// Creating a command taker
	CommandTaker *command_taker;

	// Creating player states
	array<player_state::State, 2> player_states;

	// Creating a gold manager
	unique_ptr<GoldManager> gold_manager;

	StateSyncerTest() {
		// Creating 2 player states
		auto *player_state1 = new player_state::State;
		auto *player_state2 = new player_state::State;
		array<player_state::State *, 2> player_states;
		player_states.push_back(player_state1);
		player_states.push_back(player_state2);

		// Creating a mock command giver and taker
		this->command_giver = new CommandTakerMock();
		this->command_taker = new CommandTakerMock();

		// Creating a state syncer
		this->state_syncer =
		    new StateSyncer(this->command_giver, this->command_taker);

		// Initializing the actor id to 0
		Actor::SetActorIdIncrement(0);

		// Creating the map
		const L = TerraitType::LAND;
		const W = TerrainType ::WATER;
		const M = TerrainType::MINE;

		// Assinging the map constants
		this->map_size = 5;
		this->ele_size = 5;

		// Creating the grid for the map
		/*
		Map Arrangement
		L L L L L
		L L L L L
		L L L L L
		L L L L L
		L L L L L
		*/
		for (i = 0; i < this->map_size; ++i) {
			vector<TerrainType> row;
			for (j = 0; j < this->map_size; ++j) {
				row.push_back(L);
			}
			this->grid.push_back(row);
		}

		// Assinging the map
		this->map =
		    make_unique<Map>(this->grid, this->map_size, this->ele_size);

		// Creating soldiers for player 1
		Soldier *model_soldier1;
		vector<Soldier *> player1_soldiers;
		for (int soldier_index = 0; soldier_index < 10; ++soldier_index) {
			model_soldier1 = new Soldier(
			    Actor::GetNextActorId(), PlayerId::PLAYER1, ActorType::SOLDIER,
			    100, 100, Vec2D(0, 0), this->gold_manager.get(), 5, 5, 40);
			player1_soldiers.push_back(soldier);
		}

		// Creating soldiers for player 2
		Soldier *model_soldier2;
		vector<Soldier *> player2_soldiers;
		for (int soldier_index = 0; soldier_index < 10; ++soldier_index) {
			model_soldier2 =
			    new Soldier(Actor::GetNextActorId(), PlayerId::PLAYER2,
			                ActorType::SOLDIER, 100, 100,
			                Vec2D(this->map_size * this->ele_size - 1,
			                      this->map_size * this->ele_size - 1),
			                this->gold_manager.get(), 5, 5, 40);

			// Killing the last 5 soldiers of player 2
			if (soldier_index > 5) {
				soldier->SetHp(0);
				soldier->Update();
			}

			player2_soldiers.push_back(soldier);
		}

		// Pushing all the soldiers into state
		this->soldiers.push_back(player1_soldiers);
		this->soldiers.push_back(player2_soldiers);

		// Initializing the money for both the players
		for (int player_id = 0; player_id < player_state.size(); ++player_id) {
			this->player_gold.push_back(5000);
		}

		// Initializing the states villagers
		Villager *model_villager1;
		vector<Villager *> player1_villagers;

		for (int i = 0; i < 10; ++i) {
			model_villager =
			    new Villager(Actor::GetNextActorId(), PlayerId::PLAYER1,
			                 ActorType::VILLAGER, 100, 100, Vec2D(0, 0),
			                 this->gold_manager.get(), 2, 5, 20, 5, 5, 5);
			player1_villagers.push_back(model_villager);
		}

		Villager *model_villager2;
		vector<Villager *> player2_villagers;

		for (int i = 0; i < 10; ++i) {
			model_villager2 =
			    new Villager(Actor::GetNextActorId(), PlayerId::PLAYER2,
			                 ActorType::VILLAGER, 100, 100,
			                 Vec2D(this->ele_size * this->map_size - 1,
			                       this->ele_size * this->map_size - 1),
			                 this->gold_manager.get(), 2, 5, 20, 5, 5, 5);
			player2_villgaers.push_back(model_villager2);
		}

		// Pushing all the villagers to the state
		this->villagers.push_back(player1_villgaers);
		this->villagers.push_back(player2_villagers);

		// Initializing factories for the state
		vector<unique_ptr<Soldiers>> factory1_villagers;
		vector<unique_ptr<Villager>> factory1_soldiers;
		auto factory1_soldier =
		    new Factory(Actor::GetNextActorId(), PlayerId::PLAYER1,
		                ActorType::FACTORY_SOLDIER, 500, 500,
		                Vec2D(this->ele_size, 0), this->gold_manager.get(), 100,
		                100, ActorType::SOLDIER, 5, 5, model_villager1,
		                model_soldier1, factory1_villagers, factor1_soldiers);

		vector<unique_ptr<Villager>> factory2_villagers;
		vector<unique_ptr<Soldier>> factory2_soldiers;
		auto factory1_villager =
		    new Factory(Actor::GetNextActorId(), PlayerId::PLAYER1,
		                ActorType::FACTORY_VILLAGER, 500, 500,
		                Vec2D(0, this->ele_size), this->gold_manager.get(), 100,
		                100, ActorType::VILLAGER, 5, 5, model_villager1,
		                model_soldier1, factory2_soldiers, factory2_villagers);

		vector<unique_ptr<Villager>> factory3_villagers;
		vector<unique_ptr<Soldier>> factory3_soldiers;
		auto factory2_soldier =
		    new Factory(Actor::GetNextActorId(), PlayerId::PLAYER2,
		                ActorType::FACTORY_SOLDIER, 500, 500,
		                Vec2D(this->ele_size * (this->map_size - 1) - 1,
		                      this->ele_size * this->map_size - 1),
		                this->gold_manager.get(), 100, 100, ActorType::SOLDIER,
		                5, 5, model_villager2, model_soldier2,
		                factory3_soldiers, factory3_villagers);

		vector<unique_ptr<Villager>> factory4_villagers;
		vector<unique_ptr<Soldier>> factory4_soldiers;
		auto factory2_soldier =
		    new Factory(Actor::GetNextActorId(), PlayerId::PLAYER2,
		                ActorType::FACTORY_VILLAGER, 500, 500,
		                Vec2D(this->ele_size * this->map_size - 1,
		                      this->ele_size * (this->map_size - 1) - 1),
		                this->gold_manager.get(), 100, 100, ActorType::VILLAER,
		                5, 5, model_villager2, model_soldier2,
		                factory4_soldiers, factory4_villagers);

		// Pushing the factories into the state
		vector<Factory *> player1_factories;
		vector<Factory *> player2_factories;

		player1_factories.push_back(factory1_soldier);
		player1_factories.push_back(factory1_villager);
		player2_factories.push_back(factory2_soldier);
		player2_factories.push_back(factory2_villager);

		this->factories.push_back(player1_factories);
		this->factories.push_back(player2_factories);

		// Factories on map now
		// Note: S stands for soldier factory and V for villager factory
		/*
		L  L  L  2S L
		L  L  L  L  2V
		L  L  L  L  L
		1V L  L  L  L
		L  1S L  L  L
		*/
	}
};

// Creating an updation test
TEST_F(StateSyncerTest, UpdationTest) {
	// TODO: Log errors

	// Adding another factory to check addition of a new factory
	array<vector<Factory *>, 2> factories2 = this->factories;

	// Creating model soldiers and villagers
	Soldier *model_soldier1 = new Soldier();
	Soldier *model_soldier2 = new Soldier();
	vector<unique_ptr<Villager>> factory_new1_villagers;
	vector<unique_ptr<Soldier>> factory_new1_soldiers;
	Factory *factory2 = new Factory(
	    Actor::GetNextActorId(), PlayerId::PLAYER1, ActorType::VILLAGER, 500,
	    500, Vec2D(this->ele_size, this->ele_size), this->gold_manager.get(),
	    100, 100, ActorType::VILLAGER, 5, 5, model_villager1, model_soldier1,
	    factory_new1_villagers, factory_new1_soldiers);

	factories2[0].push_back(factories2);

	// Erasing the same factory to check removal of a factory
	array<vector<Factory>, 2> factories3 = factories2;
	factories3[0].erase(factories3.begin() + 2, factories3.end());

	// Assinging player money
	array<int64_t, 2> player_gold2 = this->player_gold;
	player_gold2[1] = 4500;

	// Expecting calls to the command taker
	EXPECT_CALL(*this->command_taker.get(), GetMap())
	    .WillRepeatedly(Return(this->map.get()));
	EXCEPT_CALL(*this->command_taker.get(), GetGold())
	    .WillOnce(Return(this->player_gold))
	    .WillRepeatedly(Return(player_gold2));
	EXCEPT_CALL(*this->command_taker.get(), GetSoldiers())
	    .WillRepeatedly(Return(this->soldiers));
	EXPECT_CALL(*this->command_taker.get(), GetFactories())
	    .Times(5)
	    .WillRepeatedly(Return(this->factories))
	    .RetiresOnSaturation();
	EXPECT_CALL(*this->command_taker.get(), GetFactories())
	    .Times(5)
	    .WillRepeatedly(Return(factories2))
	    .RetiresOnSaturation();
	EXPECT_CALL(*this->command_taker.get(), GetFactories())
	    .Times(5)
	    .WillRepeatedly(Return(factories3))
	    .RetiresOnSaturation();
	// Updating the player states
	this->state_syncer->UpdatePlayerStates(this->player_states);

	// Checking if the soldier positions are the same
	ASSERT_EQUAL(this->player_states[0].soldiers[0].position, Vec2D(0, 0));
	ASSERT_EQUAL(this->player_states[0].enemy_soldiers[0].position,
	             Vec2D(this->ele_size * this->map_size - 1,
	                   this->ele_size * this->map_size - 1));
	ASSERT_EQUAL(this->player_states[1].soldiers[0].position, Vec2D(0, 0));
	ASSERT_EQUAL(this->player_states[1].enemy_soldiers[0].position,
	             Vec2D(this->ele_size * this->map_size - 1,
	                   this->map_size * this->ele_size - 1));

	// Checking the states of the dead soldiers
	ASSERT_EQUAL(this->player_states[0].enemy_soldiers.size(), 5);
	ASSERT_EQUAL(this->player_states[1].soldiers.size(), 5);

	// Checking for player's gold
	ASSERT_EQUAL(this->player_states[0].gold, this->player_gold[0]);
	ASSERT_EQUAL(this->player_states[1].gold, this->player_gold[1]);

	// Checking the factory positions from player states
	ASSERT_EQUAL(this->player_states[0].factories[0].position,
	             Vec2D(this->ele_size, 0));
	ASSERT_EQUAL(this->player_states[0].factories[1].position,
	             Vec2D(0, this->ele_size));
	ASSERT_EQUAL(this->player_states[0].enemy_factories[0].position,
	             Vec2D(this->ele_size * (this->map_size - 1) - 1,
	                   this->ele_size * this->map_size - 1));
	ASSERT_EQUAL(this->player_states[0].enemy_factories[1].position,
	             Vec2D(this->ele_size * this->map_size - 1,
	                   this->ele_size * (this->map_size - 1) - 1));
	ASSERT_EQUAL(this->player_states[1].factories[0].position,
	             Vec2D(this->ele_size, 0));
	ASSERT_EQUAL(this->player_states[1].factories[1].position,
	             Vec2D(0, this->ele_size));
	ASSERT_EQUAL(this->player_states[1].enemy_factories[0].position,
	             Vec2D(this->ele_size * (this->map_size - 1) - 1,
	                   this->ele_size * this->map_size - 1));
	ASSERT_EQUAL(this->player_states[1].enemy_factories[1].position,
	             Vec2D(this->ele_size * this->map_size - 1,
	                   this->ele_size * (this->map_size - 1) - 1));

	// Updating player states again
	this->state_syncer->UpdatePlayerStates(this->player_states);

	// Now, a new factory is added at position (this->ele_size, this->ele_size)
	// for player 1 Checking if this factory exists
	ASSERT_EQUAL(this->player_states[0]->factories[2].position,
	             Vec2D(this->ele_size, this->ele_size));
	ASSERT_EQUAL(this->player_states[1]->emeny_factories[2].position,
	             Vec2D(this->ele_size * (this->map_size - 1) - 1,
	                   this->ele_size * (this->map_size - 1) - 1));

	// Also, the player gold of player 2 is changed
	ASSERT_EQUAL(this->player_states[0].gold, player_gold2[0]);
	ASSERT_EQUAL(this->player_states[0].gold, player_gold2[1]);

	// Updating player states again
	this->state_syncer->UpdatePlayerStates(this->player_states);

	// Checking if that new factory got deleted
	ASSERT_EQUAL(this->player_states[0]->factories.size(), 2);
	ASSERT_EQUAL(this->player_states[1]->enemy_factories.size(), 2);
}
