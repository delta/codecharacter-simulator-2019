/**
 * @file i_state_syncer.h
 * Declares the StateSyncer class
 */

#pragma once

#include "logger/interfaces/i_logger.h"
#include "state/interfaces/i_command_giver.h"
#include "state/interfaces/i_command_taker.h"
#include "state/interfaces/i_state_syncer.h"
#include "state/utilities.h"

#include <array>

namespace state {

class STATE_EXPORT StateSyncer : public IStateSyncer {

	/**
	 * Command giver instance that makes calls to update game state
	 */
	std::unique_ptr<ICommandGiver> command_giver;

	/**
	 * State instance, to give commands to
	 */
	std::unique_ptr<ICommandTaker> state;

	/**
	 * Pointer to logger instance for logging game state every turn
	 */
	logger::ILogger *logger;

	/**
	 * Flips an induvidual position to know it's equivalent position that player
	 * 2 will have
	 */
	DoubleVec2D FlipPosition(const Map *map, DoubleVec2D position);

	/**
	 * Assiging the soldiers' attribues to default values
	 */
	void AssignVillagerAttributes(
	    int64_t id, std::vector<player_state::Villager> &player_villagers,
	    bool is_enemy);

	/**
	 * Assiging the villagers' attribues to default values
	 */
	void
	AssignSoldierAttributes(int64_t id,
	                        std::vector<player_state::Soldier> &player_soldiers,
	                        bool is_enemy);

	/**
	 * Assiging the factories' attribues to default values
	 */
	void AssignFactoryAttributes(
	    int64_t id, std::vector<player_state::Factory> &player_factories,
	    bool is_enemy);

	/**
	 * Returns the same id if is_enemy is false, else returns the opposite id
	 */
	int64_t GetPlayerId(int id, bool is_enemy);

  public:
	StateSyncer(std::unique_ptr<ICommandGiver> command_giver,
	            std::unique_ptr<ICommandTaker> state, logger::ILogger *logger);

	/**
	 * @see IStateSyncer#UpdateMainState
	 */
	void UpdateMainState(
	    const std::array<player_state::State, 2> &player_states) override;

	/**
	 * @see IStateSyncer#UpdatePlayerStates
	 */
	void UpdatePlayerStates(
	    std::array<player_state::State, 2> &player_states) override;

	/**
	 * @see IStateSyncer#GetScores
	 */
	std::array<int64_t, 2> GetScores() override;
};

} // namespace state
