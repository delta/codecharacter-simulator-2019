#include "logger/error_type.h"
#include "logger/mocks/logger_mock.h"
#include "state/command_giver.h"
#include "state/gold_manager/gold_manager.h"
#include "state/map/map.h"
#include "state/mocks/command_taker_mock.h"
#include "state/mocks/state_syncer_mock.h"
#include "state/player_state.h"
#include "state/utilities.h"
#include "gtest/gtest.h"

using namespace std;
using namespace state;
using namespace testing;
using namespace physics;
using namespace logger;

Vec2D FlipPosition(Vec2D position, int map_size, int ele_size) {
	Vec2D new_position;
	new_position.x = map_size * ele_size - 1 - position.x;
	new_position.y = map_size * ele_size - 1 - position.y;
	return new_position;
}

// Helper functions to create state objects
Soldier *CreateStateSoldier(int64_t actor_id, int hp, GoldManager *gold_manager,
                            ScoreManager *score_manager,
                            PathPlanner *path_planner, DoubleVec2D position) {
	vector<Soldier *> soldiers;
	auto soldier = new Soldier(actor_id, PlayerId::PLAYER1, ActorType::SOLDIER,
	                           hp, 100, position, gold_manager, score_manager,
	                           path_planner, 5, 5, 40);

	return soldier;
}

Villager *CreateStateVillager(int64_t actor_id, int hp,
                              GoldManager *gold_manager,
                              ScoreManager *score_manager,
                              PathPlanner *path_planner, DoubleVec2D position) {
	vector<Villager *> villagers;
	auto villager = new Villager(
	    actor_id, PlayerId::PLAYER1, ActorType::VILLAGER, hp, 100, position,
	    gold_manager, score_manager, path_planner, 2, 5, 20, 5, 5, 5);

	return villager;
}

Factory *CreateStateFactory(int64_t actor_id, int hp, GoldManager *gold_manager,
                            ScoreManager *score_manager,
                            PathPlanner *path_planner, DoubleVec2D position,
                            ActorType actor_type) {
	vector<Factory *> factories;
	ActorType actor;
	auto factory =
	    new Factory(actor_id, PlayerId::PLAYER1, ActorType::FACTORY, hp, 500,
	                position, gold_manager, score_manager, 100, 100, actor_type,
	                5, 5, UnitProductionCallback{});

	return factory;
}

class CommandGiverTest : public Test {
  protected:
	// Creating command giver and command taker
	unique_ptr<CommandTakerMock> command_taker;
	unique_ptr<CommandGiver> command_giver;
	unique_ptr<GoldManager> gold_manager;
	unique_ptr<ScoreManager> score_manager;
	unique_ptr<PathPlanner> path_planner;
	unique_ptr<LoggerMock> logger;
	unique_ptr<Map> map;

	int64_t ele_size;
	int64_t map_size;
	std::vector<std::vector<TerrainType>> grid;

	// Creating player states
	array<player_state::State, 2> player_states;
	array<int64_t, 2> player_gold;

	// Helper function to manage state expectations
	void ManageActorExpectations(array<vector<Soldier *>, 2> soldiers,
	                             array<vector<Villager *>, 2> villagers,
	                             array<vector<Factory *>, 2> factories,
	                             array<player_state::State, 2> &player_states,
	                             ActorType actor_type) {
		EXPECT_CALL(*command_taker, GetSoldiers).WillOnce(Return(soldiers));
		EXPECT_CALL(*command_taker, GetVillagers).WillOnce(Return(villagers));
		EXPECT_CALL(*command_taker, GetFactories).WillOnce(Return(factories));
		command_giver->RunCommands(player_states, {false, false});

		if (actor_type == ActorType::SOLDIER) {
			// Resetting the player state soldier
			player_states[0].soldiers[0] = player_state::Soldier{};
			player_states[0].soldiers[0].id = 0;
		} else if (actor_type == ActorType::VILLAGER) {
			// Resetting the villager's state
			player_states[0].villagers[0] = player_state::Villager{};
			player_states[0].villagers[0].id = 2;
		}
	}

