#include "state/path_planner/path_planner.h"
#include "gtest/gtest.h"

using namespace std;
using namespace state;
using namespace physics;
using namespace testing;

const auto ELEMENT_SIZE = size_t{10};

const auto L = TerrainType::LAND;
const auto W = TerrainType::WATER;
const auto G = TerrainType::GOLD_MINE;

class PathPlannerTest : public Test {
  protected:
	unique_ptr<Map> map;
	unique_ptr<PathPlanner> path_planner;

  public:
	void InitPathPlanner(std::vector<std::vector<TerrainType>> map_matrix,
	                     size_t element_size) {
		map = make_unique<Map>(map_matrix, map_matrix.size(), element_size);
		path_planner = make_unique<PathPlanner>(map.get());
	}

	PathPlannerTest() {}
};

TEST_F(PathPlannerTest, LightMapTest) {
	// clang-format off
	auto map_matrix = vector<vector<TerrainType>>{
		{L, L, L, L, L},
		{L, L, L, L, L},
		{L, W, W, W, W},
		{L, W, L, L, L},
		{L, L, L, W, L}
	};
	// clang-format on
	InitPathPlanner(map_matrix, ELEMENT_SIZE);
	auto map_size = map_matrix.size();
	auto target =
	    DoubleVec2D(map_size * ELEMENT_SIZE - 1, map_size * ELEMENT_SIZE - 1);
	int count = 0;

	auto pos = DoubleVec2D{0, 0};
	while (pos != target) {
		pos = path_planner->GetNextPosition(pos, target, 5);
		if (pos == DoubleVec2D::null) {
			throw std::logic_error("Cannot reach destination!");
			break;
		}
		count++;
	}
	EXPECT_EQ(count, 22);
}

TEST_F(PathPlannerTest, HeavyMapTest) {
	// clang-format off
	auto map_matrix = vector<vector<TerrainType>>{
		{L, L, L, W, L, L, L, L, W, L, L, L, L, W, L, L, L, L, W, L, L, L, L, W, L, L, L, L, W, L, L, L, L, W, L, L, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, W, L, L, W, L, W, L, W, L, L, L, L},
		{L, W, L, L, L, W, L, L, L, L, L, W, L, L, L, W, L, L, L, L, L, W, L, L, L, W, L, L, L, L, L, W, L, L, L, W, L, L, L, L},
	};
	// clang-format on
	InitPathPlanner(map_matrix, ELEMENT_SIZE);
	auto map_size = map_matrix.size();
	auto target =
	    DoubleVec2D(map_size * ELEMENT_SIZE - 1, map_size * ELEMENT_SIZE - 1);
	int count = 0;

	auto pos = DoubleVec2D{0, 0};
	while (pos != target) {
		pos = path_planner->GetNextPosition(pos, target, 5);
		if (pos == DoubleVec2D::null) {
			throw std::logic_error("Cannot reach destination!");
			break;
		}
		count++;
	}
	EXPECT_EQ(count, 1244);
}
