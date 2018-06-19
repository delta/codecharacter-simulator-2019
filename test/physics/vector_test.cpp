#include "vector"
#include "gtest/gtest.h"
#include <iostream>

using namespace std;
using namespace physics;
using namespace testing;

class VectorTest : public Test {
  protected:
	unique_ptr<Vector<double>> vectorA, vectorB, vectorC;
	VectorTest() {
		this->vectorA = make_unique<Vector<double>>();
		this->vectorB = make_unique<Vector<double>>();
		this->vectorC = make_unique<Vector<double>>();
	}
};

TEST_F(VectorTest, EqualityTest) {
	vectorA->x = 1.0;
	vectorA->y = 2.0;
	vectorB->x = 1.0;
	vectorB->y = 2.0;
	ASSERT_EQ(*vectorA == *vectorB, true);
}

TEST_F(VectorTest, AdditionTest) {
	vectorA->x = 1.0;
	vectorA->y = 2.0;
	vectorB->x = 1.0;
	vectorB->y = 2.0;
	vectorC->x = 2.0;
	vectorC->y = 4.0;
	ASSERT_EQ((*vectorA + *vectorB) == *vectorC, true);
}

TEST_F(VectorTest, ScalarMultiplicationTest) {
	vectorA->x = 1.0;
	vectorA->y = 2.0;
	int scalar = 3;
	vectorC->x = 3.0;
	vectorC->y = 6.0;
	ASSERT_EQ(((*vectorA)*scalar) == *vectorC, true);
}

TEST_F(VectorTest, ScalarDivisionTest) {
	vectorA->x = 1.0;
	vectorA->y = 2.0;
	int scalar = 2;
	vectorC->x = 0.5;
	vectorC->y = 1.0;
	ASSERT_EQ(((*vectorA) / scalar) == *vectorC, true);
}

TEST_F(VectorTest, DotProductTest) {
	vectorA->x = 1.0;
	vectorA->y = 2.0;
	vectorB->x = 0.5;
	vectorB->y = 1.0;
	double dotProduct = 2.5;
	ASSERT_EQ(vectorA->dot(*vectorB) == dotProduct, true);
}

TEST_F(VectorTest, MagnitudeTest) {
	vectorA->x = 3.0;
	vectorA->y = 4.0;
	double magnitude = 5.0;
	ASSERT_EQ(vectorA->magnitude() == magnitude, true);
}
/* 
TEST_F(VectorTest, FloorTest) {
	vectorA->x = 3.99;
	vectorA->y = 4.01;
	vectorB->x = 3;
	vectorB->y = 4;
	ASSERT_EQ(vectorA->floor() == *vectorB, true);
}
 */