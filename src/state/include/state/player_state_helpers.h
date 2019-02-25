/**
 * @file player_state_helpers.h
 * Various helpers to aid the player on their quest
 */

#pragma once

#include "state/player_state.h"

inline Vec2D OffsetToPosition(Vec2D offset) {
	return Vec2D{
	    static_cast<int64_t>(static_cast<size_t>(offset.x) * ELEMENT_SIZE +
	                         (ELEMENT_SIZE / 2)),
	    static_cast<int64_t>(static_cast<size_t>(offset.y) * ELEMENT_SIZE +
	                         (ELEMENT_SIZE / 2))};
}

inline Vec2D PositionToOffset(Vec2D position) {
	return position / ELEMENT_SIZE;
}
