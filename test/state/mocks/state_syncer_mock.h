#pragma once

#include "gmock/gmock.h"
#include "state/interfaces/i_state_syncer.h"

class StateSyncerMock : public state::IStateSyncer {
  public:
	MOCK_METHOD1(UpdateMainState,
	             void(const std::array<player_state::State, 2> &player_states));

	MOCK_METHOD1(UpdatePlayerStates,
	             void(std::array<player_state::State, 2> &player_states));

	MOCK_METHOD1(IsGameOver, bool(PlayerId &winner));

	MOCK_METHOD0(GetScores, std::array<int64_t, 2>());

	MOCK_METHOD0(GetInterestingness, int64_t());
};
