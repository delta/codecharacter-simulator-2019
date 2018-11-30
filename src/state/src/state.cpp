/**
 * @file state.cpp
 * Definitions for functions of the State class
 */

#include "state/state.h"

namespace state {

State::State(std::unique_ptr<Map> map,
             std::unique_ptr<GoldManager> gold_manager)
    : map(std::move(map)), gold_manager(std::move(gold_manager)) {}

} // namespace state
