#include "constants/constants.h"
#include "drivers/main_driver.h"
#include "drivers/shared_memory_utils/shared_memory_player.h"
#include "drivers/timer.h"
#include "drivers/transfer_state.h"
#include "logger/mocks/logger_mock.h"
#include "state/mocks/state_syncer_mock.h"
#include "gtest/gtest.h"
#include <atomic>
#include <cmath>
#include <iostream>
#include <memory>
#include <thread>

using namespace ::testing;
using namespace std;
using namespace state;
using namespace drivers;

class MainDriverTest : public testing::Test {
  protected:
	unique_ptr<MainDriver> driver;

	const static vector<string> shared_memory_names;

	const static int num_turns;

	const static int time_limit_ms;

	const static int turn_instruction_limit;

	const static int game_instruction_limit;

	// Returns a new mock main driver
	static unique_ptr<MainDriver>
	CreateMockMainDriver(unique_ptr<StateSyncerMock> state_syncer_mock,
	                     unique_ptr<LoggerMock> v_logger) {
		vector<unique_ptr<SharedMemoryMain>> shm;
		for (const auto &shm_name : shared_memory_names) {
			// Remove shm if it already exists
			boost::interprocess::shared_memory_object::remove(shm_name.c_str());
			// Create new shm
			shm.emplace_back(new SharedMemoryMain(shm_name, false, false, 0,
			                                      transfer_state::State()));
		}

		return unique_ptr<MainDriver>(new MainDriver(
		    move(state_syncer_mock), move(shm), turn_instruction_limit,
		    game_instruction_limit, num_turns, Timer::Interval(time_limit_ms),
		    move(v_logger), "game.log"));
	}

  public:
	MainDriverTest() : driver(nullptr) {}
};

const vector<string> MainDriverTest::shared_memory_names = {"ShmTest1",
                                                            "ShmTest2"};
const int MainDriverTest::num_turns = pow(10, 4);
const int MainDriverTest::time_limit_ms = 1000;
const int MainDriverTest::turn_instruction_limit = 5;
const int MainDriverTest::game_instruction_limit = 10;

TEST_F(MainDriverTest, CleanRunByScore) {
	// Expect a lock-free atomic_bool as only they will definitely work in shm.
	// TODO: find some way to use atomic_flag, or something else that's fast and
	// lock-free.
	EXPECT_EQ(ATOMIC_BOOL_LOCK_FREE, 2);

	// Declaring mock state syncer and setting expectations
	unique_ptr<StateSyncerMock> state_syncer_mock(new StateSyncerMock());

	EXPECT_CALL(*state_syncer_mock, UpdateMainState(_, _)).Times(num_turns);
	EXPECT_CALL(*state_syncer_mock, IsGameOver(_)).Times(num_turns);
	EXPECT_CALL(*state_syncer_mock, UpdatePlayerStates(_)).Times(num_turns + 1);

	// Expect scores and interestingness calls
	EXPECT_CALL(*state_syncer_mock, GetScores(_))
	    .WillOnce(Return(array<int64_t, 2>{10, 10}));
	EXPECT_CALL(*state_syncer_mock, GetInterestingness()).WillOnce(Return(69));

	// Declare mock logger and setting expectations
	unique_ptr<LoggerMock> v_logger(new LoggerMock());

	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER1, _))
	    .Times(num_turns);
	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER2, _))
	    .Times(num_turns);
	EXPECT_CALL(*v_logger, LogFinalGameParams(_, _, _)).Times(1);
	EXPECT_CALL(*v_logger, WriteGame(_)).Times(1);

	driver = CreateMockMainDriver(move(state_syncer_mock), move(v_logger));

	// Start main driver on new thread
	GameResult game_result;
	thread main_runner([this, &game_result] { game_result = driver->Start(); });

	vector<thread> player_runners;
	for (int i = 0; i < 2; ++i) {
		ostringstream command_stream;
		command_stream << "./main_driver_test_player " << shared_memory_names[i]
		               << ' ' << time_limit_ms << ' ' << num_turns << ' '
		               << turn_instruction_limit;
		string command = command_stream.str();
		player_runners.emplace_back(
		    [command] { EXPECT_EQ(system(command.c_str()), 0); });
	}

	// Wait for player processes to wrap up
	for (auto &runner : player_runners) {
		runner.join();
	}
	// Wait for main driver to wrap up
	main_runner.join();

	// Everyone gets a score of 10 and status is normal as game finished
	// normally
	EXPECT_EQ(game_result.winner, GameResult::Winner::TIE);
	EXPECT_EQ(game_result.win_type, GameResult::WinType::SCORE);
	for (auto const &result : game_result.player_results) {
		EXPECT_EQ(result.score, 10);
		EXPECT_EQ(result.status, PlayerResult::Status::NORMAL);
	}
}

