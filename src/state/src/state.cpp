/**
 * @file state.cpp
 * Definitions for functions of the State class
 */

#include "state/state.h"

namespace state {

State::State(std::unique_ptr<Map> map) : map(std::move(map)) {}

} // namespace state
