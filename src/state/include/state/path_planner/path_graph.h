/**
 * @file path_graph.h
 * Implements the PathGraph class, which performs path searches
 */

#pragma once

#include "physics/vector.hpp"
#include "state/map/map.h"
#include "state/path_planner/matrix.h"
#include "state/path_planner/open_list.h"

#include <vector>

namespace state {

class PathGraph {
  private:
	/**
	 * Map graph that specifies valid terrain
	 */
	Matrix<bool> graph;

	/**
	 * Returns valid neighbour tiles of an offset, from the map
	 *
	 * @param offset Grid location on the map
	 * @return std::vector<Vec2D> List of neighbour offsets
	 */
	std::vector<Vec2D> GetNeighbours(Vec2D offset);

	/**
	 * Find a path between the two given offsets
	 *
	 * @param start_offset
	 * @param target_offset
	 * @return std::vector<Vec2D> List of nodes with the path
	 */
	std::vector<Vec2D> GetPath(Vec2D start_offset, Vec2D target_offset);

	/**
	 * Precomputed paths from all nodes to all other nodes
	 */
	Matrix<Matrix<Vec2D>> path_cache;

	/**
	 * Booleans to set whether the path between two nodes has been computed yet
	 */
	Matrix<Matrix<bool>> is_path_computed;

	/**
	 * Run precomputation for all node paths
	 */
	void GeneratePathCache();

	/**
	 * Adds all possible paths from given path to node cache
	 *
	 * @param path List of nodes
	 * @param start index
	 * @param end index
	 */
	void AddPathToCache(std::vector<Vec2D> &path, size_t start, size_t end);

	/**
	 * Gets the next node from the open list
	 *
	 * @param[inout] offset Vec2D passed by reference to return next node
	 * @return true Success, node returned
	 * @return false Failure, open list is empty
	 */
	bool GetNextSmallestNodeFromOpenList(Vec2D &offset);

	/**
	 * Clear the open list by unsetting all nodes, set a new start point
	 *
	 * @param start_offset source node to initialize
	 */
	void InitOpenList(Vec2D start_offset);

	/**
	 * Matrix of entries, comprising the open list. This info is used to pick
	 * the next best node in the A* algorithm.
	 */
	Matrix<OpenListEntry> open_list;

	/**
	 * Vector holding the index of the node that's the head of the Open List
	 */
	Vec2D open_list_head;

	/**
	 * Size of the graph
	 */
	size_t size;

  public:
	PathGraph();
	PathGraph(Matrix<bool> graph);

	/**
	 * Get the next offset to move to, given the current offset
	 *
	 * @param source Current offset
	 * @param destination Target offset
	 * @return Vec2D Next offset along the path
	 */
	Vec2D GetNextNode(Vec2D source, Vec2D destination);
};

} // namespace state
