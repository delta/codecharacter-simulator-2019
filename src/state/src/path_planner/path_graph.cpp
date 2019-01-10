/**
 * @file path_graph.cpp
 * Defines the PathGraph class
 */

#include "state/path_planner/path_graph.h"

#include <algorithm>

namespace state {

PathGraph::PathGraph() {}

PathGraph::PathGraph(Matrix<bool> graph) : graph(graph), size(graph.size()) {
	open_list = InitMatrix(OpenListEntry{Vec2D::null, 0, 0, Vec2D::null, false,
	                                     Vec2D::null, Vec2D::null, false},
	                       size);

	// Init open_list.pos elems with respective positions
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			open_list[i][j].pos = Vec2D{i, j};
		}
	}

	// Compute paths for all nodes
	GeneratePathCache();
}

void PathGraph::InitOpenList(Vec2D start_offset) {
	// Unset all entries
	for (auto &row : open_list) {
		for (auto &open_list_entry : row) {
			open_list_entry.is_set = false;
		}
	}

	// Initialize head element
	open_list_head = start_offset;
	GetAt(open_list, open_list_head) =
	    OpenListEntry{open_list_head, 0,           0,           Vec2D::null,
	                  false,          Vec2D::null, Vec2D::null, true};
}

bool PathGraph::GetNextSmallestNodeFromOpenList(Vec2D &offset) {
	// Get head node
	auto current_node = GetAt(open_list, open_list_head);
	auto found_an_unvisited_node = false;

	auto min_offset = Vec2D::null;
	auto min_cost = INT64_MAX;

	// Traverse the linked list of open list entries to find the smallest cost
	while (true) {
		if (current_node && not current_node.is_closed) {
			found_an_unvisited_node = true;
			if (current_node.total_cost < min_cost) {
				min_cost = current_node.total_cost;
				min_offset = current_node.pos;
			}
		}

		// If we've reached the end of the list...
		if (not current_node || current_node.next == Vec2D::null) {
			break;
		}

		// Increment node pointer
		current_node = GetAt(open_list, current_node.next);
	}

	// If we found an unvisited node, return it through &offset
	if (found_an_unvisited_node) {
		offset = min_offset;
		return true;
	}

	return false;
}

Vec2D PathGraph::GetNextNode(Vec2D source, Vec2D destination) {
	// If source or destination are out of bounds...
	if (source.x < 0 || source.x >= size || source.y < 0 || source.y >= size ||
	    destination.x < 0 || destination.x >= size || destination.y < 0 ||
	    destination.y >= size) {
		return Vec2D::null;
	}

	// If source or destination are invalid locations (water)...
	if (not graph[source.x][source.y] ||
	    not graph[destination.x][destination.y]) {
		return {};
	}

	// If the source and destination are the same...
	if (source == destination) {
		return {};
	}
	auto source_matrix = GetAt(path_cache, destination);
	auto next_node = GetAt(source_matrix, source);

	return next_node;
}

std::vector<Vec2D> PathGraph::GetPath(Vec2D start_offset, Vec2D target_offset) {
	// If source or destination are invalid locations (water)...
	if (not graph[start_offset.x][start_offset.y] ||
	    not graph[target_offset.x][target_offset.y]) {
		return {};
	}

	// If the source and destination are the same...
	if (start_offset == target_offset) {
		return {};
	}

	// Clear the Open List and init the start location
	InitOpenList(start_offset);

	// InOut param to pass, that will hold the next node to process
	auto current_offset = Vec2D::null;

	while (GetNextSmallestNodeFromOpenList(current_offset)) {
		// current_offset always holds the offset of the lowest weight node
		auto &current_node = GetAt(open_list, current_offset);

		// If target it found, stop and get the full path
		if (current_offset == target_offset) {
			auto result = std::vector<Vec2D>{};
			auto result_node = current_node;

			// Traceback through the nodes' parents to get the complete path
			while (result_node && result_node.parent != Vec2D::null) {
				result.push_back(result_node.pos);
				result_node = GetAt(open_list, result_node.parent);
			}

			// Note: Path is reversed
			return result;
		}

		// For each neighbour of the current node...
		for (auto &neighbour_pos : GetNeighbours(current_node.pos)) {
			auto &neighbour = GetAt(open_list, neighbour_pos);

			// If we've already seen this node before, and it has been closed...
			if (neighbour && neighbour.is_closed) {
				continue;
			}

			// Find path cost and and total cost (path cost + heuristic cost)
			auto path_cost =
			    current_node.cost + neighbour_pos.distance(current_node.pos);
			auto total_cost = path_cost + neighbour_pos.distance(target_offset);

			if (not neighbour) {
				// Set properties of this node
				neighbour.cost = path_cost;
				neighbour.total_cost = total_cost;
				neighbour.parent = current_node.pos;
				neighbour.is_closed = false;
				neighbour.is_set = true;

				// Add this node to top of the open list
				auto &head = GetAt(open_list, open_list_head);
				head.prev = neighbour.pos;
				neighbour.next = open_list_head;
				neighbour.prev = Vec2D::null;
				open_list_head = neighbour.pos;
			} else {
				// If we've already found a lower cost in our open list...
				if (neighbour.total_cost < total_cost) {
					continue;
				} else {
					// Update this node with the better cost
					neighbour.cost = path_cost;
					neighbour.total_cost = total_cost;
					neighbour.parent = current_node.pos;
					neighbour.is_closed = false;
					neighbour.is_set = true;
				}
			}
		}

		// Close the current node and remove it from the open_list
		if (open_list_head == current_node.pos) {
			open_list_head = current_node.next;
		}
		if (current_node.prev) {
			GetAt(open_list, current_node.prev).next = current_node.next;
		}
		if (current_node.next) {
			GetAt(open_list, current_node.next).prev = current_node.prev;
		}
		current_node.next = Vec2D::null;
		current_node.prev = Vec2D::null;
		current_node.is_closed = true;
	}

	// No valid path exists
	return std::vector<Vec2D>{};
}

std::vector<Vec2D> PathGraph::GetNeighbours(Vec2D offset) {
	auto neighbours = std::vector<Vec2D>{};
	neighbours.reserve(4);

	auto rel_offsets = std::vector<Vec2D>{{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
	auto current_offset = Vec2D::null;

	// For each of the 4 positions around the current offset...
	for (const auto &rel_offset : rel_offsets) {
		auto current_offset = offset + rel_offset;

		// If the location is within bounds and a valid location...
		if (current_offset.x >= 0 && current_offset.x < size &&
		    current_offset.y >= 0 && current_offset.y < size &&
		    graph[current_offset.x][current_offset.y]) {
			neighbours.push_back(current_offset);
		}
	}

	return neighbours;
}

} // namespace state
