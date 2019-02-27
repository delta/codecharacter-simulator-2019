/**
 * @file game.cpp
 * Defines the game.cpp class
 */

#include "game/game.h"
#include "boost/process.hpp"
#include "drivers/game_result.h"

namespace bp = boost::process;

Game::Game(std::unique_ptr<drivers::MainDriver> main_driver)
    : main_driver(std::move(main_driver)) {}

std::string Game::GenerateRandomString(const std::string::size_type length) {
	using namespace std;
	static string chars = "0123456789"
	                      "abcdefghijklmnopqrstuvwxyz"
	                      "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	static mt19937 gen{random_device{}()};
	static uniform_int_distribution<string::size_type> dis(0,
	                                                       chars.length() - 1);

	string s(length, '*');

	for (auto &c : s)
		c = chars[dis(gen)];

	return s;
}

const drivers::GameResult Game::Start() {
	// Launching player child processes
	std::vector<bp::child> player_processes;
	std::vector<std::error_code> player_process_errors(2);
	for (int i = 0; i < 2; ++i) {
		player_processes.emplace_back("./player_" + std::to_string(i + 1), "",
		                              player_process_errors[i]);
	}

	// Starting main driver
	drivers::GameResult result;
	std::thread main_runner(
	    [this, &result] { result = this->main_driver->Start(); });

	// Monitor child processes
	// If one fails, terminate the rest

	std::vector<std::atomic_bool> players_failed(2);
	std::atomic_bool any_player_failed(false);
	std::vector<std::thread> player_monitors;

	// For each player...
	for (int player_id = 0; player_id < 2; ++player_id) {

		// Get both processes, including the opponents'
		auto &process = player_processes[player_id];
		auto &other_process = player_processes[1 - player_id];

		players_failed[player_id] = false;
		auto &player_failed = players_failed[player_id];

		// Start the player_monitors
		player_monitors.emplace_back(
		    [&process, &other_process, &player_failed, &any_player_failed] {
			    // Wait for the player processes to exit
			    process.wait();

			    // If the process did not exit gracefully, suspend the game and
			    // kill the other player process
			    if (process.exit_code() != 0 && !any_player_failed) {
				    player_failed = true;
				    any_player_failed = true;
				    kill(other_process.id(), SIGTERM);
			    }
		    });
	}

	for (auto &monitor : player_monitors) {
		monitor.join();
	}

	// If any child process failed, stop the main_driver
	// Otherwise, simply wait for the main_driver to wrap up
	if (any_player_failed) {
		main_driver->Cancel();
		main_runner.join();
		for (int player_id = 0; player_id < 2; ++player_id) {
			if (players_failed[player_id]) {
				result.player_results[player_id].status =
				    drivers::PlayerResult::Status::RUNTIME_ERROR;
				result.win_type = drivers::GameResult::WinType::RUNTIME_ERROR;
			}
		}

		// Assign winner
		if (players_failed[0] && players_failed[1]) {
			// This case is currently impossible. Driver quits on Player1 Error
			result.winner = drivers::GameResult::Winner::TIE;
		} else if (players_failed[0]) {
			result.winner = drivers::GameResult::Winner::PLAYER2;
		} else if (players_failed[1]) {
			result.winner = drivers::GameResult::Winner::PLAYER1;
		}

	} else {
		main_runner.join();
	}

	return result;
}
