#include "constants/constants.h"
#include "logger/logger.h"
#include "state/mocks/command_taker_mock.h"
#include "state/mocks/gold_manager_mock.h"
#include "state/utilities.h"
#include "gtest/gtest.h"
#include <sstream>

using namespace std;
using namespace ::testing;
using namespace state;
using namespace logger;

const auto TEST_MAP_SIZE = size_t{5};

const auto L = TerrainType::LAND;
const auto W = TerrainType::WATER;
const auto G = TerrainType::GOLD_MINE;

class LoggerTest : public testing::Test {
  protected:
	std::unique_ptr<CommandTakerMock> state;
	std::unique_ptr<Logger> logger;

	std::unique_ptr<GoldManagerMock> gold_manager;
	std::unique_ptr<PathPlanner> path_planner;

	std::unique_ptr<Map> map;

	int64_t villager_frequency = 5;
	int64_t soldier_frequency = 10;

	LoggerTest()
	    : state(make_unique<CommandTakerMock>()),
	      logger(make_unique<Logger>(state.get(), PLAYER_INSTRUCTION_LIMIT_TURN,
	                                 PLAYER_INSTRUCTION_LIMIT_GAME,
	                                 SOLDIER_MAX_HP, VILLAGER_MAX_HP,
	                                 FACTORY_MAX_HP)),
	      gold_manager(make_unique<GoldManagerMock>()) {
		auto map_matrix = vector<vector<TerrainType>>{{
		    {L, L, L, L, L},
		    {L, L, L, L, L},
		    {G, L, L, L, G},
		    {W, L, L, L, W},
		    {W, W, W, W, W},
		}};
		map = make_unique<Map>(map_matrix, TEST_MAP_SIZE, ELEMENT_SIZE);
	}
};

