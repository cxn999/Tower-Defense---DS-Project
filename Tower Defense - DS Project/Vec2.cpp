#include "Vec2.h"
#include <math.h>

// File to define implementation of Vec2 class methods

bool Vec2::operator == (const Vec2& rhs) const { // Check
	return (x == rhs.x && y == rhs.y);
}

bool Vec2::operator != (const Vec2& rhs) const {
	return !(x == rhs.x && y == rhs.y);
}

Vec2 Vec2::operator + (const Vec2& rhs) const {
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const {
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator * (const float c) {
	return Vec2(x * c, y * c);
}

Vec2 Vec2::operator / (const float c) {
	return Vec2(x / c, y / c);
}

void Vec2::operator += (const Vec2& rhs) {
	x += rhs.x; y += rhs.y;
}

void Vec2::operator -= (const Vec2& rhs) {
	x -= rhs.x; y -= rhs.y;
}

void Vec2::operator *= (const float c) {
	x *= c; y *= c;
}

void Vec2::operator /= (const float c) {
	x /= c; y /= c;
}

// Distance between 2 vectors
float Vec2::dist(const Vec2& rhs) const {
	return sqrt((x - rhs.x) * (x - rhs.x) + (y - rhs.y) * (y - rhs.y));
}