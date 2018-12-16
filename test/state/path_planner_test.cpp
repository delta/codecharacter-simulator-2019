#include "state/path_planner/path_planner.h"
#include "gtest/gtest.h"

using namespace std;
using namespace state;
using namespace physics;
using namespace testing;

const auto MAP_SIZE = size_t{5};
const auto ELEMENT_SIZE = size_t{10};

const auto L = TerrainType::LAND;
const auto W = TerrainType::WATER;
const auto G = TerrainType::GOLD_MINE;

class PathPlannerTest : public Test {
  protected:
	unique_ptr<Map> map;
	unique_ptr<PathPlanner> path_planner;

  public:
	PathPlannerTest() {
		auto map_matrix = vector<vector<TerrainType>>{{
		    {L, L, L, L, L},
		    {L, L, L, L, L},
		    {L, L, L, L, L},
		    {L, L, L, L, L},
		    {L, L, L, L, L},
		}};

		map = make_unique<Map>(map_matrix, MAP_SIZE, ELEMENT_SIZE);

		path_planner = make_unique<PathPlanner>(map.get());
	}
};

TEST_F(PathPlannerTest, SampleTest) {
	// Sample Test
	ASSERT_TRUE(path_planner != nullptr);
}
