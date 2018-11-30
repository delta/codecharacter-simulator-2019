/**
 * @file state.h
 * Declarations for State which representing the internal state of the game
 */

#pragma once

#include "constants/state.h"
#include "physics/vector.hpp"
#include "state/actor/factory.h"
#include "state/actor/soldier.h"
#include "state/actor/villager.h"
#include "state/gold_manager/gold_manager.h"
#include "state/interfaces/i_command_taker.h"
#include "state/interfaces/i_updatable.h"
#include "state/map/map.h"
#include "state/utilities.h"

#include <memory>

namespace state {

class STATE_EXPORT State {
  private:
	/**
	 * Map instance that maintains game terrain
	 */
	std::unique_ptr<Map> map;

	/**
	 * Gold Manager instance to maintain player gold
	 */
	std::unique_ptr<GoldManager> gold_manager;

	/**
	 * List of soldiers, indexed by player
	 */
	std::array<std::vector<std::unique_ptr<Soldier>>, 2> soldiers;

	/**
	 * List of villagers, indexed by player
	 */
	std::array<std::vector<std::unique_ptr<Villager>>, 2> villagers;

	/**
	 * List of factories, indexed by player
	 */
	std::array<std::vector<std::unique_ptr<Factory>>, 2> factories;

  public:
	/**
	 * Constructor
	 */
	State(std::unique_ptr<Map> map, std::unique_ptr<GoldManager> gold_manager,
	      std::array<std::vector<std::unique_ptr<Soldier>>, 2> soldiers,
	      std::array<std::vector<std::unique_ptr<Villager>>, 2> villagers,
	      std::array<std::vector<std::unique_ptr<Factory>>, 2> factories);

};
} // namespace state