TEST_F(MainDriverTest, Player1WinByDeathmatch) {
	// Declaring mock state syncer and setting expectations
	unique_ptr<StateSyncerMock> state_syncer_mock(new StateSyncerMock());

	// The game will end halfway in, with IsGameOver returning true
	EXPECT_CALL(*state_syncer_mock, UpdateMainState(_, _)).Times(num_turns / 2);
	EXPECT_CALL(*state_syncer_mock, UpdatePlayerStates(_))
	    .Times(num_turns / 2 + 1);

	EXPECT_CALL(*state_syncer_mock, IsGameOver(_))
	    .Times(1)
	    .WillRepeatedly(
	        DoAll(SetArgReferee<0>(PlayerId::PLAYER1), Return(true)))
	    .RetiresOnSaturation();

	EXPECT_CALL(*state_syncer_mock, IsGameOver(_))
	    .Times((num_turns / 2) - 1)
	    .WillRepeatedly(Return(false))
	    .RetiresOnSaturation();

	// Expect scores and interestingness calls
	EXPECT_CALL(*state_syncer_mock, GetScores(_))
	    .WillOnce(Return(array<int64_t, 2>{10, 10}));
	EXPECT_CALL(*state_syncer_mock, GetInterestingness()).WillOnce(Return(69));

	// Declare mock logger and setting expectations
	unique_ptr<LoggerMock> v_logger(new LoggerMock());

	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER1, _))
	    .Times(num_turns / 2);
	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER2, _))
	    .Times(num_turns / 2);
	EXPECT_CALL(*v_logger, LogFinalGameParams(_, _, _)).Times(1);
	EXPECT_CALL(*v_logger, WriteGame(_)).Times(1);

	driver = CreateMockMainDriver(move(state_syncer_mock), move(v_logger));

	// Start main driver on new thread
	GameResult game_result;
	thread main_runner([this, &game_result] { game_result = driver->Start(); });

	vector<thread> player_runners;
	for (int i = 0; i < 2; ++i) {
		ostringstream command_stream;
		command_stream << "./main_driver_test_player " << shared_memory_names[i]
		               << ' ' << time_limit_ms << ' ' << num_turns << ' '
		               << turn_instruction_limit;
		string command = command_stream.str();
		player_runners.emplace_back(
		    [command] { EXPECT_EQ(system(command.c_str()), 0); });
	}

	// Wait for player processes to wrap up
	for (auto &runner : player_runners) {
		runner.join();
	}
	// Wait for main driver to wrap up
	main_runner.join();

	// Everyone gets a score of 10 and status is normal as game finished
	// normally
	EXPECT_EQ(game_result.winner, GameResult::Winner::PLAYER1);
	EXPECT_EQ(game_result.win_type, GameResult::WinType::DEATHMATCH);
	for (auto const &result : game_result.player_results) {
		EXPECT_EQ(result.score, 10);
		EXPECT_EQ(result.status, PlayerResult::Status::NORMAL);
	}
}

