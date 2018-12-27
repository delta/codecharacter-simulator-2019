#pragma once

#include "gmock/gmock.h"
#include "state/interfaces/i_command_giver.h"

class CommandGiverMock : public state::ICommandGiver {
  public:
	MOCK_METHOD2(RunCommands,
	             void(ICommandTaker *state,
	                  const std::array<player_state::State, 2> &player_states));
};
