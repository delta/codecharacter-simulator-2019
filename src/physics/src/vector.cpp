/**
 * @file vector.cpp
 * Definitions for the 2D Vector class
 */

#include "physics/vector.h"
#include <cmath>

namespace physics {

template <typename T>
Vector<T>::Vector() : x(), y() {}

template <typename T>
Vector<T>::Vector(T x, T y) : x(x), y(y) {}

template <typename T>
bool Vector<T>::operator==(const Vector<T> &rhs) const {
	return (x == rhs.x && y == rhs.y);
}

template <typename T>
bool Vector<T>::operator!=(const Vector<T> &rhs) const {
	return (x != rhs.x || y != rhs.y);
}

template <typename T>
Vector<T> Vector<T>::operator+(const Vector<T> &rhs) const {
	return Vector(x + rhs.x, y + rhs.y);
}

template <typename T>
Vector<T> Vector<T>::operator-(const Vector<T> &rhs) const {
	return Vector(x - rhs.x, y - rhs.y);
}

template <typename T>
template <typename T2>
Vector<T> Vector<T>::operator+(const T2 &scalar) const {
	return Vector(x + scalar, y + scalar);
}

template <typename T>
template <typename T2>
Vector<T> Vector<T>::operator-(const T2 &scalar) const {
	return Vector(x - scalar, y - scalar);
}

template <typename T>
template <typename T2>
Vector<T> Vector<T>::operator*(const T2 &scalar) const {
	return Vector(x * scalar, y * scalar);
}

template <typename T>
template <typename T2>
Vector<T> Vector<T>::operator/(const T2 &scalar) const {
	return Vector(x / scalar, y / scalar);
}

template <typename T>
std::ostream &operator<<(std::ostream &ostream, const Vector<T> &vector) {
	ostream << "(" << vector.x << ", " << vector.y << ")";
	return ostream;
}

template <typename T>
double Vector<T>::dot(const Vector<T> &rhs) const { return (x * rhs.x + y * rhs.y); }

template <typename T>
double Vector<T>::magnitude() const { return sqrt(x * x + y * y); }

template <typename T>
double Vector<T>::distance(const Vector<T> &other) const {
	return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2));
}

template <typename T>
Vector<T> Vector<T>::floor() const { return Vector<T>(std::floor(x), std::floor(y)); }

template <typename T>
Vector<T> Vector<T>::ceil() const { return Vector<T>(std::ceil(x), std::ceil(y)); }
}