TEST_F(LoggerTest, WriteReadTest) {
	Actor::SetActorIdIncrement(0);

	// Creating one soldier for each player
	auto *soldier = new Soldier(
	    Actor::GetNextActorId(), state::PlayerId::PLAYER1,
	    state::ActorType::SOLDIER, SOLDIER_MAX_HP, SOLDIER_MAX_HP,
	    DoubleVec2D(1, 1), gold_manager.get(), path_planner.get(), 10, 10, 10);
	auto *soldier2 = new Soldier(
	    Actor::GetNextActorId(), state::PlayerId::PLAYER2,
	    state::ActorType::SOLDIER, SOLDIER_MAX_HP, SOLDIER_MAX_HP,
	    DoubleVec2D(2, 2), gold_manager.get(), path_planner.get(), 10, 10, 10);

	std::array<vector<Soldier *>, 2> soldiers;
	vector<Soldier *> player_soldiers;
	vector<Soldier *> player_soldiers2;
	player_soldiers.push_back(soldier);
	player_soldiers2.push_back(soldier2);
	soldiers[0] = player_soldiers;
	soldiers[1] = player_soldiers2;

	// Creating one villager for each player
	auto *villager = new Villager(
	    Actor::GetNextActorId(), PlayerId::PLAYER1, ActorType::VILLAGER,
	    VILLAGER_MAX_HP, VILLAGER_MAX_HP, DoubleVec2D(1, 2), gold_manager.get(),
	    path_planner.get(), 10, 10, 10, 10, 10, 10);
	auto *villager2 = new Villager(
	    Actor::GetNextActorId(), PlayerId::PLAYER2, ActorType::VILLAGER,
	    VILLAGER_MAX_HP, VILLAGER_MAX_HP, DoubleVec2D(2, 3), gold_manager.get(),
	    path_planner.get(), 10, 10, 10, 10, 10, 10);

	std::array<vector<Villager *>, 2> villagers;
	vector<Villager *> player_villagers;
	vector<Villager *> player_villagers2;
	player_villagers.push_back(villager);
	player_villagers2.push_back(villager2);
	villagers[0] = player_villagers;
	villagers[1] = player_villagers2;

	// Creating one factory for each player
	auto *factory = new Factory(Actor::GetNextActorId(), PlayerId::PLAYER1,
	                            ActorType::FACTORY, 1, FACTORY_MAX_HP,
	                            DoubleVec2D(1, 1), gold_manager.get(), 0, 100,
	                            ActorType::VILLAGER, villager_frequency,
	                            soldier_frequency, UnitProductionCallback());
	auto *factory2 = new Factory(Actor::GetNextActorId(), PlayerId::PLAYER2,
	                             ActorType::FACTORY, 1, FACTORY_MAX_HP,
	                             DoubleVec2D(3, 3), gold_manager.get(), 0, 100,
	                             ActorType::VILLAGER, villager_frequency,
	                             soldier_frequency, UnitProductionCallback());

	std::array<vector<Factory *>, 2> factories;
	vector<Factory *> player_factories;
	vector<Factory *> player_factories2;
	player_factories.push_back(factory);
	player_factories2.push_back(factory2);
	factories[0] = player_factories;
	factories[1] = player_factories2;

	// Two new factories for Player1
	auto *factory3 = new Factory(Actor::GetNextActorId(), PlayerId::PLAYER1,
	                             ActorType::FACTORY, 1, FACTORY_MAX_HP,
	                             DoubleVec2D(1, 2), gold_manager.get(), 0, 100,
	                             ActorType::VILLAGER, villager_frequency,
	                             soldier_frequency, UnitProductionCallback());
	auto *factory4 = new Factory(Actor::GetNextActorId(), PlayerId::PLAYER1,
	                             ActorType::FACTORY, 1, FACTORY_MAX_HP,
	                             DoubleVec2D(3, 4), gold_manager.get(), 0, 100,
	                             ActorType::VILLAGER, villager_frequency,
	                             soldier_frequency, UnitProductionCallback());

	std::array<vector<Factory *>, 2> factories2 = factories;
	factories2[0].push_back(factory3);
	factories2[0].push_back(factory4);

	// FACTORY DESTROY CASE
	// Make another copy and remove the middle factory
	// factory and factory4 remain in Player1
	std::array<vector<Factory *>, 2> factories3 = factories2;
	factories3[0].erase(factories3[0].begin() + 1);

	// FACTORY CHANGE CASE
	// Make another copy and alter a factories HP (done later)
	std::array<vector<Factory *>, 2> factories4 = factories3;

	// Another copy and alter a factories production state
	std::array<vector<Factory *>, 2> factories5 = factories4;

	// SIMULTANEOUS FACTORY BUILD AND DESTROY CASE
	// Make another copy, destroy both player1 factories and add a new factory
	auto *factory5 = new Factory(Actor::GetNextActorId(), PlayerId::PLAYER1,
	                             ActorType::FACTORY, 1, FACTORY_MAX_HP,
	                             DoubleVec2D(1, 4), gold_manager.get(), 0, 100,
	                             ActorType::VILLAGER, villager_frequency,
	                             soldier_frequency, UnitProductionCallback());
	std::array<vector<Factory *>, 2> factories6 = factories3;
	factories6[0].erase(factories6[0].begin(), factories6[0].begin() + 2);
	factories6[0].push_back(factory5);

	/// Set Map expectations
	EXPECT_CALL(*state, GetMap()).WillOnce(Return(map.get()));

	// Set soldier expectations
	EXPECT_CALL(*state, GetSoldiers()).WillRepeatedly(Return(soldiers));

	// Set villager expectations
	EXPECT_CALL(*state, GetVillagers()).WillRepeatedly(Return(villagers));

	// Set factory expectations
	EXPECT_CALL(*state, GetFactories())
	    .WillOnce(Return(factories))
	    .WillOnce(Return(factories2))
	    .WillOnce(Return(factories3))
	    .WillOnce(Return(factories4))
	    .WillOnce(Return(factories5))
	    .WillRepeatedly(Return(factories6));

	// Set money expectations
	std::array<int64_t, 2> money1 = {400, 500};
	std::array<int64_t, 2> money2 = {300, 600};
	EXPECT_CALL(*state, GetGold())
	    .WillOnce(Return(money1))
	    .WillRepeatedly(Return(money2));

	vector<int64_t> inst_counts = {123456, 654321};
	logger->LogInstructionCount(PlayerId::PLAYER1, inst_counts[0]);
	logger->LogInstructionCount(PlayerId::PLAYER2, inst_counts[1]);

	// Log some errors for the first turn
	logger->LogError(PlayerId::PLAYER1, ErrorType::INVALID_MOVE_POSITION,
	                 "Error 1");
	logger->LogError(PlayerId::PLAYER1, ErrorType::INVALID_MOVE_POSITION,
	                 "Error 2");
	logger->LogError(PlayerId::PLAYER2, ErrorType::INVALID_MOVE_POSITION,
	                 "Error 3");
	logger->LogError(PlayerId::PLAYER2, ErrorType::INVALID_MOVE_POSITION,
	                 "Error 4");

	// Run 3 turns
	logger->LogState();
	logger->LogState();
	logger->LogState();
	// Update Hp of factory and run
	int newHp = 25;
	factory2->SetHp(newHp);
	logger->LogState();
	// Build factory, increment build percent and run
	factory2->IncrementConstructionCompletion(
	    factory2->GetTotalConstructionCompletion());
	logger->LogState();
	// Run once more
	logger->LogState();
	logger->LogFinalGameParams();

	ostringstream str_stream;
	logger->WriteGame(str_stream);
	string result_string = str_stream.str();

	auto game = make_unique<proto::Game>();
	game->ParseFromString(result_string);

	// Check for terrain properties
	ASSERT_EQ(game->map_size(), TEST_MAP_SIZE);
	ASSERT_EQ(game->map_element_size(), ELEMENT_SIZE);
	ASSERT_EQ(game->map_elements(0), proto::LAND);
	ASSERT_EQ(game->map_elements(10), proto::GOLD_MINE);
	ASSERT_EQ(game->map_elements(20), proto::WATER);

	// Check if max hps are fine
	ASSERT_EQ(game->soldier_max_hp(), SOLDIER_MAX_HP);
	ASSERT_EQ(game->villager_max_hp(), VILLAGER_MAX_HP);
	ASSERT_EQ(game->factory_max_hp(), FACTORY_MAX_HP);

	// Check if money is fine
	ASSERT_EQ(game->states(0).gold(0), money1[0]);
	ASSERT_EQ(game->states(0).gold(1), money1[1]);
	ASSERT_EQ(game->states(1).gold(0), money2[0]);
	ASSERT_EQ(game->states(1).gold(1), money2[1]);

	// Check if soldiers are there
	ASSERT_EQ(game->states(0).soldiers_size(), 2);
	ASSERT_EQ(game->states(1).soldiers_size(), 2);

	// Check if instruction count is there
	ASSERT_EQ(game->states(0).instruction_counts_size(), 2);
	ASSERT_EQ(game->states(0).instruction_counts(0), inst_counts[0]);
	ASSERT_EQ(game->states(0).instruction_counts(1), inst_counts[1]);

	// Ensure instruction counts are cleared on next turn
	ASSERT_EQ(game->states(1).instruction_counts_size(), 2);
	ASSERT_EQ(game->states(1).instruction_counts(1), 0);
	ASSERT_EQ(game->states(1).instruction_counts(0), 0);

	// Check if the errors got logged on the first turn
	// Error codes should increment from 0
	// Player 1 errors
	ASSERT_EQ(game->states(0).player_errors(0).errors_size(), 2);
	ASSERT_EQ(game->states(0).player_errors(0).errors(0), 0);
	ASSERT_EQ(game->states(0).player_errors(0).errors(1), 1);
	// Player 2 errors
	ASSERT_EQ(game->states(0).player_errors(1).errors_size(), 2);
	ASSERT_EQ(game->states(0).player_errors(1).errors(0), 2);
	ASSERT_EQ(game->states(0).player_errors(1).errors(1), 3);

	// Check if the mapping got set and the message string matches
	auto error_map = *game->mutable_error_map();
	ASSERT_EQ(error_map[game->states(0).player_errors(0).errors(0)],
	          "INVALID_MOVE_POSITION: Error 1");
	ASSERT_EQ(error_map[game->states(0).player_errors(0).errors(1)],
	          "INVALID_MOVE_POSITION: Error 2");
	ASSERT_EQ(error_map[game->states(0).player_errors(1).errors(0)],
	          "INVALID_MOVE_POSITION: Error 3");
	ASSERT_EQ(error_map[game->states(0).player_errors(1).errors(1)],
	          "INVALID_MOVE_POSITION: Error 4");

	// Check if both factories are there in the first turn
	ASSERT_EQ(game->states(0).factories(0).id(), factory->GetActorId());
	ASSERT_EQ(game->states(0).factories(1).id(), factory2->GetActorId());
	// Check if both villagers and soldiers are there in the first turn
	ASSERT_EQ(game->states(0).soldiers(0).id(), soldier->GetActorId());
	ASSERT_EQ(game->states(0).soldiers(1).id(), soldier2->GetActorId());
	ASSERT_EQ(game->states(0).villagers(0).id(), villager->GetActorId());
	ASSERT_EQ(game->states(0).villagers(1).id(), villager2->GetActorId());

	// FACTORY BUILD CASE - 2nd turn
	// Ensure only the newly built factories are there on the second turn
	ASSERT_EQ(game->states(1).factories_size(), 2);
	ASSERT_EQ(game->states(1).factories(0).id(), factory3->GetActorId());
	ASSERT_EQ(game->states(1).factories(1).id(), factory4->GetActorId());

	// FACTORY DESTROY CASE - 3rd turn
	// Ensure only the destroyed factory is there on the third turn
	ASSERT_EQ(game->states(2).factories_size(), 1);
	ASSERT_EQ(game->states(2).factories(0).id(), factory3->GetActorId());
	ASSERT_EQ(game->states(2).factories(0).state(), proto::FACTORY_DESTROYED);

	// FACTORY HP CHANGE CASE - 4th turn
	// Ensure only the altered factory is there on the fourth turn
	ASSERT_EQ(game->states(3).factories_size(), 1);
	ASSERT_EQ(game->states(3).factories(0).id(), factory2->GetActorId());
	ASSERT_EQ(game->states(3).factories(0).hp(), newHp);

	// FACTORY PRODUCTION CHANGE CASE - 5th turn
	ASSERT_EQ(game->states(4).factories_size(), 1);
	ASSERT_EQ(game->states(4).factories(0).id(), factory2->GetActorId());
	ASSERT_EQ(game->states(4).factories(0).build_percent(),
	          factory2->GetTotalConstructionCompletion());

	// SIMULTANEOUS TOWERS BUILD AND DESTROY CASE - 6th turn
	// Ensure both dead factories and new factory on the fifth turn
	ASSERT_EQ(game->states(5).factories_size(), 3);
	ASSERT_EQ(game->states(5).factories(0).id(), factory->GetActorId());
	ASSERT_EQ(game->states(5).factories(1).id(), factory4->GetActorId());
	ASSERT_EQ(game->states(5).factories(0).state(), proto::FACTORY_DESTROYED);
	ASSERT_EQ(game->states(5).factories(1).state(), proto::FACTORY_DESTROYED);
	ASSERT_EQ(game->states(5).factories(2).id(), factory5->GetActorId());
	ASSERT_EQ(game->states(5).factories(2).state(), proto::FACTORY_UNBUILT);
	// Check if both villagers and soldiers are there in the 6th turn too
	ASSERT_EQ(game->states(5).soldiers(0).id(), soldier->GetActorId());
	ASSERT_EQ(game->states(5).soldiers(1).id(), soldier2->GetActorId());
	ASSERT_EQ(game->states(5).villagers(0).id(), villager->GetActorId());
	ASSERT_EQ(game->states(5).villagers(1).id(), villager2->GetActorId());

	delete soldier;
	delete soldier2;
	delete villager;
	delete villager2;
	delete factory;
	delete factory2;
	delete factory3;
	delete factory4;
	delete factory5;
}
