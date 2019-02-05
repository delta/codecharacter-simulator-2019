/**
 * @file game.cpp
 * Defines the game.cpp class
 */

#include "game/game.h"

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

const std::vector<drivers::PlayerResult> Game::Start() {
	// Start the player processes
	for (int i = 1; i <= 2; ++i) {
		std::string command = "./player_" + std::to_string(i) + " &";
		system(command.c_str());
	}

	// Start the main process
	auto results = main_driver->Start();

	return results;
}
