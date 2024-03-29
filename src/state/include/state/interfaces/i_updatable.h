/**
 * @file i_updatable.h
 * Declares the updatable interfacs, which gives classs an update method
 */

#pragma once

#include "state/state_export.h"

namespace state {

/**
 * IUpdatable interface to define the Update method
 */
class STATE_EXPORT IUpdatable {
  public:
	virtual ~IUpdatable(){};

	virtual void Update() = 0;
};
} // namespace state
