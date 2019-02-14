/**
 * @file game.h
 * Declarations for Game, the apex class that contains the entire game
 */

#pragma once

#include "constants/constants.h"
#include "drivers/main_driver.h"
#include "drivers/shared_memory_utils/shared_memory_main.h"
#include "drivers/timer.h"
#include "game/game_export.h"
#include "logger/logger.h"
#include "physics/vector.hpp"
#include "simulator_constants/constants.h"
#include "state/actor/actor.h"
#include "state/actor/factory.h"
#include "state/actor/soldier.h"
#include "state/actor/villager.h"
#include "state/command_giver.h"
#include "state/map/map.h"
#include "state/path_planner/path_planner.h"
#include "state/player_state.h"
#include "state/state.h"
#include "state/state_syncer.h"
#include "state/utilities.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>

/**
 * Class representing the entire game. Controls the main thread and player
 * processes
 */
class GAME_EXPORT Game {
  private:
	/**
	 * Main driver instance where the game calculations are run
	 */
	std::unique_ptr<drivers::MainDriver> main_driver;

  public:
	Game(std::unique_ptr<drivers::MainDriver> main_driver);

	/**
	 * Static helper method to generate a random string
	 * Used to assign random names to SHM instances
	 *
	 * @param length Desired length of the string
	 * @return std::string Result random string
	 */
	static std::string
	GenerateRandomString(const std::string::size_type length);

	/**
	 * Creates and starts the main driver
	 *
	 * @return GameResult object with winner, win type, and player results
	 */
	const drivers::GameResult Start();
};
