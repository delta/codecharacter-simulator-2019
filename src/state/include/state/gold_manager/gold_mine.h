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

	/**
	 * Helper function to get the amount of gold in the GoldMine
	 */

	int64_t GetGoldReserve();

	/**
	 * Helper function to set the amount of gold reserve in a GoldMine
	 */
	void SetGoldReserve(int64_t gold_value);
};

/**
 * Defining a way to compare 2 GoldMine objects so that is can be stored in an
 * unordered hash map
 */

class GoldMineCompare {
  public:
	bool operator()(const GoldMine *lhs, const GoldMine *rhs) {
		return (lhs->value - rhs->value);
	}
};

} // namespace state
