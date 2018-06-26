#include "gmock/gmock.h"
#include "physics/vector.h"
#include "gtest/gtest.h"
#include <cstdint>

using namespace std;
using namespace physics;
using namespace testing;

class VectorTest : public Test {
  protected:
	unique_ptr<Vector> vector;
	double x, y;
	VectorTest() {
		this->x = 3.0;
		this->y = 4.0;
		this->vector = make_unique<Vector>(x, y);
	}
};

TEST(VectorTest, ValidOperations) {}