/**
 * @file i_state_syncer.h
 * Declares the StateSyncer class
 */

#pragma once

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
	ICommandGiver *command_giver;

	/**
	 * State instance, to give commands to
	 */
	ICommandTaker *state;


	/**
	 * Flips map orientation to facilitate easily modification of player 2's state
	 */
	void FlipMap(std::array<std::array<TerrainType, MAP_SIZE>, MAP_SIZE> &player_map);

	/**
	 * Flips an induvidual position to know it's equivalent position that player 2 will have
	 */
	physics::Vector<int64_t> FlipPosition(const Map *map, physics::Vector<int64_t> position);

	/**
	 * Assiging the soldiers' attribues to default values 
	 */
	void AssignVillagerAttributes(int64_t id, std::vector<player_state::Villager> &player_villagers, bool is_enemy);

	/**
	 * Assiging the villagers' attribues to default values 
	 */
	void AssignSoldierAttributes(int64_t id, std::vector<player_state::Soldier> &player_soldiers, bool is_enemy);

	/**
	 * Assiging the factories' attribues to default values 
	 */
	void AssignFactoryAttributes(int64_t id, std::vector<player_state::Factory> &player_factories, bool is_enemy);
  
  public:
	StateSyncer(ICommandGiver *command_giver, ICommandTaker *state);

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

