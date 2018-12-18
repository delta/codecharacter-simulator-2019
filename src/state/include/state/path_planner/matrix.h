/**
 * @file matrix.h
 * Declares the matrix type, a helpful alias for a 2D vector and some helpers
 */

#pragma once

#include "physics/vector.hpp"

/**
 * 2D matrix alias type
 */
template <typename T> using Matrix = std::vector<std::vector<T>>;

/**
 * Initializes matrix with elements of the given value and returns it
 * Takes a single size as the parameter
 *
 * @param[in]  init_value  The initialize value
 * @param[in]  size        The size of the matrix
 *
 * @tparam     T           Data type matrix holds
 *
 * @return     The initialized 2D matrix
 */
template <typename T> inline Matrix<T> InitMatrix(T init_value, int64_t size) {
	std::vector<T> row(size, init_value);
	Matrix<T> a(size, row);
	return a;
}

/**
 * Helper method to get the a matrix element, using a Vec2D as the indices
 */
template <typename T> inline T &GetAt(Matrix<T> &m, Vec2D index) {
	return m[index.x][index.y];
}
