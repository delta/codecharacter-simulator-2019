#include "constants/constants.h"
#include "state/map/map.h"
#include "state/utilities.h"
#include <fstream>
#include <iostream>
#include <memory>

using namespace std;
using namespace state;

/*
 * The example file should be of the format -
 * L L L L L
 * L W W W L
 * L L G L L
 * L L L L L
 * L L L L L
 * File and each line ends with a '\n'. TerrainType separated by ' '.
 */
const auto MAP_FILE_NAME = "example.txt";

const auto L = TerrainType::LAND;
const auto W = TerrainType::WATER;
const auto G = TerrainType::GOLD_MINE;

unique_ptr<Map> BuildMap() {
	auto map_elements = vector<vector<TerrainType>>{};

	auto map_file = ifstream(MAP_FILE_NAME, ifstream::in);

	// Compute file size
	map_file.seekg(0, map_file.end);
	auto map_file_size = map_file.tellg();
	map_file.seekg(0);

	auto map_file_input = vector<char>(map_file_size, '\0');
	map_file.read(&map_file_input[0], map_file_size);

	auto map_row = vector<TerrainType>{};
	for (auto character : map_file_input) {
		switch (character) {
		case 'L':
			map_row.push_back(L);
			break;
		case 'W':
			map_row.push_back(W);
			break;
		case 'G':
			map_row.push_back(G);
			break;
		case ' ':
			break;
		case '\n':
			map_elements.push_back(map_row);
			map_row.clear();
			break;
		}
	}
	map_file.close();

	// TODO : Put map element size here
	return make_unique<Map>(map_elements, map_elements.size(), 10);
}

int main(int argc, char *argv[]) {
	std::cout << "Hello World" << std::endl;
	return 0;
}
