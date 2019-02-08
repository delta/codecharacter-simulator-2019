/**
 * @file command_giver.cpp
 * Defines the CommandGiver class
 */

#include "state/command_giver.h"
#include "constants/gold_manager.h"
#include "state/state.h"
#include <algorithm>

namespace state {

CommandGiver::CommandGiver() {}

CommandGiver::CommandGiver(ICommandTaker *state, logger::ILogger *logger)
    : state(state), logger(logger) {}

bool CommandGiver::IsValidPosition(Vec2D position) const {
	auto map = state->GetMap();
	auto map_size = map->GetSize();
	auto element_size = map->GetElementSize();

	if (position.x >= map_size * element_size || position.x < 0 ||
	    position.y >= map_size * element_size || position.y < 0) {
		return false;
	}
	return true;
}

bool CommandGiver::IsValidOffset(Vec2D position) const {
	auto map = state->GetMap();
	auto map_size = map->GetSize();

	// Bounds check
	if ((position.x >= map_size || position.x < 0) &&
	    (position.y >= map_size || position.y < 0)) {
		return false;
	}
	return true;
}

bool CommandGiver::IsDeadTarget(int64_t player_id, int64_t enemy_actor_id,
                                ActorType &enemy_type) {
	int64_t enemy_id =
	    (player_id + 1) % static_cast<int>(PlayerId::PLAYER_COUNT);
	auto actor =
	    state->FindActorById(static_cast<PlayerId>(enemy_id), enemy_actor_id);
	bool is_dead_target = (actor->GetHp() == 0);
	if (is_dead_target) {
		enemy_type = actor->GetActorType();
	}
	return is_dead_target;
}

bool CommandGiver::IsValidTarget(int64_t player_id, int64_t enemy_actor_id,
                                 ActorType &target_type, bool &found) {
	// Checking a player's own actors to see if the target belongs to his own
	// actors
	int64_t enemy_id =
	    (player_id + 1) % static_cast<int>(PlayerId::PLAYER_COUNT);
	auto target = this->state->FindActorById(static_cast<PlayerId>(enemy_id),
	                                         enemy_actor_id);

	if (target != nullptr) {
		return true;
	} else {
		auto check_self_target = state->FindActorById(
		    static_cast<PlayerId>(player_id), enemy_actor_id);
		if (check_self_target != nullptr) {
			target_type = check_self_target->GetActorType();
			found = true;
		}
		return false;
	}
}

bool CommandGiver::IsOccupied(
    Vec2D offset, std::array<std::vector<Factory *>, 2> state_factories) {
	// Getting the position from the offset
	auto position = DoubleVec2D{};
	auto element_size = this->state->GetMap()->GetElementSize();
	position.x = offset.x * element_size + (element_size / 2);
	position.y = offset.y * element_size + (element_size / 2);

	// Looking through the current state factories and checking if the position
	// is already occupied by another factory
	for (int64_t player_id = 0; player_id < 2; ++player_id) {
		for (auto factory : state_factories[player_id]) {
			if (factory->GetPosition() == position) {
				return true;
			}
		}
	}
	return false;
}

void CommandGiver::AttackActor(PlayerId player_id, ActorId unit_id,
                               ActorId enemy_actor_id) {
	state->AttackActor(player_id, unit_id, enemy_actor_id);
}

void CommandGiver::CreateFactory(PlayerId player_id, ActorId villager_id,
                                 Vec2D offset) {
	state->CreateFactory(player_id, villager_id, offset);
}

void CommandGiver::BuildFactory(PlayerId player_id, ActorId villager_id,
                                ActorId factory_id) {
	state->BuildFactory(player_id, villager_id, factory_id);
}

void CommandGiver::MoveUnit(PlayerId player_id, ActorId actor_id,
                            Vec2D position) {
	state->MoveUnit(player_id, actor_id, position);
}

void CommandGiver::SetFactoryProduction(PlayerId player_id, ActorId factory_id,
                                        ActorType actor_type) {
	state->SetFactoryProduction(player_id, factory_id, actor_type);
}

void CommandGiver::StopOrStartFactory(PlayerId player_id, ActorId factory_id,
                                      bool should_stop) {
	state->StopOrStartFactory(player_id, factory_id, should_stop);
}

void CommandGiver::MineLocation(PlayerId player_id, ActorId villager_id,
                                Vec2D mine_location) {
	state->MineLocation(player_id, villager_id, mine_location);
}

void CommandGiver::RunCommands(
    const std::array<player_state::State, 2> &player_states) {

	// Getting all the soldiers, factory and villagers
	auto state_soldiers = state->GetSoldiers();
	auto state_villagers = state->GetVillagers();
	auto state_factories = state->GetFactories();
	auto state_map = state->GetMap();
	auto state_gold = state->GetGold();

	// Iterating through the soldiers and assigning soldier tasks

	// For each player...
	for (int player_id = 0; player_id < player_states.size(); ++player_id) {

		// For each soldier...
		for (int64_t soldier_index = 0;
		     soldier_index < player_states[player_id].soldiers.size();
		     ++soldier_index) {

			// Get the player soldier and the 'true' soldier from state
			auto soldier = player_states[player_id].soldiers[soldier_index];
			auto state_soldier = state_soldiers[player_id][soldier_index];

			PlayerId Player_Id = static_cast<PlayerId>(player_id);

			// Check what the soldier wants to perform
			bool is_attacking = soldier.target != -1;
			bool is_moving = (soldier.destination != Vec2D(-1, -1));

			// Validating the the soldier id
			if (soldier.id != state_soldier->GetActorId()) {
				logger->LogError(Player_Id,
				                 logger::ErrorType::NO_ALTER_ACTOR_ID,
				                 "Cannot alter soldier id");
				continue;
			}

			// Check if soldier is alive
			else if (state_soldier->GetHp() == 0) {
				logger->LogError(Player_Id,
				                 logger::ErrorType::NO_ACTION_BY_DEAD_SOLDIER,
				                 "Dead soldier cannot perform actions");
				continue;
			}

			// Checking if the soldier is attacking and moving at the same time
			else if (is_attacking && is_moving) {
				logger->LogError(
				    Player_Id, logger::ErrorType::NO_MULTIPLE_SOLDIER_TASKS,
				    "Soldier cannot attack and move at the same time");
			} else {
				if (is_attacking) {
					// Checking if the target is an enemy
					ActorType self_target_type;
					bool found = false;
					bool valid_target = IsValidTarget(player_id, soldier.target,
					                                  self_target_type, found);
					if (valid_target == false) {
						if (found) {
							switch (self_target_type) {
							case ActorType::SOLDIER:
								logger->LogError(
								    Player_Id,
								    logger::ErrorType::NO_ATTACK_SELF_SOLDIER,
								    "Soldier is attacking his own soldier");
								break;
							case ActorType::VILLAGER:
								logger->LogError(
								    Player_Id,
								    logger::ErrorType::NO_ATTACK_SELF_VILLAGER,
								    "Soldier is attacking his own villager");
								break;
							case ActorType::FACTORY:

								logger->LogError(
								    Player_Id,
								    logger::ErrorType::NO_ATTACK_SELF_FACTORY,
								    "Soldier is attacking his own soldier");
								break;
							}
						} else {
							logger->LogError(
							    Player_Id, logger::ErrorType::INVALID_TARGET_ID,
							    "Invalid target id");
						}

						continue;
					}

					// If the target is valid, then checking if the enemy has HP
					// or the target is dead
					ActorType enemy_type;
					bool is_dead =
					    IsDeadTarget(player_id, soldier.target, enemy_type);
					if (is_dead) {
						switch (enemy_type) {
						case ActorType::SOLDIER:
							logger->LogError(
							    Player_Id,
							    logger::ErrorType::NO_ATTACK_DEAD_SOLDIER,
							    "Soldier attacking a dead soldier");
							break;
						case ActorType::VILLAGER:
							logger->LogError(
							    Player_Id,
							    logger::ErrorType::NO_ATTACK_DEAD_VILLAGER,
							    "Soldier is attacking a dead villager");
							break;
						case ActorType::FACTORY:
							logger->LogError(
							    Player_Id,
							    logger::ErrorType::NO_ATTACK_RAZED_FACTORY,
							    "Soldier is attacking a dead factory");
							break;
						}
					}
					AttackActor(Player_Id, soldier.id, soldier.target);
				}

				else if (is_moving) {
					// Checking if the destination is valid
					bool is_valid_destination =
					    IsValidPosition(soldier.destination);

					if (is_valid_destination) {
						MoveUnit(Player_Id, soldier.id, soldier.destination);
					} else {
						logger->LogError(
						    Player_Id, logger::ErrorType::INVALID_MOVE_POSITION,
						    "Soldier trying to move to invalid location");
					}
				}
			}
		}
	}

	// Iterating through the villagers and assigning tasks

	// For each player...
	for (int player_id = 0; player_id < player_states.size(); ++player_id) {

		// For each villager...
		for (int64_t villager_index = 0;
		     villager_index < player_states[player_id].villagers.size();
		     ++villager_index) {

			// Get the player villager, and the 'true' villager from state
			auto villager = player_states[player_id].villagers[villager_index];
			auto state_villager = state_villagers[player_id][villager_index];

			int64_t factory_target = villager.target_factory_id;
			PlayerId Player_id = static_cast<PlayerId>(player_id);

			// Check what the villager wants to perform
			bool should_create_factory = villager.build_position != Vec2D::null;
			bool should_build_factory = villager.target_factory_id != -1;
			bool should_move = villager.destination != Vec2D::null;
			bool should_attack = villager.target != -1;
			bool should_mine = villager.mine_target != Vec2D::null;

			// Check if the villager's id is valid
			if (villager.id != state_villager->GetActorId()) {
				logger->LogError(Player_id,
				                 logger::ErrorType::NO_ALTER_ACTOR_ID,
				                 "Cannot alter villager id");
				continue;
			}

			// Checking if the villager is alive
			else if (state_villager->GetHp() == 0) {
				logger->LogError(Player_id,
				                 logger::ErrorType::NO_ACTION_BY_DEAD_VILLAGER,
				                 "Dead villager cannot act");
				continue;
			}

			// Check to make sure only one action is being attempted
			std::vector<bool> checks{should_create_factory,
			                         should_build_factory, should_move,
			                         should_attack, should_mine};
			if (std::count(checks.begin(), checks.end(), true) > 1) {
				logger->LogError(
				    Player_id, logger::ErrorType::NO_MULTIPLE_VILLAGER_TASKS,
				    "Villager cannot do multiple tasks at the same time");
				continue;
			}

			else if (should_create_factory) {
				// If villager is creating a new factory
				bool is_valid = IsValidPosition(villager.build_position);
				if (is_valid) {
					if (state_gold[player_id] >= FACTORY_COST) {
						TerrainType terrain_type =
						    state_map->GetTerrainTypeByPosition(
						        villager.build_position.x,
						        villager.build_position.y);
						switch (terrain_type) {
						case TerrainType::LAND:
							if (state_factories[player_id].size() <
							    MAX_NUM_FACTORIES) {
								bool is_occupied = IsOccupied(
								    villager.build_position, state_factories);
								if (is_occupied) {
									logger->LogError(
									    Player_id,
									    logger::ErrorType::POSITION_OCCUPIED,
									    "Villager is trying to build a factory "
									    "in a position that is already "
									    "occupied");
								} else {
									CreateFactory(Player_id, villager.id,
									              villager.build_position);
								}

							} else {
								logger->LogError(
								    Player_id,
								    logger::ErrorType::NO_MORE_FACTORIES,
								    "Trying to build more factories that the "
								    "factory limit");
							}
							break;
						case TerrainType::WATER:
							logger->LogError(
							    Player_id,
							    logger::ErrorType::NO_BUILD_FACTORY_ON_WATER,
							    "Villager trying to build factory on water");
							break;
						case TerrainType::GOLD_MINE:
							logger->LogError(
							    Player_id,
							    logger::ErrorType::
							        NO_BUILD_FACTORY_ON_GOLD_MINE,
							    "Villager trying to build factory ");
							break;
						}
					} else {
						logger->LogError(Player_id,
						                 logger::ErrorType::INSUFFICIENT_FUNDS,
						                 "You do not have sufficient funds to "
						                 "purchase a factory");
					}
				} else {
					logger->LogError(Player_id,
					                 logger::ErrorType::INVALID_BUILD_POSITION,
					                 "Villager cannot build factory in "
					                 "invalid position");
				}
			}

			// If villager is building an existing factory
			else if (should_build_factory) {
				// Validating whether factory exists
				bool factory_exists = false;
				for (auto &factory : state_factories[player_id]) {
					if (factory->GetActorId() == factory_target)
						factory_exists = true;
					break;
				}
				if (factory_exists) {
					BuildFactory(Player_id, villager.id,
					             villager.target_factory_id);
				} else {
					logger->LogError(
					    Player_id,
					    logger::ErrorType::NO_BUILD_FACTORY_THAT_DOSENT_EXIST,
					    "Villager trying to build factory that dosen't exist");
				}
			}

			// If the villager is moving to another destination
			else if (should_move) {
				// Validating the move position
				bool is_valid = IsValidPosition(villager.destination);
				if (is_valid) {
					MoveUnit(Player_id, villager.id, villager.destination);
				} else {
					logger->LogError(
					    Player_id, logger::ErrorType::INVALID_MOVE_POSITION,
					    "Villager cannot move to invalid position");
				}
			}

			// If the villager is attacking another actor
			else if (should_attack) {
				ActorType self_target_type;
				bool found = false;
				bool valid_target = IsValidTarget(player_id, villager.target,
				                                  self_target_type, found);
				if (valid_target == false) {
					if (found) {
						switch (self_target_type) {
						case ActorType::SOLDIER:
							logger->LogError(
							    Player_id,
							    logger::ErrorType::NO_ATTACK_SELF_SOLDIER,
							    "Villager is attacking his own soldier");
							break;
						case ActorType::VILLAGER:
							logger->LogError(
							    Player_id,
							    logger::ErrorType::NO_ATTACK_SELF_VILLAGER,
							    "Villager is attacking his own villager");
							break;
						case ActorType::FACTORY:
							logger->LogError(
							    Player_id,
							    logger::ErrorType::NO_ATTACK_SELF_FACTORY,
							    "Villager is attacking his own soldier");
							break;
						}
					} else {
						logger->LogError(Player_id,
						                 logger::ErrorType::INVALID_TARGET_ID,
						                 "Invalid target id");
					}
				}

				// If the target is valid, then checking if the enemy has HP or
				// the target is dead
				ActorType enemy_type;
				bool is_dead =
				    IsDeadTarget(player_id, villager.target, enemy_type);
				if (is_dead == true) {
					switch (enemy_type) {
					case ActorType::SOLDIER:
						logger->LogError(
						    Player_id,
						    logger::ErrorType::NO_ATTACK_DEAD_SOLDIER,
						    "Villager attacking a dead soldier");
						break;
					case ActorType::VILLAGER:
						logger->LogError(
						    Player_id,
						    logger::ErrorType::NO_ATTACK_DEAD_VILLAGER,
						    "Villager is attacking a dead villager");
						break;
					case ActorType::FACTORY:
						logger->LogError(
						    Player_id,
						    logger::ErrorType::NO_ATTACK_RAZED_FACTORY,
						    "Villager is attacking a dead factory");
						break;
					}
				}
				AttackActor(Player_id, villager.id, villager.target);
			}

			// If the villager is wants to mine gold
			else if (should_mine) {
				// Validating the mine location
				bool is_valid = IsValidOffset(villager.mine_target);
				if (is_valid) {
					bool is_gold_mine =
					    (state_map->GetTerrainTypeByOffset(
					         villager.mine_target.x, villager.mine_target.y) ==
					     TerrainType::GOLD_MINE);
					if (is_gold_mine) {
						MineLocation(Player_id, villager.id,
						             villager.mine_target);
					} else {
						logger->LogError(
						    Player_id, logger::ErrorType::INVALID_MINE_POSITION,
						    "Villager cannot mine in invalid position");
					}
				} else {
					logger->LogError(
					    Player_id, logger::ErrorType::INVALID_MINE_POSITION,
					    "Villager cannot mine in invalid position");
				}
			}
		}
	}

	// Iterating through all the factories

	// For each player...
	for (int player_id = 0; player_id < player_states.size(); ++player_id) {

		// For each factory...
		for (int64_t factory_index = 0;
		     factory_index < player_states[player_id].factories.size();
		     ++factory_index) {
			auto factory = player_states[player_id].factories[factory_index];
			PlayerId Player_id = static_cast<PlayerId>(player_id);

			// Validating the factory id
			if (factory.id !=
			    state_factories[player_id][factory_index]->GetActorId()) {
				logger->LogError(Player_id,
				                 logger::ErrorType::NO_ALTER_ACTOR_ID,
				                 "Cannot alter factory id");
				continue;
			}

			// Validating factory hp
			else if (state_factories[player_id][factory_index]->GetHp() == 0) {
				logger->LogError(Player_id,
				                 logger::ErrorType::NO_ACTION_BY_DEAD_FACTORY,
				                 "Dead factory cannot act");
				continue;
			}

			// Starting or stopping factory production
			StopOrStartFactory(Player_id, factory.id, factory.stopped);

			// Changing the production state of the factory if changed, else it
			// stays the same
			auto build_factory_type = factory.production_state;
			if (build_factory_type ==
			    player_state::FactoryProduction::VILLAGER) {
				SetFactoryProduction(Player_id, factory.id,
				                     ActorType::VILLAGER);
			} else if (build_factory_type ==
			           player_state::FactoryProduction::SOLDIER) {
				SetFactoryProduction(Player_id, factory.id, ActorType::SOLDIER);
			}
		}
	}
}

} // namespace state
