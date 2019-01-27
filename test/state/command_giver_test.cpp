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
vector<Soldier *> CreateStateSoldier(int64_t actor_id, int hp,
                                     GoldManager *gold_manager,
                                     PathPlanner *path_planner,
                                     DoubleVec2D position) {
	vector<Soldier *> soldiers;
	auto soldier =
	    new Soldier(actor_id, PlayerId::PLAYER1, ActorType::SOLDIER, hp, 100,
	                position, gold_manager, path_planner, 5, 5, 40);
	soldiers.push_back(soldier);
	return soldiers;
}

vector<Villager *> CreateStateVillager(int64_t actor_id, int hp,
                                       GoldManager *gold_manager,
                                       PathPlanner *path_planner,
                                       DoubleVec2D position) {
	vector<Villager *> villagers;
	auto villager =
	    new Villager(actor_id, PlayerId::PLAYER1, ActorType::VILLAGER, hp, 100,
	                 position, gold_manager, path_planner, 2, 5, 20, 5, 5, 5);
	villagers.push_back(villager);
	return villagers;
}

vector<Factory *> CreateStateFactory(int64_t actor_id, int hp,
                                     GoldManager *gold_manager,
                                     PathPlanner *path_planner,
                                     DoubleVec2D position,
                                     ActorType production_type) {
	vector<Factory *> factories;
	ActorType actor;
	if (production_type == ActorType::FACTORY_SOLDIER) {
		actor = ActorType::SOLDIER;
	} else {
		actor = ActorType::VILLAGER;
	}
	auto factory = new Factory(actor_id, PlayerId::PLAYER1, production_type, hp,
	                           500, position, gold_manager, 100, 100, actor, 5,
	                           5, UnitProductionCallback{});
	factories.push_back(factory);
	return factories;
}

// Helper function to manage state expectations
void ManageActorExpectations(array<vector<Soldier *>, 2> soldiers,
                             array<vector<Villager *>, 2> villagers,
                             array<vector<Factory *>, 2> factories,
                             CommandTakerMock *command_taker,
                             array<player_state::State, 2> &player_states,
                             CommandGiver *command_giver,
                             ActorType actor_type) {
	EXPECT_CALL(*command_taker, GetSoldiers).WillOnce(Return(soldiers));
	EXPECT_CALL(*command_taker, GetVillagers).WillOnce(Return(villagers));
	EXPECT_CALL(*command_taker, GetFactories).WillOnce(Return(factories));
	command_giver->RunCommands(player_states);

	if (actor_type == ActorType::SOLDIER) {
		// Resetting the player state soldier
		player_states[0].soldiers[0] = player_state::Soldier{};
		player_states[0].soldiers[0].id = 0;
	} else if (actor_type == ActorType::VILLAGER) {
		// Resetting the villager's state
		player_states[0].villagers[0] = player_state::Villager{};
		player_states[0].villagers[0].id = 2;

	} else {
		// Resetting the factory's state
		player_states[0].factories[0] = player_state::Factory{};
		player_states[0].factories[0].id = 4;
	}
}

class CommandGiverTest : public Test {
  protected:
	// Creating command giver and command taker
	unique_ptr<CommandTakerMock> command_taker;
	unique_ptr<CommandGiver> command_giver;
	unique_ptr<GoldManager> gold_manager;
	unique_ptr<PathPlanner> path_planner;
	unique_ptr<LoggerMock> logger;
	unique_ptr<Map> map;

	int64_t ele_size;
	int64_t map_size;
	std::vector<std::vector<TerrainType>> grid;

	// Creating player states
	array<player_state::State, 2> player_states;
	array<int64_t, 2> player_gold;

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

		this->player_states[0].gold_mine_locations.push_back(Vec2D(2, 3));
		this->player_states[0].gold_mine_locations.push_back(Vec2D(4, 2));
		this->player_states[1].gold_mine_locations.push_back(Vec2D(2, 1));
		this->player_states[1].gold_mine_locations.push_back(Vec2D(0, 2));
		dummy_map[4][2] = TerrainType::GOLD_MINE;
		dummy_map[2][3] = TerrainType::GOLD_MINE;

