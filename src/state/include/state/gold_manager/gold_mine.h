#include "physics/vector.hpp"

namespace state {

class GoldMine {
  public:
	/**
	 * A vector to hold the offset where the gold mine is located
	 */
	Vec2D offset;

	/**
	 * Holds the amount of gold reserve that the gold mine has
	 */
	int64_t value;

	/**
	 * Constructors
	 */

	GoldMine();

	GoldMine(Vec2D offset, int64_t value);

	/**
	 * Helper function to extract valid amount of gold depending on amount of
	 * gold left in the gold mine
	 */
	int64_t ExtractGold(int64_t ext_amount);
};

} // namespace state