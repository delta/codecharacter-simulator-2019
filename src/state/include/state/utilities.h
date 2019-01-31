/**
 * @file utilities.h
 * Set of types and utilities functions for state
 */

#pragma once

#include <cstdint>

namespace state {

/**
 * Integer type to represent unique Actor IDs
 */
typedef int64_t ActorId;

/**
 * Enum for the different actor unit types
 */
enum class ActorType { SOLDIER, VILLAGER, FACTORY };

/**
 * Enum for the different types of terrain on the map
 */
enum class TerrainType { LAND, WATER, GOLD_MINE };

/**
 * Enum for the two game players
 */
enum class PlayerId : int {
	PLAYER1 = 0,
	PLAYER2 = 1,
	PLAYER_COUNT = 2,
};
} // namespace state