TEST_F(MainDriverTest, Player2WinByDeathmatch) {
	// Declaring mock state syncer and setting expectations
	unique_ptr<StateSyncerMock> state_syncer_mock(new StateSyncerMock());

	// The game will end halfway in, with IsGameOver returning true
	EXPECT_CALL(*state_syncer_mock, UpdateMainState(_, _)).Times(num_turns / 2);
	EXPECT_CALL(*state_syncer_mock, UpdatePlayerStates(_))
	    .Times(num_turns / 2 + 1);

	EXPECT_CALL(*state_syncer_mock, IsGameOver(_))
	    .Times(1)
	    .WillRepeatedly(
	        DoAll(SetArgReferee<0>(PlayerId::PLAYER2), Return(true)))
	    .RetiresOnSaturation();

	EXPECT_CALL(*state_syncer_mock, IsGameOver(_))
	    .Times((num_turns / 2) - 1)
	    .WillRepeatedly(Return(false))
	    .RetiresOnSaturation();

	// Expect scores and interestingness calls
	EXPECT_CALL(*state_syncer_mock, GetScores(_))
	    .WillOnce(Return(array<int64_t, 2>{10, 10}));
	EXPECT_CALL(*state_syncer_mock, GetInterestingness()).WillOnce(Return(69));

	// Declare mock logger and setting expectations
	unique_ptr<LoggerMock> v_logger(new LoggerMock());

	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER1, _))
	    .Times(num_turns / 2);
	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER2, _))
	    .Times(num_turns / 2);
	EXPECT_CALL(*v_logger, LogFinalGameParams(_, _, _)).Times(1);
	EXPECT_CALL(*v_logger, WriteGame(_)).Times(1);

	driver = CreateMockMainDriver(move(state_syncer_mock), move(v_logger));

	// Start main driver on new thread
	GameResult game_result;
	thread main_runner([this, &game_result] { game_result = driver->Start(); });

	vector<thread> player_runners;
	for (int i = 0; i < 2; ++i) {
		ostringstream command_stream;
		command_stream << "./main_driver_test_player " << shared_memory_names[i]
		               << ' ' << time_limit_ms << ' ' << num_turns << ' '
		               << turn_instruction_limit;
		string command = command_stream.str();
		player_runners.emplace_back(
		    [command] { EXPECT_EQ(system(command.c_str()), 0); });
	}

	// Wait for player processes to wrap up
	for (auto &runner : player_runners) {
		runner.join();
	}
	// Wait for main driver to wrap up
	main_runner.join();

	// Everyone gets a score of 10 and status is normal as game finished
	// normally
	EXPECT_EQ(game_result.winner, GameResult::Winner::PLAYER2);
	EXPECT_EQ(game_result.win_type, GameResult::WinType::DEATHMATCH);
	for (auto const &result : game_result.player_results) {
		EXPECT_EQ(result.score, 10);
		EXPECT_EQ(result.status, PlayerResult::Status::NORMAL);
	}
}

TEST_F(MainDriverTest, TieByDeathmatch) {
	// Declaring mock state syncer and setting expectations
	unique_ptr<StateSyncerMock> state_syncer_mock(new StateSyncerMock());

	// The game will end halfway in, with IsGameOver returning true
	EXPECT_CALL(*state_syncer_mock, UpdateMainState(_, _)).Times(num_turns / 2);
	EXPECT_CALL(*state_syncer_mock, UpdatePlayerStates(_))
	    .Times(num_turns / 2 + 1);

	EXPECT_CALL(*state_syncer_mock, IsGameOver(_))
	    .Times(1)
	    .WillRepeatedly(
	        DoAll(SetArgReferee<0>(PlayerId::PLAYER_NULL), Return(true)))
	    .RetiresOnSaturation();

	EXPECT_CALL(*state_syncer_mock, IsGameOver(_))
	    .Times((num_turns / 2) - 1)
	    .WillRepeatedly(Return(false))
	    .RetiresOnSaturation();

	// Expect scores and interestingness calls
	EXPECT_CALL(*state_syncer_mock, GetScores(_))
	    .WillOnce(Return(array<int64_t, 2>{10, 10}));
	EXPECT_CALL(*state_syncer_mock, GetInterestingness()).WillOnce(Return(69));

	// Declare mock logger and setting expectations
	unique_ptr<LoggerMock> v_logger(new LoggerMock());

	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER1, _))
	    .Times(num_turns / 2);
	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER2, _))
	    .Times(num_turns / 2);
	EXPECT_CALL(*v_logger, LogFinalGameParams(_, _, _)).Times(1);
	EXPECT_CALL(*v_logger, WriteGame(_)).Times(1);

	driver = CreateMockMainDriver(move(state_syncer_mock), move(v_logger));

	// Start main driver on new thread
	GameResult game_result;
	thread main_runner([this, &game_result] { game_result = driver->Start(); });

	vector<thread> player_runners;
	for (int i = 0; i < 2; ++i) {
		ostringstream command_stream;
		command_stream << "./main_driver_test_player " << shared_memory_names[i]
		               << ' ' << time_limit_ms << ' ' << num_turns << ' '
		               << turn_instruction_limit;
		string command = command_stream.str();
		player_runners.emplace_back(
		    [command] { EXPECT_EQ(system(command.c_str()), 0); });
	}

	// Wait for player processes to wrap up
	for (auto &runner : player_runners) {
		runner.join();
	}
	// Wait for main driver to wrap up
	main_runner.join();

	// Everyone gets a score of 10 and status is normal as game finished
	// normally
	EXPECT_EQ(game_result.winner, GameResult::Winner::TIE);
	EXPECT_EQ(game_result.win_type, GameResult::WinType::DEATHMATCH);
	for (auto const &result : game_result.player_results) {
		EXPECT_EQ(result.score, 10);
		EXPECT_EQ(result.status, PlayerResult::Status::NORMAL);
	}
}

