/**
 * @file instruction_limits.h
 * Player instruction limit constants
 */

#pragma once

#include <cstdint>

// Number of instructions exceeding which a turn is cancelled
const int64_t PLAYER_INSTRUCTION_LIMIT_TURN = 10000000;

// Number of instructions exceeding which the entire game is cancelled
const int64_t PLAYER_INSTRUCTION_LIMIT_GAME = 3 * PLAYER_INSTRUCTION_LIMIT_TURN;
