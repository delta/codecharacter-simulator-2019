/**
 * @file main_driver.cpp
 * Contains definitions for the main driver
 */

#include "drivers/main_driver.h"
#include "drivers/game_result.h"

#include <csignal>
#include <fstream>

namespace drivers {

MainDriver::MainDriver(
    std::unique_ptr<state::IStateSyncer> state_syncer,
    std::vector<std::unique_ptr<SharedMemoryMain>> shared_memories,
    int64_t player_instruction_limit_turn,
    int64_t player_instruction_limit_game, int64_t max_no_turns,
    Timer::Interval game_duration, std::unique_ptr<logger::ILogger> logger,
    std::string log_file_name)
    : state_syncer(std::move(state_syncer)),
      shared_memories(std::move(shared_memories)),
      player_instruction_limit_turn(player_instruction_limit_turn),
      player_instruction_limit_game(player_instruction_limit_game),
      max_no_turns(max_no_turns), is_game_timed_out(false), game_timer(),
      game_duration(game_duration), logger(std::move(logger)),
      log_file_name(log_file_name), cancel(false) {
	for (auto &shared_memory : this->shared_memories) {
		// Get pointers to shared memory and store
		SharedBuffer *shared_buffer = shared_memory->GetBuffer();
		shared_buffers.push_back(shared_buffer);
	}

	// Store pointers to player state
	this->transfer_states[0] = &shared_buffers[0]->transfer_state;
	this->transfer_states[1] = &shared_buffers[1]->transfer_state;
}

void MainDriver::/*Avengers:*/ EndGame(state::PlayerId player_id,
                                       bool was_deathmatch,
                                       std::array<int64_t, 2> final_scores) {
	std::ofstream log_file(log_file_name, std::ios::out | std::ios::binary);

	logger->LogFinalGameParams(player_id, was_deathmatch, final_scores);
	logger->WriteGame(log_file);
	this->game_timer.Cancel();
}

std::array<PlayerResult, 2> MainDriver::GetPlayerResults() {
	// Get the scores, with the game_over parameter set to true
	auto player_scores = this->state_syncer->GetScores(true);
	auto player_results = std::array<PlayerResult, 2>{};

	// Write the player results
	for (int i = 0; i < 2; ++i) {
		player_results[i] =
		    PlayerResult{player_scores[i], PlayerResult::Status::NORMAL};
	}

	return player_results;
}

void MainDriver::SetPids(std::array<int, 2> pids) { this->process_pids = pids; }

const GameResult MainDriver::Start() {
	// Initialize contents of shared memory
	for (auto buffer : shared_buffers) {
		buffer->is_player_running = false;
		buffer->is_game_complete = false;
		buffer->instruction_counter = 0;
	}

	// Initialize player states with contents of main state
	this->state_syncer->UpdatePlayerStates(player_states);

	// Convert current player states to transfer states
	for (int i = 0; i < 2; ++i) {
		*(this->transfer_states[i]) =
		    transfer_state::ConvertToTransferState(this->player_states[i]);
	}

	// Start a timer. Game is invalid if it does not complete within the timer
	// limit
	this->is_game_timed_out = false;
	this->game_timer.Start(this->game_duration,
	                       [this]() { this->is_game_timed_out = true; });

	// Run the game and return results
	return this->Run();
}

GameResult::Winner
GetWinnerByInstCountExceeded(std::array<PlayerResult, 2> player_results) {
	auto status1 = player_results[0].status;
	auto status2 = player_results[1].status;
	auto FAIL = PlayerResult::Status::EXCEEDED_INSTRUCTION_LIMIT;

	if (status1 == FAIL && status2 == FAIL) {
		return GameResult::Winner::TIE;
	} else if (status1 == FAIL) {
		return GameResult::Winner::PLAYER2;
	}
	return GameResult::Winner::PLAYER1;
}

GameResult::Winner GetWinnerFromPlayerId(state::PlayerId player_id) {
	switch (player_id) {
	case state::PlayerId::PLAYER1:
		return GameResult::Winner::PLAYER1;
	case state::PlayerId::PLAYER2:
		return GameResult::Winner::PLAYER2;
	case state::PlayerId::PLAYER_NULL:
		return GameResult::Winner::TIE;
	}
}

GameResult::Winner
GetWinnerByScore(std::array<PlayerResult, 2> player_results) {
	auto score1 = player_results[0].score;
	auto score2 = player_results[1].score;

	if (score1 > score2) {
		return GameResult::Winner::PLAYER1;
	} else if (score1 < score2) {
		return GameResult::Winner::PLAYER2;
	}
	return GameResult::Winner::TIE;
}

state::PlayerId GetPlayerIdFromWinner(GameResult::Winner winner) {
	switch (winner) {
	case GameResult::Winner::PLAYER1:
		return state::PlayerId::PLAYER1;
	case GameResult::Winner::PLAYER2:
		return state::PlayerId::PLAYER2;
	case GameResult::Winner::TIE:
		return state::PlayerId::PLAYER_NULL;
	}
}

const GameResult MainDriver::Run() {
	// Initializing stuff...
	auto skip_player_turn = std::array<bool, 2>{false, false};
	auto player_results = std::array<PlayerResult, 2>{
	    PlayerResult{0, PlayerResult::Status::UNDEFINED},
	    PlayerResult{0, PlayerResult::Status::UNDEFINED}};
	auto winner = GameResult::Winner::NONE;
	auto win_type = GameResult::WinType::NONE;
	auto player_scores = std::array<int64_t, 2>{0, 0};
	auto instruction_count_exceeded = false;

	// Main loop that runs every turn
	for (int i = 0; i < this->max_no_turns; ++i) {
		// Loop over each player
		for (int cur_player_id = 0; cur_player_id < 2; ++cur_player_id) {
			auto current_player_buffer = this->shared_buffers[cur_player_id];

			// Let player do their updates
			current_player_buffer->is_player_running = true;

			// Wait for updates, the timer or cancellation
			while (current_player_buffer->is_player_running &&
			       !this->is_game_timed_out && !this->cancel)
				;

			// If game has been cancelled, return immediately
			if (this->cancel) {
				this->cancel = false;
				EndGame();
				return GameResult{winner, win_type, 0, player_results};
			}

			// If the game timed out
			if (this->is_game_timed_out) {

				// THIS IS AN UGLY HACK! PLEASE CHANGE THIS. TODO.
				auto success = kill(process_pids[cur_player_id], SIGTERM);
				return GameResult{};
			}

			// Check for instruction counter to see if player has
			// exceeded some limit
			if (current_player_buffer->instruction_counter >
			    this->player_instruction_limit_game) {
				player_results[cur_player_id].status =
				    PlayerResult::Status::EXCEEDED_INSTRUCTION_LIMIT;
				instruction_count_exceeded = true;
			} else if (current_player_buffer->instruction_counter >
			           this->player_instruction_limit_turn) {
				skip_player_turn[cur_player_id] = true;
			} else {
				skip_player_turn[cur_player_id] = false;
			}

			// Write the turn's instruction counts
			logger->LogInstructionCount(
			    static_cast<state::PlayerId>(cur_player_id),
			    current_player_buffer->instruction_counter);
		}

		// If the game instruction count has been exceeded by some
		// player, game is forfeit
		if (instruction_count_exceeded) {
			EndGame();
			winner = GetWinnerByInstCountExceeded(player_results);
			win_type = GameResult::WinType::EXCEEDED_INSTRUCTION_LIMIT;
			return GameResult{winner, win_type, 0, player_results};
		}

		// If the game timer has expired, the game has to stop
		if (this->is_game_timed_out) {
			EndGame();
			return GameResult{winner, win_type, 0, player_results};
		}

		// If we're here, the game is not yet over

		// Validate and run the player's commands. Skips a player if
		// they have exceeded turn instruction limit

		// Convert current transfer states into player states
		for (int i = 0; i < 2; ++i) {
			player_states[i] = transfer_state::ConvertToPlayerState(
			    *(this->transfer_states[i]));
		}
		this->state_syncer->UpdateMainState(this->player_states,
		                                    skip_player_turn);

		// Write the updated main state back to the player's state
		// copies
		this->state_syncer->UpdatePlayerStates(this->player_states);

		// Convert these player states back into transfer states
		for (int i = 0; i < 2; ++i) {
			*(transfer_states[i]) =
			    transfer_state::ConvertToTransferState(player_states[i]);
		}

		// If the game is over now, some player had all units killed
		// End the game as a deathmatch
		state::PlayerId player_winner;
		if (this->state_syncer->IsGameOver(player_winner)) {
			// Cancel player drivers as a game over by deathmatch
			for (auto buffer : shared_buffers) {
				buffer->is_game_complete = true;
			}

			player_results = GetPlayerResults();
			EndGame(player_winner, true,
			        {player_results[0].score, player_results[1].score});
			winner = GetWinnerFromPlayerId(player_winner);
			win_type = GameResult::WinType::DEATHMATCH;
			auto interest = this->state_syncer->GetInterestingness();
			return GameResult{winner, win_type, interest, player_results};
		}
	}

	// Done with the game now

	// Write scores and complete game
	player_results = GetPlayerResults();

	// Set result parameters
	auto interest = this->state_syncer->GetInterestingness();
	winner = GetWinnerByScore(player_results);
	win_type = GameResult::WinType::SCORE;

	// Log the winner
	auto winner_player_id = GetPlayerIdFromWinner(winner);
	EndGame(winner_player_id, false,
	        {player_results[0].score, player_results[1].score});

	return GameResult{winner, win_type, interest, player_results};
}

void MainDriver::Cancel() {
	this->cancel = true;
	auto wait_time = std::chrono::seconds(1);
	auto end_time = std::chrono::system_clock::now() + wait_time;
	while (std::chrono::system_clock::now() < end_time)
		;
}
} // namespace drivers
