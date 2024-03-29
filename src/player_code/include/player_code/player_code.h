#pragma once

#include "constants/constants.h"
#include "player_code/player_code_export.h"
#include "player_wrapper/interfaces/i_player_code.h"
#include "state/player_state.h"
#include "state/player_state_helpers.h"

#include <iostream>
#include <vector>

namespace player_code {

/**
 * Class where player defines AI code
 */
class PLAYER_CODE_EXPORT PlayerCode : public player_wrapper::IPlayerCode {
	/**
	 * Player AI update function (main logic of the AI)
	 */
	player_state::State Update(player_state::State state) override;
};
} // namespace player_code
