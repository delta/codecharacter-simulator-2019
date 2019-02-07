#include "state/gold_manager/gold_mine.h"

namespace state {

GoldMine::GoldMine() : offset(Vec2D::null), value(0){};

GoldMine::GoldMine(Vec2D offset, int64_t value) : offset(offset), value(value) {};

int64_t GoldMine::ExtractGold(int64_t ext_amount) {
	if (this->value - ext_amount < 0) {
		this->value = 0;
		return this->value;
	} else {
		this->value -= ext_amount;
		return ext_amount;
	}
}
} // namespace state