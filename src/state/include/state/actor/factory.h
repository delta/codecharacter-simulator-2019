/**
 * @file factory.h
 * Declares the Factory class
 */

#include "state/actor/actor.h"
#include "state/state_export.h"
#include "state/utilities.h"

#pragma once

namespace state {

/**
 * Factory class represents one game factory
 */
class STATE_EXPORT Factory : public Actor {
  protected:
	/**
	 * Number of points of effort spent on contruction so far
	 */
	int64_t construction_complete;

	/**
	 * Number of points of effort needed to complete factory
	 */
	int64_t construction_total;

	/**
	 * Type of unit that the factory is currently producing
	 */
	ProductionState production_state;

  public:
	Factory();

	Factory(ActorId id, PlayerId player_id, ActorType actor_type, int64_t hp,
	        int64_t max_hp, physics::Vector<int64_t> position,
	        GoldManager *gold_manager, int64_t construction_complete,
	        int64_t construction_total, ProductionState production_state);

	/**
	 * Put some effort into construction
	 *
	 * @param value Value to Increment
	 */
	void IncrementConstructionCompletion(int64_t value);

	/**
	 * @brief Get the amount of effort completed
	 *
	 * @return int64_t Amount of effort
	 */
	int64_t GetConstructionCompletion();

	/**
	 * @brief Get the total required construction completion amount
	 *
	 * @return int64_t Amount of effort (total)
	 */
	int64_t GetTotalConstructionCompletion();

	/**
	 * Check if construction is complete
	 *
	 * @return true If construction is complete (complete amount = total)
	 * @return false otherwise
	 */
	bool IsConstructionComplete();
};

} // namespace state
