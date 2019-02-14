/**
 * @file constants.h
 * Constants which the simulator needs, but the players don't
 */

#pragma once

// Number of instructions exceeding which a turn is cancelled
const int64_t PLAYER_INSTRUCTION_LIMIT_TURN = 10000000;

// Number of instructions exceeding which the entire game is cancelled
const int64_t PLAYER_INSTRUCTION_LIMIT_GAME = 3 * PLAYER_INSTRUCTION_LIMIT_TURN;

// Number of turns in the game
const int64_t NUM_TURNS = 1000;

// Duration of the game in milliseconds
const int64_t GAME_DURATION_MS = 50 * 1000;

// File names for passing SHM names to player processes
const auto SHM_FILE_NAMES = std::array<std::string, 2>{"shm1.txt", "shm2.txt"};

// File where the output game binary log will be stored
const auto GAME_LOG_FILE_NAME = "game.log";

// Interestingness threshold
const int64_t INTEREST_THRESHOLD = 0;
