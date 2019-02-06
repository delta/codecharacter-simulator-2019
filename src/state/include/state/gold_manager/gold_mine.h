#include "physics/vector.hpp"

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

	void ExtractGold(int64_t mine_rate);
};