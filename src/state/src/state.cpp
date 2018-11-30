/**
 * @file state.cpp
 * Definitions for functions of the State class
 */

#include "state/state.h"

namespace state {

State::State(std::unique_ptr<Map> map,
             std::unique_ptr<GoldManager> gold_manager,
             std::array<std::vector<std::unique_ptr<Soldier>>, 2> soldiers,
             std::array<std::vector<std::unique_ptr<Villager>>, 2> villagers,
             std::array<std::vector<std::unique_ptr<Factory>>, 2> factories)
    : map(std::move(map)), gold_manager(std::move(gold_manager)),
      soldiers(std::move(soldiers)), villagers(std::move(villagers)),
      factories(std::move(factories)) {}

} // namespace state
