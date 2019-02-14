#include "state/utilities.h"

namespace state {

struct BuildRequest {
	/**
	 * Id of the villager who is trying to build the factory
	 */
	int64_t villager_id;

	/**
	 * The type of actor that the player wants to factory to produce
	 */
	state::ActorType prod_type;

	/**
	 * A vector for the offset position where the factory is to be built
	 */
	Vec2D offset;
};

} // namespace state