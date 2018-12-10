/**
 * @file i_command_giver.h
 * Interface for class that calls command taker methods
 */

#pragma once

#include "state/interfaces/i_command_taker.h"
#include "state/player_state.h"
#include "state/state_export.h"
#include "state/utilities.h"

namespace state {

/**
 * ICommandGiver interface to call command taker methods
 */
class STATE_EXPORT ICommandGiver {
    private:
        ICommandTaker *state;

    virtual void
    AttackActor(ICommandTaker * state, PlayerId player_id,
            ActorId unit_id, ActorId enemy_actor_id) = 0;

    virtual void
    CreateFactory(ICommandTaker * state, PlayerId player_id,
            ActorId villager_id, physics::Vector<int64_t> offset) = 0;

    virtual void
    BuildFactory(ICommandTaker * state, PlayerId player_id,
            ActorId villager_id, ActorId factory_id) = 0;

    virtual void
    MoveUnit(ICommandTaker * state, PlayerId player_id, ActorId actor_id,
                  physics::Vector<int64_t> position) = 0;

  public:

	virtual ~ICommandGiver() {}

    virtual void
	RunCommands(ICommandTaker *state, const std::array<player_state::State, 2> &player_states) = 0;

};

} // namespace state