  public:
	CommandGiverTest() {
		// Creating the map
		this->ele_size = 5;
		this->map_size = 5;

		// Helper map constants
		const auto L = player_state::TerrainType::LAND;
		const auto W = player_state::TerrainType::WATER;
		const auto G = player_state::TerrainType::GOLD_MINE;

		/**
		 * Initialize player and state map
		 * G - Gold Mine
		 * W - Water
		 * L - Land
		 * Map Diagram: Player 1 View
		 *
		 * W L L L L
		 * L W L L L
		 * L L W L G
		 * L L G W L
		 * L L L L W
		 */
		this->command_taker = make_unique<CommandTakerMock>();
		vector<vector<TerrainType>> dummy_map;

		for (int64_t i = 0; i < this->map_size; ++i) {
			vector<TerrainType> map_row;
			for (int64_t j = 0; j < this->map_size; ++j) {

				// Set map of player states
				if (i == j) {
					this->player_states[0].map[i][j] = W;
					this->player_states[1]
					    .map[this->map_size - 1 - i][this->map_size - 1 - j] =
					    W;
				} else {
					this->player_states[0].map[i][j] = L;
					this->player_states[1]
					    .map[this->map_size - 1 - i][this->map_size - 1 - j] =
					    L;
				}

				// Set map of main state
				if (i == j) {
					map_row.push_back(TerrainType::WATER);
				} else {
					map_row.push_back(TerrainType::LAND);
				}
			}
			dummy_map.push_back(map_row);
		}

		// Assinging the gold mine locations
		this->player_states[0].map[2][3] = G;
		this->player_states[0].map[4][2] = G;
		this->player_states[1].map[2][1] = G;
		this->player_states[1].map[0][2] = G;

		this->player_states[0].gold_mine_offsets.push_back(Vec2D(2, 3));
		this->player_states[0].gold_mine_offsets.push_back(Vec2D(4, 2));
		this->player_states[1].gold_mine_offsets.push_back(Vec2D(2, 1));
		this->player_states[1].gold_mine_offsets.push_back(Vec2D(0, 2));
		dummy_map[4][2] = TerrainType::GOLD_MINE;
		dummy_map[2][3] = TerrainType::GOLD_MINE;

		this->map = make_unique<Map>(dummy_map, this->map_size, this->ele_size);
		this->path_planner = make_unique<PathPlanner>(this->map.get());
		this->gold_manager = make_unique<GoldManager>();
		this->score_manager = make_unique<ScoreManager>();
		this->logger = make_unique<LoggerMock>();
		this->command_giver = make_unique<CommandGiver>(
		    this->command_taker.get(), this->logger.get());

		// Creating the initial player state
		int64_t actor_id_count = 0;

		// Init Soldiers
		for (int i = 0; i < 2; ++i) {
			// Assigning id to all the soliders
			player_state::Soldier new_soldier;
			new_soldier.id = actor_id_count;
			actor_id_count++;

			new_soldier.position = Vec2D(this->ele_size, this->ele_size);

			// Adding new soldier to the vector new_soldiers
			this->player_states[i % 2].soldiers.push_back(new_soldier);

			// Flipping the position
			new_soldier.position =
			    Vec2D((this->map_size - 1) * this->ele_size - 1,
			          (this->map_size - 1) * this->ele_size - 1);
			this->player_states[(1 + i) % 2].enemy_soldiers.push_back(
			    new_soldier);
		}

		// Init Villagers
		for (int i = 0; i < 2; ++i) {
			// Reassigning the villager's basic attribites
			player_state::Villager new_villager;
			new_villager.id = actor_id_count;
			actor_id_count++;

			// For player1, positions are correct
			new_villager.position = Vec2D(this->ele_size, 0);

			// Pushing new_villager into new_villagers
			this->player_states[i % 2].villagers.push_back(new_villager);

			// Flipping the position
			new_villager.position =
			    Vec2D((this->map_size - 1) * this->ele_size - 1,
			          (this->map_size - 1) * this->ele_size - 1);
			this->player_states[(1 + i) % 2].enemy_villagers.push_back(
			    new_villager);
		}

		this->player_states[0].gold = 500;
		this->player_states[1].gold = 500;
		this->player_gold = {500, 500};
	}
};

