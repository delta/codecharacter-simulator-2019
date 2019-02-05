/**
 * @file driver.h
 * Constants related to the overall game
 */

#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <string>

// Number of turns in the game
const int64_t NUM_TURNS = 1000;

// Duration of the game in milliseconds
const int64_t GAME_DURATION_MS = 50 * 1000;

// File names for passing SHM names to player processes
const auto SHM_FILE_NAMES = std::array<std::string, 2>{"shm1.txt", "shm2.txt"};

// File where the output game binary log will be stored
const auto GAME_LOG_FILE_NAME = "game.log";
