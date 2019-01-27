/**
 * file error_type.h
 * Defines a type to represent a particular category of errors
 */

#pragma once

#include <string>
#include <vector>

namespace logger {

/**
 * Enum representing different categories of errors. Errors from the same
 * category can potentially be grouped
 *
 * ALERT! - When adding a value here, add it in the string mapping below as well
 */
enum class ErrorType {
	/**
	 * Trying to perform multiple soldier commands
	 */
	NO_MULTIPLE_SOLDIER_TASKS,

	/**
	 * Trying to perform multiple villager commands
	 */
	NO_MULTIPLE_VILLAGER_TASKS,

	/**
	 * Trying to perform multiple factory commands
	 */
	NO_MULTIPLE_FACTORY_TASKS,

	/**
	 * Trying to change the actor_id property
	 */
	NO_ALTER_ACTOR_ID,

	/**
	 * Trying to command a dead soldier
	 */
	NO_ACTION_BY_DEAD_SOLDIER,

	/**
	 * Trying to command a dead villager
	 */
	NO_ACTION_BY_DEAD_VILLAGER,

	/**
	 * Trying to command a dead factory
	 */
	NO_ACTION_BY_DEAD_FACTORY,

	/**
	 * Trying to move a soldier / villager to an invalid position
	 */
	INVALID_MOVE_POSITION,

	/**
	 * Trying to make a villager mine at an invalid location
	 */
	INVALID_MINE_POSITION,

	/**
	 * Trying to make a villager build factory at an invalid location
	 */
	INVALID_BUILD_POSITION,

	/**
	 * Trying to attack one's own soldier
	 */
	NO_ATTACK_SELF_SOLDIER,

	/**
	 * Trying to attack one's own villager
	 */
	NO_ATTACK_SELF_VILLAGER,

	/**
	 * Trying to attack one's own factory
	 */
	NO_ATTACK_SELF_FACTORY,

	/**
	 * Trying to attack a dead soldier
	 */
	NO_ATTACK_DEAD_SOLDIER,

	/**
	 * Trying to attack a dead villager
	 */
	NO_ATTACK_DEAD_VILLAGER,

	/**
	 * Trying to attack a dead villager
	 */
	NO_BUILD_FACTORY_THAT_DOSENT_EXIST,

	/**
	 * Trying to build or upgrade with insufficient money
	 */
	INSUFFICIENT_FUNDS,

	/**
	 * Trying to build factory after reaching maximum limit
	 */
	NO_MORE_FACTORIES,

	/**
	 * Trying to attack factory that will be razed in same turn by opponent
	 */
	NO_ATTACK_RAZED_FACTORY,

	/**
	 * Trying to build factory on water
	 */
	NO_BUILD_FACTORY_ON_WATER,

	/**
	 * Trying to build factory on gold mine
	 */
	NO_BUILD_FACTORY_ON_GOLD_MINE,

	/**
	 * Invalid value for attack target actor id
	 */
	INVALID_TARGET_ID,

	/**
	 * Trying to build a factory in a position that is already occupied
	 */
	POSITION_OCCUPIED
};

/**
 * Provides an easy way to get the string representation of the above enum
 */
const std::vector<std::string> ErrorTypeName = {
    "NO_MULTIPLE_SOLDIER_TASKS",
    "NO_MULTIPLE_VILLAGER_TASKS",
    "NO_MULTIPLE_FACTORY_TASKS",
    "NO_ALTER_ACTOR_ID",
    "NO_ACTION_BY_DEAD_SOLDIER",
    "NO_ACTION_BY_DEAD_VILLAGER",
    "NO_ACTION_BY_DEAD_FACTORY",
    "INVALID_MOVE_POSITION",
    "INVALID_MINE_POSITION",
    "INVALID_BUILD_POSITION",
    "NO_ATTACK_SELF_SOLDIER",
    "NO_ATTACK_SELF_VILLAGER",
    "NO_ATTACK_SELF_FACTORY",
    "NO_ATTACK_DEAD_SOLDIER",
    "NO_ATTACK_DEAD_VILLAGER",
    "NO_BUILD_FACTORY_THAT_DOSENT_EXIST",
    "INSUFFICIENT_FUNDS",
    "NO_MORE_FACTORIES",
    "NO_ATTACK_RAZED_FACTORY",
    "NO_BUILD_FACTORY_ON_WATER",
    "NO_BUILD_FACTORY_ON_GOLD_MINE",
    "INVALID_TARGET_ID",
    "POSITION_OCCUPIED"};

} // namespace logger