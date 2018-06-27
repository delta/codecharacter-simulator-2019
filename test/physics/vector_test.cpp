#include "physics/vector.h"
#include "gtest/gtest.h"

using namespace std;
using namespace physics;
using namespace testing;

class VectorTest : public Test {
  protected:
	unique_ptr<Vector> vectorA, vectorB;
	VectorTest() {
		this->vectorA = make_unique<Vector>();
		this->vectorB = make_unique<Vector>();
	}
};

TEST_F(VectorTest, EqualityTest) {
	vectorA->x = 1.0;
	vectorA->y = 2.0;
	vectorB->x = 1.0;
	vectorB->y = 2.0;
	ASSERT_EQ(*vectorA == *vectorB, true);
}
