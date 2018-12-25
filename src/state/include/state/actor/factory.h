/**
 * @file factory.h
 * Declares the Factory class
 */

#include "state/actor/actor.h"
#include "state/actor/factory_states/factory_state.h"
#include "state/actor/soldier.h"
#include "state/actor/villager.h"
#include "state/state_export.h"
#include "state/utilities.h"

#include <functional>
#include <vector>

#pragma once

namespace state {

/**
 * Define the type for the function to call, in order to create a new unit
 */
using UnitProductionCallback =
    std::function<void(state::PlayerId, state::ActorType, Vec2D)>;

/**
using UnitProductionCallback =
    std::function<void(state::PlayerId, state::ActorType, Vec2D)>;
 * Factory class represents one game factory
 */
class STATE_EXPORT Factory : public Actor {
  protected:
	/**
	 * Controls logic for factory's current state
	 */
	std::unique_ptr<FactoryState> state;

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
	ActorType production_state;

	/**
	 * Set if the factory is stopped
	 */
	bool stopped;

	/**
	 * Amount of damage the factory incurred in the current turn
	 * Applied to hp at the end of the turn
	 */
	int64_t damage_incurred;

	/**
	 * Number of turns after which a villager is produced
	 */
	int64_t villager_frequency;

	/**
	 * Number of turns after which a soldier is produced
	 */
	int64_t soldier_frequency;

	/**
	 * Callback to call to create a new unit in this factory. Will be passed in
	 * through the state.
	 */
	UnitProductionCallback unit_production_callback;

  public:
	Factory();

	Factory(ActorId id, PlayerId player_id, ActorType actor_type, int64_t hp,
	        int64_t max_hp, physics::Vector<int64_t> position,
	        GoldManager *gold_manager, int64_t construction_complete,
	        int64_t construction_total, ActorType production_state,
	        int64_t villager_frequency, int64_t soldier_frequency,
	        UnitProductionCallback unit_production_callback);

	/**
	 * Creates a clone of this Factory
	 *
	 * @return Factory new_factory
	 */
	Factory Clone();

	/**
	 * Calls the callback with the current parameters, to produce a unit
	 */
	void ProduceUnit();

	/**
	 * Used to set the callback for unit production
	 */
	void SetUnitProductionCallback(UnitProductionCallback callback);

	/**
	 * Put some effort into construction
	 *
	 * @param value Value to Increment
	 */
	void IncrementConstructionCompletion(int64_t value);

	/**
	 * Get the amount of effort completed
	 *
	 * @return int64_t Amount of effort
	 */
	int64_t GetConstructionCompletion();

	/**
	 * Get the total required construction completion amount
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

	/**
	 * Get the current Production state
	 *
	 * @return ProductionState
	 */
	ActorType GetProductionState();

	/**
	 * Returns if the factory is currently stopped
	 *
	 * @return true If stopped
	 * @return false Otherwise
	 */
	bool IsStopped();

	/**
	 * Stops production
	 */
	void Stop();

	/**
	 * Resumes production
	 */
	void Start();

	/**
	 * Set the currrent production
	 *
	 * @param production
	 */
	void SetProductionState(ActorType production_state);

	/**
	 * Returns factory's new_hp
	 *
	 * @see Actor#GetLatestHp
	 *
	 * @return     The factory's new_hp
	 */
	int64_t GetLatestHp() override;

	/**
	 * Get the Villager Frequency
	 *
	 * @return int64_t
	 */
	int64_t GetVillagerFrequency();

	/**
	 * Get the Soldier Frequency
	 *
	 * @return int64_t
	 */
	int64_t GetSoldierFrequency();

	/**
	 * Get the name of the current state
	 *
	 * @return     Name of current state of type FactoryStateName
	 */
	FactoryStateName GetState();

	/**
	 * @see Actor#Damage
	 */
	void Damage(int64_t damage_amount) override;

	/**
	 * Update function of the Factory
	 */
	void LateUpdate();

	/**
	 * Update function of the Factory
	 */
	void Update();
};

} // namespace state
