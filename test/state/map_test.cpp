#include "state/map/map.h"

#include <gtest/gtest.h>
#include <memory>

using namespace std;
using namespace state;

const auto MAP_SIZE = size_t{5};
const auto ELEMENT_SIZE = size_t{10};

const auto L = TerrainType::LAND;
const auto W = TerrainType::WATER;
const auto G = TerrainType::GOLD_MINE;

class MapTest : public testing::Test {
  protected:
	unique_ptr<Map> map;
	MapTest() {
		auto map_matrix = vector<vector<TerrainType>>{{
		    {L, L, L, L, L},
		    {L, L, L, L, L},
		    {G, G, G, G, G},
		    {W, W, W, W, W},
		    {W, W, W, W, W},
		}};

		map = make_unique<Map>(map_matrix, MAP_SIZE, ELEMENT_SIZE);
	}
};

TEST_F(MapTest, GetterTest) {
	EXPECT_EQ(map->GetSize(), MAP_SIZE);
	EXPECT_EQ(map->GetElementSize(), ELEMENT_SIZE);
}

TEST_F(MapTest, GetElementByOffsetTest) {
	EXPECT_EQ(map->GetTerrainTypeByOffset(0, 0), TerrainType::LAND);
	EXPECT_EQ(map->GetTerrainTypeByOffset(1, 1), TerrainType::LAND);
	EXPECT_EQ(map->GetTerrainTypeByOffset(2, 2), TerrainType::GOLD_MINE);
	EXPECT_EQ(map->GetTerrainTypeByOffset(4, 4), TerrainType::WATER);
	EXPECT_EQ(map->GetTerrainTypeByOffset(0, 4), TerrainType::LAND);
	EXPECT_EQ(map->GetTerrainTypeByOffset(4, 0), TerrainType::WATER);
}

TEST_F(MapTest, GetElementByPositionTest) {
	EXPECT_EQ(map->GetTerrainTypeByPosition(0, 0), TerrainType::LAND);
	EXPECT_EQ(map->GetTerrainTypeByPosition(15, 15), TerrainType::LAND);
	EXPECT_EQ(map->GetTerrainTypeByPosition(24, 24), TerrainType::GOLD_MINE);
	EXPECT_EQ(map->GetTerrainTypeByPosition(49, 49), TerrainType::WATER);
	EXPECT_EQ(map->GetTerrainTypeByPosition(0, 49), TerrainType::LAND);
	EXPECT_EQ(map->GetTerrainTypeByPosition(49, 0), TerrainType::WATER);
}