		this->map = make_unique<Map>(dummy_map, this->map_size, this->ele_size);
		this->path_planner = make_unique<PathPlanner>(this->map.get());
		this->gold_manager = make_unique<GoldManager>();
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

		// Init Factories
		for (int i = 0; i < 2; ++i) {
			// Reassinging basic attributes
			player_state::Factory new_factory;
			new_factory.id = actor_id_count;
			actor_id_count++;
			new_factory.hp = 500;

			this->player_states[i % 2].factories.push_back(new_factory);

			// Flipping the position
			new_factory.position = Vec2D(this->map_size * this->ele_size - 1,
			                             this->map_size * this->ele_size - 1);
			this->player_states[(1 + i) % 2].enemy_factories.push_back(
			    new_factory);
		}

		this->player_states[0].gold = 500;
		this->player_states[1].gold = 500;
		this->player_gold = {500, 500};
	}
};

TEST_F(CommandGiverTest, CommandExecutionTest) {
	// Expect errors from logger
	// Soldier errors

	EXPECT_CALL(*this->command_taker, GetMap())
	    .WillRepeatedly(Return(this->map.get()));
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, ErrorType::NO_MULTIPLE_SOLDIER_TASKS, _))
	    .Times(1);
	EXPECT_CALL(*this->logger,
	            LogError(PlayerId::PLAYER1, ErrorType::NO_ALTER_ACTOR_ID, _))
	    .Times(3);
	EXPECT_CALL(*this->logger, LogError(PlayerId::PLAYER1,
	                                    ErrorType::NO_ATTACK_SELF_SOLDIER, _))
	    .Times(1);
	EXPECT_CALL(*this->logger, LogError(PlayerId::PLAYER1,
	                                    ErrorType::NO_ATTACK_SELF_VILLAGER, _))
	    .Times(1);
	EXPECT_CALL(*this->logger, LogError(PlayerId::PLAYER1,
	                                    ErrorType::NO_ATTACK_SELF_FACTORY, _))
	    .Times(1);
	EXPECT_CALL(*this->logger, LogError(PlayerId::PLAYER1,
	                                    ErrorType::NO_ATTACK_DEAD_SOLDIER, _))
	    .Times(1);
	EXPECT_CALL(*this->logger, LogError(PlayerId::PLAYER1,
	                                    ErrorType::INVALID_MOVE_POSITION, _))
	    .Times(1);
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, ErrorType::NO_ACTION_BY_DEAD_SOLDIER, _))
	    .Times(2);
	EXPECT_CALL(*this->command_taker, GetGold)
	    .WillRepeatedly(Return(this->player_gold));

	// Factory calls will occur every turn, so suppress them here
	// TODO: Manually filter for factory updates in CommandGiver instead of
	// unconditionally updating their state at every turn
	EXPECT_CALL(*this->command_taker, SetFactoryProduction).Times(20);
	EXPECT_CALL(*this->command_taker, StopOrStartFactory).Times(20);

	// Creating state soldier
	auto state_soldier1 = CreateStateSoldier(
	    0, 100, this->gold_manager.get(), this->path_planner.get(),
	    DoubleVec2D(this->ele_size, this->ele_size));
	auto state_soldier2 = CreateStateSoldier(
	    1, 100, this->gold_manager.get(), this->path_planner.get(),
	    DoubleVec2D(this->ele_size, this->ele_size));
	array<vector<Soldier *>, 2> state_soldiers = {state_soldier1,
	                                              state_soldier2};

	// Creating dead soldiers
	auto dead_soldier1 = CreateStateSoldier(
	    0, 0, this->gold_manager.get(), this->path_planner.get(),
	    DoubleVec2D(this->ele_size, this->ele_size));
	auto dead_soldier2 = CreateStateSoldier(
	    1, 0, this->gold_manager.get(), this->path_planner.get(),
	    DoubleVec2D(this->ele_size, this->ele_size));

	// Creating state villagers
	auto state_villager1 = CreateStateVillager(
	    2, 100, this->gold_manager.get(), this->path_planner.get(),
	    DoubleVec2D(this->ele_size, this->ele_size));
	auto state_villager2 = CreateStateVillager(
	    3, 100, this->gold_manager.get(), this->path_planner.get(),
	    DoubleVec2D(this->ele_size, this->ele_size));
	array<vector<Villager *>, 2> state_villagers = {state_villager1,
	                                                state_villager2};

	// Creating dead villagers
	auto dead_villager1 = CreateStateVillager(
	    2, 0, this->gold_manager.get(), this->path_planner.get(),
	    DoubleVec2D(this->ele_size, this->ele_size));
	auto dead_villager2 = CreateStateVillager(
	    3, 0, this->gold_manager.get(), this->path_planner.get(),
	    DoubleVec2D(this->ele_size, this->ele_size));

	// Creating state factories
	auto state_factory1 = CreateStateFactory(
	    4, 500, this->gold_manager.get(), this->path_planner.get(),
	    DoubleVec2D(this->ele_size, this->ele_size),
	    ActorType::FACTORY_SOLDIER);
	auto state_factory2 = CreateStateFactory(
	    5, 100, this->gold_manager.get(), this->path_planner.get(),
	    DoubleVec2D(this->ele_size, this->ele_size),
	    ActorType::FACTORY_SOLDIER);
	array<vector<Factory *>, 2> state_factories = {state_factory1,
	                                               state_factory2};

	// Creating a dead factory
	auto dead_factory1 = CreateStateFactory(
	    4, 0, this->gold_manager.get(), this->path_planner.get(),
	    DoubleVec2D(1, 1), ActorType::SOLDIER);
	auto dead_factoty2 = CreateStateFactory(
	    5, 0, this->gold_manager.get(), this->path_planner.get(),
	    DoubleVec2D(1, 1), ActorType::SOLDIER);

	// Making soldiers attack each other and move at the same time
	this->player_states[0].soldiers[0].target =
	    this->player_states[0].enemy_soldiers[0].id;
	this->player_states[0].soldiers[0].destination =
	    Vec2D(this->ele_size, this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::SOLDIER);

	// Soldier trying to attack soldiers after changing id
	this->player_states[0].soldiers[0].id =
	    this->player_states[0].soldiers[0].id + 1;
	this->player_states[0].soldiers[0].target =
	    this->player_states[0].enemy_soldiers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::SOLDIER);

	// Soldier trying to attack villagers after changing id
	this->player_states[0].soldiers[0].id =
	    this->player_states[0].soldiers[0].id + 1;
	this->player_states[0].soldiers[0].target =
	    this->player_states[0].enemy_villagers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::SOLDIER);

	// Soldier trying to attack factories after changing id
	this->player_states[0].soldiers[0].id =
	    this->player_states[0].enemy_factories[0].id + 1;
	this->player_states[0].soldiers[0].target =
	    this->player_states[0].enemy_factories[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::SOLDIER);

	// Soldier trying to attack own soldier
	EXPECT_CALL(*this->command_taker, FindActorById)
	    .WillOnce(Return(nullptr))
	    .WillOnce(Return(state_soldier1[0]));
	this->player_states[0].soldiers[0].target =
	    this->player_states[0].soldiers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::SOLDIER);

	// Soldier trying to attack own villager
	EXPECT_CALL(*this->command_taker, FindActorById)
	    .WillOnce(Return(nullptr))
	    .WillOnce(Return(state_villager1[0]));
	this->player_states[0].soldiers[0].target =
	    this->player_states[0].villagers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::SOLDIER);

	// Soldier trying to attack own factory
	EXPECT_CALL(*this->command_taker, FindActorById)
	    .WillOnce(Return(nullptr))
	    .WillOnce(Return(state_factory1[0]));
	this->player_states[0].soldiers[0].target =
	    this->player_states[0].factories[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::SOLDIER);

	// Soldier trying to move out of the map
	this->player_states[0].soldiers[0].destination =
	    Vec2D(this->map_size * this->ele_size, this->map_size * this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::SOLDIER);

	// Assigning player 2's soldier as dead
	state_soldiers[1] = dead_soldier2;

	// Soldier trying to attack a dead soldier
	EXPECT_CALL(*this->command_taker, FindActorById)
	    .WillOnce(Return(state_soldiers[1][0]))
	    .WillOnce(Return(state_soldiers[1][0]));
	this->player_states[0].soldiers[0].target =
	    this->player_states[0].enemy_soldiers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::SOLDIER);

	// Making player 1's soldier as dead and player 2's soldier as alive
	state_soldiers[0] = dead_soldier1;
	state_soldiers[1] = state_soldier2;

	// Dead soldier trying to move
	this->player_states[0].soldiers[0].destination =
	    Vec2D(this->ele_size, this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::SOLDIER);

	// Dead soldier trying to attack
	this->player_states[0].soldiers[0].target =
	    this->player_states[0].enemy_soldiers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::SOLDIER);

	// Resetting state soldiers
	state_soldiers = {state_soldier1, state_soldier2};

	// Villager errors
	EXPECT_CALL(*this->logger,
	            LogError(PlayerId::PLAYER1, ErrorType::NO_ALTER_ACTOR_ID, _))
	    .Times(3);
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, ErrorType::NO_MULTIPLE_VILLAGER_TASKS, _))
	    .Times(10);
	EXPECT_CALL(*this->logger, LogError(PlayerId::PLAYER1,
	                                    ErrorType::INVALID_MOVE_POSITION, _))
	    .Times(1);
	EXPECT_CALL(*this->logger, LogError(PlayerId::PLAYER1,
	                                    ErrorType::INVALID_BUILD_POSITION, _))
	    .Times(2);
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, logger::ErrorType::INSUFFICIENT_FUNDS, _));
	EXPECT_CALL(*this->logger, LogError(PlayerId::PLAYER1,
	                                    ErrorType::INVALID_MINE_POSITION, _))
	    .Times(3);
	EXPECT_CALL(*this->logger,
	            LogError(PlayerId::PLAYER1,
	                     logger::ErrorType::NO_BUILD_FACTORY_THAT_DOSENT_EXIST,
	                     _));

	// Villager changing it's id and attacks soldiers
	this->player_states[0].villagers[0].id =
	    this->player_states[0].villagers[0].id + 1;
	this->player_states[0].villagers[0].target =
	    this->player_states[0].enemy_villagers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Villager trying to attack villagers after changing id
	this->player_states[0].villagers[0].id =
	    this->player_states[0].villagers[0].id + 1;
	this->player_states[0].villagers[0].target =
	    this->player_states[0].enemy_villagers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Villager trying to attack factories after changing id
	this->player_states[0].villagers[0].id =
	    this->player_states[0].villagers[0].id + 1;
	this->player_states[0].villagers[0].target =
	    this->player_states[0].enemy_factories[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Villager trying to attack and move
	this->player_states[0].villagers[0].target =
	    this->player_states[0].enemy_villagers[0].id;
	this->player_states[0].villagers[0].destination =
	    Vec2D(this->ele_size, this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Villager trying to attack and mine gold
	this->player_states[0].villagers[0].target =
	    this->player_states[0].enemy_soldiers[0].id;
	this->player_states[0].villagers[0].mine_target =
	    Vec2D(this->ele_size, this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Villager trying to attack and create factory at the same time
	this->player_states[0].villagers[0].target =
	    this->player_states[0].enemy_soldiers[0].id;
	this->player_states[0].villagers[0].build_position =
	    Vec2D(this->ele_size, this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Villager trying to attack and build factory
	this->player_states[0].villagers[0].target =
	    this->player_states[0].enemy_soldiers[0].id;
	this->player_states[0].villagers[0].target_factory_id =
	    this->player_states[0].factories[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Villager trying to move and mine gold
	this->player_states[0].villagers[0].destination =
	    Vec2D(this->ele_size, this->ele_size);
	this->player_states[0].villagers[0].mine_target =
	    Vec2D(this->ele_size, this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Villager trying to move and create factory
	this->player_states[0].villagers[0].destination =
	    Vec2D(this->ele_size, this->ele_size);
	this->player_states[0].villagers[0].build_position =
	    Vec2D(this->ele_size, this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Villager trying to move and build factory
	this->player_states[0].villagers[0].destination =
	    Vec2D(this->ele_size, this->ele_size);
	this->player_states[0].villagers[0].target_factory_id =
	    this->player_states[0].factories[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Villager trying to mine target and create factory
	this->player_states[0].villagers[0].mine_target =
	    Vec2D(this->ele_size, this->ele_size);
	this->player_states[0].villagers[0].build_position =
	    Vec2D(this->ele_size, this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Villager trying to mine target and build factory
	this->player_states[0].villagers[0].mine_target =
	    Vec2D(this->ele_size, this->ele_size);
	this->player_states[0].villagers[0].target_factory_id =
	    this->player_states[0].factories[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Villager trying to create factory and build factory
	this->player_states[0].villagers[0].build_position =
	    Vec2D(this->ele_size, this->ele_size);
	this->player_states[0].villagers[0].target_factory_id =
	    this->player_states[0].factories[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Making villager go out of the map
	this->player_states[0].villagers[0].destination =
	    Vec2D(this->map_size * this->ele_size, this->map_size * this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Making villager create a factory outside map
	this->player_states[0].villagers[0].build_position =
	    Vec2D(this->map_size * this->ele_size, this->map_size * this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Making villager create a factory on water
	this->player_states[0].villagers[0].build_position =
	    Vec2D(this->map_size * this->ele_size, this->map_size * this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	auto player_gold2 = this->player_gold;
	player_gold2[0] = 0;

	EXPECT_CALL(*this->command_taker, GetGold)
	    .WillRepeatedly(Return(player_gold2));

	// Villager trying to create a factory with insufficient funds
	this->player_states[0].villagers[0].build_position =
	    Vec2D(this->map_size, 0);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Resetting the amount of player gold
	EXPECT_CALL(*this->command_taker, GetGold)
	    .WillRepeatedly(Return(this->player_gold));

	// Making villagers try and mine outside map
	this->player_states[0].villagers[0].mine_target =
	    Vec2D(this->map_size * this->ele_size, this->map_size * this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Making villagers mine land
	this->player_states[0].villagers[0].mine_target = Vec2D(1, 0);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Making villager mine water
	this->player_states[0].villagers[0].mine_target = Vec2D(1, 1);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Making villager build factory that dosen't exist
	this->player_states[0].villagers[0].target_factory_id =
	    this->player_states[0].enemy_soldiers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Writing tests for factories
	EXPECT_CALL(*this->logger,
	            LogError(PlayerId::PLAYER1,
	                     logger::ErrorType::NO_ACTION_BY_DEAD_FACTORY, _));
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, logger::ErrorType::NO_ALTER_ACTOR_ID, _));
	EXPECT_CALL(
	    *this->logger,
	    LogError(PlayerId::PLAYER1, logger::ErrorType::NO_MORE_FACTORIES, _));

	// Making a factory change it's actor id
	this->player_states[0].factories[0].id += 1;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(),
	                        ActorType::FACTORY_SOLDIER);

	// Making a dead factory try and produce soldiers
	state_factories[0] = dead_factory1;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(),
	                        ActorType::FACTORY_SOLDIER);

	// Creating 50 soldiers to trigger maximum limit on number of soldiers
	vector<Factory *> state_max_factories;
	for (int i = 0; i < MAX_NUM_FACTORIES; ++i) {
		auto new_factory = CreateStateFactory(
		    4, 500, this->gold_manager.get(), this->path_planner.get(),
		    DoubleVec2D(this->ele_size, this->ele_size),
		    ActorType::FACTORY_SOLDIER);
		state_max_factories.push_back(new_factory[0]);
	}
	state_factories[0] = state_max_factories;
	this->player_states[0].villagers[0].build_position =
	    Vec2D(3 * this->map_size, 0 * this->map_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Resetting factories
	state_factories[0] = state_factory1;

	// Executing valid actions
	// Soldiers attacking all targets
	EXPECT_CALL(*this->command_taker, FindActorById)
	    .WillOnce(Return(state_soldier2[0]))
	    .WillOnce(Return(state_soldier2[0]));
	EXPECT_CALL(*this->command_taker,
	            AttackActor(PlayerId::PLAYER1,
	                        this->player_states[0].soldiers[0].id,
	                        this->player_states[0].enemy_soldiers[0].id));
	this->player_states[0].soldiers[0].target =
	    this->player_states[0].enemy_soldiers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::SOLDIER);
	EXPECT_CALL(*this->command_taker, FindActorById)
	    .WillOnce(Return(state_villager2[0]))
	    .WillOnce(Return(state_villager2[0]));
	EXPECT_CALL(*this->command_taker,
	            AttackActor(PlayerId::PLAYER1,
	                        this->player_states[0].soldiers[0].id,
	                        this->player_states[0].enemy_villagers[0].id));
	this->player_states[0].soldiers[0].target =
	    this->player_states[0].enemy_villagers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::SOLDIER);
	EXPECT_CALL(*this->command_taker,
	            AttackActor(PlayerId::PLAYER1,
	                        this->player_states[0].soldiers[0].id,
	                        this->player_states[0].enemy_factories[0].id));
	EXPECT_CALL(*this->command_taker, FindActorById)
	    .WillOnce(Return(state_factory2[0]))
	    .WillOnce(Return(state_factory2[0]));
	this->player_states[0].soldiers[0].target =
	    this->player_states[0].enemy_factories[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::SOLDIER);
	// Villagers attack all targets
	EXPECT_CALL(*this->command_taker, FindActorById)
	    .WillOnce(Return(state_soldier1[0]))
	    .WillOnce(Return(state_soldier1[0]));
	EXPECT_CALL(*this->command_taker,
	            AttackActor(PlayerId::PLAYER1,
	                        this->player_states[0].villagers[0].id,
	                        this->player_states[0].enemy_soldiers[0].id));
	this->player_states[0].villagers[0].target =
	    this->player_states[0].enemy_soldiers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);
	EXPECT_CALL(*this->command_taker, FindActorById)
	    .WillOnce(Return(state_villager2[0]))
	    .WillOnce(Return(state_villager2[0]));
	EXPECT_CALL(*this->command_taker,
	            AttackActor(PlayerId::PLAYER1,
	                        this->player_states[0].villagers[0].id,
	                        this->player_states[0].enemy_villagers[0].id));
	this->player_states[0].villagers[0].target =
	    this->player_states[0].enemy_villagers[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);
	EXPECT_CALL(*this->command_taker, FindActorById)
	    .WillOnce(Return(state_factory2[0]))
	    .WillOnce(Return(state_factory2[0]));
	EXPECT_CALL(*this->command_taker,
	            AttackActor(PlayerId::PLAYER1,
	                        this->player_states[0].villagers[0].id,
	                        this->player_states[0].enemy_factories[0].id));
	this->player_states[0].villagers[0].target =
	    this->player_states[0].enemy_factories[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Villagers creating and building factories
	EXPECT_CALL(*this->command_taker,
	            CreateFactory(PlayerId::PLAYER1,
	                          this->player_states[0].villagers[0].id,
	                          Vec2D(3 * this->map_size, 0 * this->map_size)));
	this->player_states[0].villagers[0].build_position =
	    Vec2D(3 * this->map_size, 0 * this->map_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	EXPECT_CALL(*this->command_taker,
	            BuildFactory(PlayerId::PLAYER1,
	                         this->player_states[0].villagers[0].id,
	                         this->player_states[0].factories[0].id));
	this->player_states[0].villagers[0].target_factory_id =
	    this->player_states[0].factories[0].id;
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);

	// Villagers mining gold and moving
	EXPECT_CALL(*this->command_taker,
	            MineLocation(PlayerId::PLAYER1,
	                         this->player_states[0].villagers[0].id,
	                         Vec2D(4, 2)));
	this->player_states[0].villagers[0].mine_target = Vec2D(4, 2);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);
	EXPECT_CALL(*this->command_taker,
	            MoveUnit(PlayerId::PLAYER1,
	                     this->player_states[0].villagers[0].id,
	                     Vec2D(this->ele_size, this->ele_size)));
	this->player_states[0].villagers[0].destination =
	    Vec2D(this->ele_size, this->ele_size);
	ManageActorExpectations(state_soldiers, state_villagers, state_factories,
	                        this->command_taker.get(), this->player_states,
	                        this->command_giver.get(), ActorType::VILLAGER);
}
