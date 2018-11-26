/**
 * @file i_command_taker.h
 * Interface for game state changing calls
 */

#ifndef STATE_INTERFACES_I_COMMAND_TAKER
#define STATE_INTERFACES_I_COMMAND_TAKER

#include "physics/vector.hpp"
#include "state/interfaces/i_updatable.h"
#include "state/state_export.h"
#include "state/utilities.h"

namespace state {

/**
 * ICommandTaker interface to define the game action methods
 */
class STATE_EXPORT ICommandTaker : public IUpdatable {
  public:
	virtual ~ICommandTaker() {}

	/**
	 * Handles soldier movement
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  soldier_id    soldier to act upon
	 * @param[in]  position      new position to move the soldier to
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	virtual void MoveSoldier(PlayerId player_id, int64_t soldier_id,
	                         physics::Vector<int64_t> position) = 0;

	/**
	 * Handles villager movement
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  villager_id   villager to act upon
	 * @param[in]  position      new position to move the soldier to
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	virtual void MoveVillager(PlayerId player_id, int64_t villager_id,
	                          const physics::Vector<int64_t> &destination) = 0;

	/**
	 * Handles attack on actor
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  actor_id      actor to act upon
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	virtual void AttackActor(PlayerId player_id, int64_t actor_id) = 0;

	/**
	 * Handles factory build
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  actor_id      actor to act upon
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	virtual void BuildFactory(PlayerId player_id, int64_t factory_id) = 0;

	// TODO: Interface the State class getters to get game state info
	//		 i.e. GetSoldiers, GetVillagers, GetMap, etc.
};
} // namespace state

#endif