// Tests for case when player exits/crashes before game ends
// Main driver should simply time out and exit cleanly
TEST_F(MainDriverTest, EarlyPlayerExit) {
	unique_ptr<StateSyncerMock> state_syncer_mock(new StateSyncerMock());

	// Expect only half the number of turns to be run
	EXPECT_CALL(*state_syncer_mock, UpdateMainState(_, _)).Times(num_turns / 2);
	EXPECT_CALL(*state_syncer_mock, IsGameOver(_)).Times(num_turns / 2);
	EXPECT_CALL(*state_syncer_mock, UpdatePlayerStates(_))
	    .Times(num_turns / 2 + 1);

	// Get Scores and interestingness WILL NOT be called
	EXPECT_CALL(*state_syncer_mock, GetScores(_)).Times(0);
	EXPECT_CALL(*state_syncer_mock, GetInterestingness()).Times(0);

	// Logger called num_turns/2 + 1 times before the driver exits
	unique_ptr<LoggerMock> v_logger(new LoggerMock());
	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER1, _))
	    .Times(num_turns / 2 + 1);
	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER2, _))
	    .Times(num_turns / 2 + 1);
	EXPECT_CALL(*v_logger, LogFinalGameParams(_, _, _)).Times(1);
	EXPECT_CALL(*v_logger, WriteGame(_)).Times(1);

	driver = CreateMockMainDriver(move(state_syncer_mock), move(v_logger));

	GameResult game_result;
	thread main_runner([this, &game_result] { game_result = driver->Start(); });

	vector<thread> player_runners;
	for (int i = 0; i < 2; ++i) {
		ostringstream command_stream;
		command_stream << "./main_driver_test_player " << shared_memory_names[i]
		               << ' ' << time_limit_ms << ' ' << num_turns / 2 << ' '
		               << turn_instruction_limit;
		string command = command_stream.str();
		player_runners.emplace_back(
		    [command] { EXPECT_EQ(system(command.c_str()), 0); });
	}

	for (auto &runner : player_runners) {
		runner.join();
	}
	main_runner.join();

	EXPECT_EQ(game_result.winner, GameResult::Winner::NONE);
	EXPECT_EQ(game_result.win_type, GameResult::WinType::NONE);
	// Results are undefined as game didn't complete
	for (auto result : game_result.player_results) {
		EXPECT_EQ(result.status, PlayerResult::Status::UNDEFINED);
	}
}

