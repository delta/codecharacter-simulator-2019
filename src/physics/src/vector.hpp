/**
 * @file vector.h
 * Declares 2D Vector class
 */

#ifndef PHYSICS_VECTOR_H
#define PHYSICS_VECTOR_H

#include "physics/physics_export.h"
#include <iostream>

namespace physics {

/**
 * Class for 2D vectors
 */
template <typename T> class PHYSICS_EXPORT Vector {
  public:
	Vector();
	Vector(T x, T y);

	/**
	 * Equal to operator for vectors
	 *
	 * @param[in]  rhs   The vector to be compared against
	 *
	 * @return     true if the vectors are equal, else false
	 */
	bool operator==(const Vector<T> &rhs) const;

	/**
	 * Not equal to operator for vectors
	 *
	 * @param[in]  rhs   The vector to be compared against
	 *
	 * @return     true if the vectors are not equal, else false
	 */
	bool operator!=(const Vector<T> &rhs) const;

	/**
	 * Addition operator for vectors
	 *
	 * @param[in]  rhs   The vector to be added with
	 *
	 * @return     Sum of the vectors
	 */
	Vector<T> operator+(const Vector<T> &rhs) const;

	/**
	 * Minus operator for vectors
	 * Subtracts the paramater passed from the vector
	 *
	 * @param[in]  rhs   The vector that subtracts from
	 *
	 * @return     Difference of the vectors
	 */
	Vector<T> operator-(const Vector<T> &rhs) const;

	/**
	 * Scalar Addition operator
	 * vec.x + scalar, vec.y + scalar
	 *
	 * @param[in]  scalar  The scalar to add
	 *
	 * @return     The new vector
	 */
	template <typename T2> Vector<T> operator+(const T2 &scalar) const;

	/**
	 * Scalar Subtraction operator
	 * vec.x - scalar, vec.y - scalar
	 *
	 * @param[in]  scalar  The scalar to subtract
	 *
	 * @return     The new vector
	 */
	template <typename T2> Vector<T> operator-(const T2 &scalar) const;

	/**
	 * Scalar multiplication operator
	 *
	 * @param[in]  scalar  The factor to multiply by
	 *
	 * @return     The scaled vector
	 */
	template <typename T2> Vector<T> operator*(const T2 &scalar) const;

	/**
	 * Scalar division operator
	 *
	 * @param[in]  scalar  The factor to divide by
	 *
	 * @return     The scaled vector
	 */
	template <typename T2> Vector<T> operator/(const T2 &scalar) const;

	template <typename T2>
	PHYSICS_EXPORT friend std::ostream &operator<<(std::ostream &ostream,
	                                               const Vector<T2> &vector);

	/**
	 * Dot product of vectors
	 *
	 * @param[in]  rhs   The vector with which the operation is performed
	 *
	 * @return     The result of the dot product
	 */
	double dot(const Vector<T> &rhs) const;

	/**
	 * The magnitude of the vector
	 *
	 * @return     The magnitude of the vector
	 */
	double magnitude() const;

	/**
	 * Distance between this and another vector
	 *
	 * @param[in]  other  The other vector
	 *
	 * @return     The distance between the two vectors
	 */
	double distance(const Vector<T> &other) const;

	/**
	 * Calculates floor of the members of the vector
	 *
	 * @return     The result Vector with floored values
	 */
	Vector<T> floor() const;

	/**
	 * Calculates ceiling of the members of the vector
	 *
	 * @return     The result Vector with ceiled values
	 */
	Vector<T> ceil() const;

	T x;
	T y;
};

template <typename T> inline Vector<T>::Vector() : x(), y() {}

template <typename T> inline Vector<T>::Vector(T x, T y) : x(x), y(y) {}

template <typename T>
inline bool Vector<T>::operator==(const Vector<T> &rhs) const {
	return (x == rhs.x && y == rhs.y);
}

template <typename T>
inline bool Vector<T>::operator!=(const Vector<T> &rhs) const {
	return (x != rhs.x || y != rhs.y);
}

template <typename T>
inline Vector<T> Vector<T>::operator+(const Vector<T> &rhs) const {
	return Vector(x + rhs.x, y + rhs.y);
}

template <typename T>
inline Vector<T> Vector<T>::operator-(const Vector<T> &rhs) const {
	return Vector(x - rhs.x, y - rhs.y);
}

template <typename T>
template <typename T2>
inline Vector<T> Vector<T>::operator+(const T2 &scalar) const {
	return Vector(x + scalar, y + scalar);
}

template <typename T>
template <typename T2>
inline Vector<T> Vector<T>::operator-(const T2 &scalar) const {
	return Vector(x - scalar, y - scalar);
}

template <typename T>
template <typename T2>
inline Vector<T> Vector<T>::operator*(const T2 &scalar) const {
	return Vector(x * scalar, y * scalar);
}

template <typename T>
template <typename T2>
inline Vector<T> Vector<T>::operator/(const T2 &scalar) const {
	return Vector(x / scalar, y / scalar);
}

template <typename T>
inline std::ostream &operator<<(std::ostream &ostream,
                                const Vector<T> &vector) {
	ostream << "(" << vector.x << ", " << vector.y << ")";
	return ostream;
}

template <typename T> inline double Vector<T>::dot(const Vector<T> &rhs) const {
	return (x * rhs.x + y * rhs.y);
}

template <typename T> inline double Vector<T>::magnitude() const {
	return sqrt(x * x + y * y);
}

template <typename T>
inline double Vector<T>::distance(const Vector<T> &other) const {
	return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2));
}

template <typename T> inline Vector<T> Vector<T>::floor() const {
	return Vector<T>(std::floor(x), std::floor(y));
}

template <typename T> inline Vector<T> Vector<T>::ceil() const {
	return Vector<T>(std::ceil(x), std::ceil(y));
}
} // namespace physics

#endif
