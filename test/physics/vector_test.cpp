#include "physics/vector.hpp"
#include "gtest/gtest.h"
#include <cstdint>

using namespace std;
using namespace physics;
using namespace testing;

TEST(VectorTest, EqualityTest) {
	Vector<int64_t> a1 = {2, 4};
	Vector<int64_t> a2 = {2, 4};

	ASSERT_EQ(a1, a2);
}

TEST(VectorTest, AdditionTest) {
	Vector<int32_t> a = {2, 4};
	Vector<int32_t> b = {1, 2};

	Vector<int32_t> c = {3, 6};
	ASSERT_EQ(a + b, c);
}

TEST(VectorTest, ScalarMultiplicationTest) {
	Vector<float> a = {2, 4};
	int scalar = 3;

	Vector<float> ans = {6, 12};
	ASSERT_EQ(a * scalar, ans);
}

TEST(VectorTest, ScalarDivisionTest) {
	Vector<float> a = {2, 4};
	int scalar = 2;

	Vector<float> ans = {1, 2};
	ASSERT_EQ(a / scalar, ans);
}

TEST(VectorTest, DotProductTest) {
	Vector<float> a = {1, 2};
	Vector<float> b = {0.5, 1};

	double c = 2.5;
	ASSERT_EQ(a.dot(b), c);
}

TEST(VectorTest, MagnitudeTest) {
	Vector<uint8_t> a = {3, 4};

	double mag = 5;
	ASSERT_EQ(a.magnitude(), mag);
}

TEST(VectorTest, FloorTest) {
	Vector<double> a = {3.99, 4.01};

	Vector<double> ans = {3, 4};
	ASSERT_EQ(a.floor(), ans);
}

TEST(VectorTest, BoolCastTest) {
	Vector<double> nullval1 = {-1, -1};
	Vector<double> nullval2 = Vector<double>::null;
	Vector<double> notnullval = {4.0, 7.0};

	ASSERT_FALSE(nullval1);
	ASSERT_FALSE(nullval2);
	ASSERT_TRUE(notnullval);
}