TEST_F(CommandGiverTest, CommandExecutionTest) {
	// NOTE!
	// SetFactoryProduction and StopOrStartFactory will ALWAYS be called for
	// every factory for every turn. If state contains any factories, you MUST
	// set expectations for those methods.

	/// ----- CREATE TEMPLATE OBJECTS (Used for setting ) -----

	// Make soldiers
	auto state_soldier1 = CreateStateSoldier(
	    0, 100, this->gold_manager.get(), this->score_manager.get(),
	    this->path_planner.get(), DoubleVec2D(this->ele_size, this->ele_size));
	auto state_soldier2 = CreateStateSoldier(
	    1, 100, this->gold_manager.get(), this->score_manager.get(),
	    this->path_planner.get(), DoubleVec2D(this->ele_size, this->ele_size));
	array<vector<Soldier *>, 2> state_soldiers = {
	    {{state_soldier1}, {state_soldier2}}};

	// Make villagers
	auto state_villager1 = CreateStateVillager(
	    2, 100, this->gold_manager.get(), this->score_manager.get(),
	    this->path_planner.get(), DoubleVec2D(this->ele_size, this->ele_size));
	auto state_villager2 = CreateStateVillager(
	    3, 100, this->gold_manager.get(), this->score_manager.get(),
	    this->path_planner.get(), DoubleVec2D(this->ele_size, this->ele_size));
	array<vector<Villager *>, 2> state_villagers = {
	    {{state_villager1}, {state_villager2}}};

	// Make soldiers
	// Used ONLY during Factory tests
	auto state_factory1 = CreateStateFactory(
	    4, 500, this->gold_manager.get(), this->score_manager.get(),
	    this->path_planner.get(), DoubleVec2D(this->ele_size, this->ele_size),
	    ActorType::VILLAGER);
	auto state_factory2 = CreateStateFactory(
	    5, 100, this->gold_manager.get(), this->score_manager.get(),
	    this->path_planner.get(), DoubleVec2D(this->ele_size, this->ele_size),
	    ActorType::SOLDIER);
	// NO Factories by default
	array<vector<Factory *>, 2> state_factories = {};

	/// ----- Set Expectations for Map and player gold -----
	EXPECT_CALL(*this->command_taker, GetMap())
	    .WillRepeatedly(Return(this->map.get()));
	EXPECT_CALL(*this->command_taker, GetGold)
	    .WillRepeatedly(Return(this->player_gold));

	// ----- SOLDIER TESTS -----

	// Making soldiers attack each other and move at the same time
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, ErrorType::NO_MULTIPLE_SOLDIER_TASKS, _));
	this->player_states[0].soldiers[0].target =
	    this->player_states[0].enemy_soldiers[0].id;
	this->player_states[0].soldiers[0].destination =
	    Vec2D(this->ele_size, this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::SOLDIER);

	// Soldier trying to attack soldiers after changing id
	EXPECT_CALL(*this->logger,
	            LogError(PlayerId::PLAYER1, ErrorType::NO_ALTER_ACTOR_ID, _));
	this->player_states[0].soldiers[0].id = 69;
	this->player_states[0].soldiers[0].target =
	    this->player_states[0].enemy_soldiers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::SOLDIER);

	// Soldier trying to attack villagers after changing id
	EXPECT_CALL(*this->logger,
	            LogError(PlayerId::PLAYER1, ErrorType::NO_ALTER_ACTOR_ID, _));
	this->player_states[0].soldiers[0].id = 69;
	this->player_states[0].soldiers[0].target =
	    this->player_states[0].enemy_villagers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::SOLDIER);

	// Soldier trying to attack factories after changing id
	EXPECT_CALL(*this->logger,
	            LogError(PlayerId::PLAYER1, ErrorType::NO_ALTER_ACTOR_ID, _));
	state_factories[1].push_back(state_factory2);
	this->player_states[0].soldiers[0].id = 69;
	this->player_states[0].soldiers[0].target = state_factory2->GetActorId();
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::SOLDIER);
	state_factories[1].clear();

	// Soldier trying to attack own soldier
	EXPECT_CALL(*this->logger, LogError(PlayerId::PLAYER1,
	                                    ErrorType::NO_ATTACK_SELF_SOLDIER, _));
	EXPECT_CALL(*this->command_taker, FindActorById)
	    .WillOnce(Return(nullptr))
	    .WillOnce(Return(state_soldier1));
	this->player_states[0].soldiers[0].target =
	    this->player_states[0].soldiers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::SOLDIER);

	// Soldier trying to attack own villager
	EXPECT_CALL(*this->logger, LogError(PlayerId::PLAYER1,
	                                    ErrorType::NO_ATTACK_SELF_VILLAGER, _));
	EXPECT_CALL(*this->command_taker, FindActorById)
	    .WillOnce(Return(nullptr))
	    .WillOnce(Return(state_villager1));
	this->player_states[0].soldiers[0].target =
	    this->player_states[0].villagers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::SOLDIER);

	// Soldier trying to attack own factory
	EXPECT_CALL(*this->logger, LogError(PlayerId::PLAYER1,
	                                    ErrorType::NO_ATTACK_SELF_FACTORY, _));
	state_factories[0].push_back(state_factory1);
	EXPECT_CALL(*this->command_taker, FindActorById)
	    .WillOnce(Return(nullptr))
	    .WillOnce(Return(state_factory1));
	this->player_states[0].soldiers[0].target = state_factory1->GetActorId();
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::SOLDIER);
	state_factories[0].clear();

	// Soldier trying to move out of the map
	EXPECT_CALL(*this->logger, LogError(PlayerId::PLAYER1,
	                                    ErrorType::INVALID_MOVE_POSITION, _));
	this->player_states[0].soldiers[0].destination =
	    Vec2D(this->map_size * this->ele_size, this->map_size * this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::SOLDIER);

	/// ----- VILLAGER TESTS -----

	// Villager trying to attack soldiers after changing id
	EXPECT_CALL(*this->logger,
	            LogError(PlayerId::PLAYER1, ErrorType::NO_ALTER_ACTOR_ID, _));
	this->player_states[0].villagers[0].id =
	    this->player_states[0].villagers[0].id + 1;
	this->player_states[0].villagers[0].target =
	    this->player_states[0].enemy_villagers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	// Villager trying to attack villagers after changing id
	EXPECT_CALL(*this->logger,
	            LogError(PlayerId::PLAYER1, ErrorType::NO_ALTER_ACTOR_ID, _));
	this->player_states[0].villagers[0].id =
	    this->player_states[0].villagers[0].id + 1;
	this->player_states[0].villagers[0].target =
	    this->player_states[0].enemy_villagers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	// Villager trying to attack factories after changing id
	state_factories[1].push_back(state_factory2);
	EXPECT_CALL(*this->logger,
	            LogError(PlayerId::PLAYER1, ErrorType::NO_ALTER_ACTOR_ID, _));
	this->player_states[0].villagers[0].id = 69;
	this->player_states[0].villagers[0].target = state_factory2->GetActorId();
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);
	state_factories[1].clear();

	// Villager trying to attack and move
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, ErrorType::NO_MULTIPLE_VILLAGER_TASKS, _));
	this->player_states[0].villagers[0].target =
	    this->player_states[0].enemy_villagers[0].id;
	this->player_states[0].villagers[0].destination =
	    Vec2D(this->ele_size, this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	// Villager trying to attack and mine gold
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, ErrorType::NO_MULTIPLE_VILLAGER_TASKS, _));
	this->player_states[0].villagers[0].target =
	    this->player_states[0].enemy_soldiers[0].id;
	this->player_states[0].villagers[0].mine_target =
	    Vec2D(this->ele_size, this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	// Villager trying to attack and create factory at the same time
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, ErrorType::NO_MULTIPLE_VILLAGER_TASKS, _));
	this->player_states[0].villagers[0].target =
	    this->player_states[0].enemy_soldiers[0].id;
	this->player_states[0].villagers[0].build_offset =
	    Vec2D(this->ele_size, this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	// Villager trying to attack and build factory
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, ErrorType::NO_MULTIPLE_VILLAGER_TASKS, _));
	state_factories[0].push_back(state_factory1);
	this->player_states[0].villagers[0].target =
	    this->player_states[0].enemy_soldiers[0].id;
	this->player_states[0].villagers[0].target_factory_id =
	    state_factory1->GetActorId();
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);
	state_factories[0].clear();

	// Villager trying to move and mine gold
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, ErrorType::NO_MULTIPLE_VILLAGER_TASKS, _));
	this->player_states[0].villagers[0].destination =
	    Vec2D(this->ele_size, this->ele_size);
	this->player_states[0].villagers[0].mine_target =
	    Vec2D(this->ele_size, this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	// Villager trying to move and create factory
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, ErrorType::NO_MULTIPLE_VILLAGER_TASKS, _));
	this->player_states[0].villagers[0].destination =
	    Vec2D(this->ele_size, this->ele_size);
	this->player_states[0].villagers[0].build_offset =
	    Vec2D(this->ele_size, this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	// Villager trying to move and build factory
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, ErrorType::NO_MULTIPLE_VILLAGER_TASKS, _));
	state_factories[0].push_back(state_factory1);
	this->player_states[0].villagers[0].destination =
	    Vec2D(this->ele_size, this->ele_size);
	this->player_states[0].villagers[0].target_factory_id =
	    state_factory1->GetActorId();
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);
	state_factories[0].clear();

	// Villager trying to mine target and create factory
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, ErrorType::NO_MULTIPLE_VILLAGER_TASKS, _));
	this->player_states[0].villagers[0].mine_target =
	    Vec2D(this->ele_size, this->ele_size);
	this->player_states[0].villagers[0].build_offset =
	    Vec2D(this->ele_size, this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	// Villager trying to mine target and build factory
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, ErrorType::NO_MULTIPLE_VILLAGER_TASKS, _));
	state_factories[0].push_back(state_factory1);
	this->player_states[0].villagers[0].mine_target =
	    Vec2D(this->ele_size, this->ele_size);
	this->player_states[0].villagers[0].target_factory_id =
	    state_factory1->GetActorId();
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);
	state_factories[0].clear();

	// Villager trying to create factory and build factory
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, ErrorType::NO_MULTIPLE_VILLAGER_TASKS, _));
	state_factories[0].push_back(state_factory1);
	this->player_states[0].villagers[0].build_offset =
	    Vec2D(this->ele_size, this->ele_size);
	this->player_states[0].villagers[0].target_factory_id =
	    state_factory1->GetActorId();
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);
	state_factories[0].clear();

	// Making villager go out of the map
	EXPECT_CALL(*this->logger, LogError(PlayerId::PLAYER1,
	                                    ErrorType::INVALID_MOVE_POSITION, _));
	this->player_states[0].villagers[0].destination =
	    Vec2D(this->map_size * this->ele_size, this->map_size * this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	// Making villager create a factory outside map
	EXPECT_CALL(*this->logger, LogError(PlayerId::PLAYER1,
	                                    ErrorType::INVALID_BUILD_POSITION, _));
	this->player_states[0].villagers[0].build_offset =
	    Vec2D(this->map_size * this->ele_size, this->map_size * this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	// Making villager create a factory on water
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, ErrorType::NO_BUILD_FACTORY_ON_WATER, _));
	// (0, 0) is a water tile
	this->player_states[0].villagers[0].build_offset = Vec2D(0, 0);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	// Villager trying to create a factory with insufficient funds
	auto player_gold2 = this->player_gold;
	player_gold2[0] = 0;
	EXPECT_CALL(*this->command_taker, GetGold)
	    .WillRepeatedly(Return(player_gold2));

	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, logger::ErrorType::INSUFFICIENT_FUNDS, _));
	this->player_states[0].villagers[0].build_offset = Vec2D(3, 0);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	EXPECT_CALL(*this->command_taker, GetGold)
	    .WillRepeatedly(Return(this->player_gold)); // Reset player gold

	// Making villagers try and mine outside map
	EXPECT_CALL(*this->logger, LogError(PlayerId::PLAYER1,
	                                    ErrorType::INVALID_MINE_POSITION, _));
	this->player_states[0].villagers[0].mine_target =
	    Vec2D(this->map_size * this->ele_size, this->map_size * this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	// Making villagers mine land
	EXPECT_CALL(*this->logger, LogError(PlayerId::PLAYER1,
	                                    ErrorType::INVALID_MINE_POSITION, _));
	this->player_states[0].villagers[0].mine_target = Vec2D(1, 0);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	// Making villager mine water
	EXPECT_CALL(*this->logger, LogError(PlayerId::PLAYER1,
	                                    ErrorType::INVALID_MINE_POSITION, _));
	this->player_states[0].villagers[0].mine_target = Vec2D(1, 1);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	// Making villager build factory that dosen't exist
	EXPECT_CALL(*this->logger,
	            LogError(PlayerId::PLAYER1,
	                     logger::ErrorType::NO_BUILD_FACTORY_THAT_DOSENT_EXIST,
	                     _));
	this->player_states[0].villagers[0].target_factory_id = 69;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	/// ----- FACTORY TESTS -----

	// Making a factory change it's actor id
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, logger::ErrorType::NO_ALTER_ACTOR_ID, _));
	state_factories[0].push_back(state_factory1);
	this->player_states[0].factories.push_back(player_state::Factory{});
	this->player_states[0].factories[0].id = 69; // Changed id

	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::FACTORY);
	this->player_states[0].factories.clear();

	// Creating 50 factories to trigger maximum limit on number of factories
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, logger::ErrorType::NO_MORE_FACTORIES, _));
	vector<Factory *> state_max_factories;
	for (int i = 0; i < MAX_NUM_FACTORIES; ++i) {
		auto new_factory = CreateStateFactory(
		    4, 500, this->gold_manager.get(), this->score_manager.get(),
		    this->path_planner.get(),
		    DoubleVec2D(this->ele_size, this->ele_size), ActorType::FACTORY);
		state_max_factories.push_back(new_factory);
	}
	state_factories[0] = state_max_factories;
	this->player_states[0].villagers[0].build_offset = Vec2D(3, 0);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);
	state_factories[0].clear();

	/// ----- VALID OPERATIONS TEST -----

	/// SOLDIERS' ATTACK
	// Soldier attacking soldier
	EXPECT_CALL(*this->command_taker, FindActorById)
	    .WillOnce(Return(state_soldier2));
	EXPECT_CALL(*this->command_taker,
	            AttackActor(PlayerId::PLAYER1,
	                        this->player_states[0].soldiers[0].id,
	                        this->player_states[0].enemy_soldiers[0].id));
	this->player_states[0].soldiers[0].target =
	    this->player_states[0].enemy_soldiers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::SOLDIER);

	// Soldier attacking villager
	EXPECT_CALL(*this->command_taker, FindActorById)
	    .WillOnce(Return(state_villager2));
	EXPECT_CALL(*this->command_taker,
	            AttackActor(PlayerId::PLAYER1,
	                        this->player_states[0].soldiers[0].id,
	                        this->player_states[0].enemy_villagers[0].id));
	this->player_states[0].soldiers[0].target =
	    this->player_states[0].enemy_villagers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::SOLDIER);

	// Soldier attacking factory
	state_factories[1].push_back(state_factory2);
	EXPECT_CALL(*this->command_taker,
	            AttackActor(PlayerId::PLAYER1,
	                        this->player_states[0].soldiers[0].id,
	                        state_factory2->GetActorId()));
	EXPECT_CALL(*this->command_taker, FindActorById)
	    .WillOnce(Return(state_factory2));
	this->player_states[0].soldiers[0].target = state_factory2->GetActorId();
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::SOLDIER);
	state_factories[1].clear();

	/// VILLAGERS' ATTACK
	// Villagers attacking soldiers
	EXPECT_CALL(*this->command_taker, FindActorById)
	    .WillOnce(Return(state_soldier1));
	EXPECT_CALL(*this->command_taker,
	            AttackActor(PlayerId::PLAYER1,
	                        this->player_states[0].villagers[0].id,
	                        this->player_states[0].enemy_soldiers[0].id));
	this->player_states[0].villagers[0].target =
	    this->player_states[0].enemy_soldiers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	// Villagers attacking villagers
	EXPECT_CALL(*this->command_taker, FindActorById)
	    .WillOnce(Return(state_villager2));
	EXPECT_CALL(*this->command_taker,
	            AttackActor(PlayerId::PLAYER1,
	                        this->player_states[0].villagers[0].id,
	                        this->player_states[0].enemy_villagers[0].id));
	this->player_states[0].villagers[0].target =
	    this->player_states[0].enemy_villagers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	// Villager attacking factories
	state_factories[1].push_back(state_factory2);
	EXPECT_CALL(*this->command_taker, FindActorById)
	    .WillOnce(Return(state_factory2));
	EXPECT_CALL(*this->command_taker,
	            AttackActor(PlayerId::PLAYER1,
	                        this->player_states[0].villagers[0].id,
	                        state_factory2->GetActorId()));
	this->player_states[0].villagers[0].target = state_factory2->GetActorId();
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);
	state_factories[1].clear();

	/// VILLAGERS' OPERATIONS
	// Villager create factory which produces soldiers
	EXPECT_CALL(*this->command_taker,
	            CreateFactory(PlayerId::PLAYER1,
	                          this->player_states[0].villagers[0].id,
	                          Vec2D(3, 0), ActorType::SOLDIER));
	this->player_states[0].villagers[0].build_offset = Vec2D(3, 0);
	this->player_states[0].villagers[0].build_factory_type =
	    player_state::FactoryProduction::SOLDIER;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	// Villager create factory which produces villagers
	EXPECT_CALL(*this->command_taker,
	            CreateFactory(PlayerId::PLAYER1,
	                          this->player_states[0].villagers[0].id,
	                          Vec2D(3, 0), ActorType::VILLAGER));
	this->player_states[0].villagers[0].build_offset = Vec2D(3, 0);
	this->player_states[0].villagers[0].build_factory_type =
	    player_state::FactoryProduction::VILLAGER;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	// Villager build factory
	state_factories[0].push_back(state_factory1);
	EXPECT_CALL(*this->command_taker,
	            BuildFactory(PlayerId::PLAYER1,
	                         this->player_states[0].villagers[0].id,
	                         state_factory1->GetActorId()));
	this->player_states[0].villagers[0].target_factory_id =
	    state_factory1->GetActorId();
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);
	state_factories[0].clear();

	// Villager mine gold
	auto mine_offset = Vec2D(4, 2);
	auto mine_position = Vec2D{mine_offset.x * ele_size + (ele_size / 2),
	                           mine_offset.y * ele_size + (ele_size / 2)};
	EXPECT_CALL(*this->command_taker,
	            MineLocation(PlayerId::PLAYER1,
	                         this->player_states[0].villagers[0].id,
	                         mine_position));
	this->player_states[0].villagers[0].mine_target = Vec2D(4, 2);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	// Villager move
	EXPECT_CALL(*this->command_taker,
	            MoveUnit(PlayerId::PLAYER1,
	                     this->player_states[0].villagers[0].id,
	                     Vec2D(this->ele_size, this->ele_size)));
	this->player_states[0].villagers[0].destination =
	    Vec2D(this->ele_size, this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->player_states, ActorType::VILLAGER);

	/// ----- END -----
}