// Test for when a player exceeds game instruction limit
// Main driver should mark the player as having exceeded instruction limit
TEST_F(MainDriverTest, InstructionLimitReached) {
	unique_ptr<StateSyncerMock> state_syncer_mock(new StateSyncerMock());

	// Expect only half the turns to run
	EXPECT_CALL(*state_syncer_mock, UpdateMainState(_, _)).Times(num_turns / 2);
	EXPECT_CALL(*state_syncer_mock, IsGameOver(_)).Times(num_turns / 2);
	EXPECT_CALL(*state_syncer_mock, UpdatePlayerStates(_))
	    .Times(num_turns / 2 + 1);

	// Get Scores and interestingness WILL NOT be called
	EXPECT_CALL(*state_syncer_mock, GetScores(_)).Times(0);
	EXPECT_CALL(*state_syncer_mock, GetInterestingness()).Times(0);

	unique_ptr<LoggerMock> v_logger(new LoggerMock());
	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER1, _))
	    .Times(num_turns / 2 + 1);
	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER2, _))
	    .Times(num_turns / 2 + 1);
	EXPECT_CALL(*v_logger, LogFinalGameParams(_, _, _)).Times(1);
	EXPECT_CALL(*v_logger, WriteGame(_)).Times(1);

	driver = CreateMockMainDriver(move(state_syncer_mock), move(v_logger));

	GameResult game_result;
	thread main_runner([this, &game_result] { game_result = driver->Start(); });

	vector<thread> player_runners;
	for (int i = 0; i < 2; ++i) {
		ostringstream command_stream;
		command_stream << "./main_driver_test_player " << shared_memory_names[i]
		               << ' ' << time_limit_ms << ' ' << num_turns / 2 - 1
		               << ' ' << turn_instruction_limit;
		string command = command_stream.str();
		player_runners.emplace_back(
		    [command] { EXPECT_EQ(system(command.c_str()), 0); });
	}

	for (auto &runner : player_runners) {
		runner.join();
	}

	// Simulating just being under instruction limit on n/2 turn by all
	// players
	for (const auto &shm_name : shared_memory_names) {
		SharedMemoryPlayer shm_player(shm_name);
		SharedBuffer *buf = shm_player.GetBuffer();
		buf->instruction_counter = game_instruction_limit;
		buf->is_player_running = false;
	}

	// Simulating instruction limit exceeding on n/2 + 1 turn by all players
	for (const auto &shm_name : shared_memory_names) {
		SharedMemoryPlayer shm_player(shm_name);
		SharedBuffer *buf = shm_player.GetBuffer();
		buf->instruction_counter = game_instruction_limit + 1;
		buf->is_player_running = false;
	}

	main_runner.join();

	EXPECT_EQ(game_result.winner, GameResult::Winner::TIE);
	EXPECT_EQ(game_result.win_type,
	          GameResult::WinType::EXCEEDED_INSTRUCTION_LIMIT);
	for (auto result : game_result.player_results) {
		EXPECT_EQ(result.status,
		          PlayerResult::Status::EXCEEDED_INSTRUCTION_LIMIT);
	}
}

// Test for cancelling main driver
// Simulate a turn and then check if cancelling works
TEST_F(MainDriverTest, Cancellation) {
	unique_ptr<StateSyncerMock> state_syncer_mock(new StateSyncerMock());

	// Expect only one turn to run
	EXPECT_CALL(*state_syncer_mock, UpdateMainState(_, _)).Times(1);
	EXPECT_CALL(*state_syncer_mock, IsGameOver(_)).Times(1);
	EXPECT_CALL(*state_syncer_mock, UpdatePlayerStates(_)).Times(2);

	// Get Scores and interestingness WILL NOT be called
	EXPECT_CALL(*state_syncer_mock, GetScores(_)).Times(0);
	EXPECT_CALL(*state_syncer_mock, GetInterestingness()).Times(0);

	unique_ptr<LoggerMock> v_logger(new LoggerMock());
	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER1, _)).Times(1);
	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER2, _)).Times(1);
	EXPECT_CALL(*v_logger, LogFinalGameParams(_, _, _)).Times(1);
	EXPECT_CALL(*v_logger, WriteGame(_)).Times(1);

	driver = CreateMockMainDriver(move(state_syncer_mock), move(v_logger));

	GameResult game_result;
	thread main_runner([this, &game_result] { game_result = driver->Start(); });

	// Simulating one turn
	for (const auto &shm_name : shared_memory_names) {
		SharedMemoryPlayer shm_player(shm_name);
		SharedBuffer *buf = shm_player.GetBuffer();
		while (!buf->is_player_running)
			;
		buf->is_player_running = false;
	}

	SharedMemoryPlayer shm_player(shared_memory_names[0]);
	SharedBuffer *buf = shm_player.GetBuffer();
	while (!buf->is_player_running)
		;

	driver->Cancel();

	main_runner.join();

	EXPECT_EQ(game_result.winner, GameResult::Winner::NONE);
	EXPECT_EQ(game_result.win_type, GameResult::WinType::NONE);
	for (auto result : game_result.player_results) {
		EXPECT_EQ(result.status, PlayerResult::Status::UNDEFINED);
	}
}
