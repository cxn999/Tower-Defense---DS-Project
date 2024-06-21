#pragma once

// Class to define a vector of 2 dimensions
class Vec2 {
public:
	// Fields are x and y coordinates
	float x, y;

	// Default constructor
	Vec2() = default;
	// Constructor that takes x and y parameters
	Vec2(float xin, float yin) : x(xin), y(yin) {}

	// Multiple operator overloading methods
	// Implementation defined at Vec2.cpp
	bool operator == (const Vec2& rhs) const;
	bool operator != (const Vec2& rhs) const;

	Vec2 operator + (const Vec2& rhs) const;
	Vec2 operator - (const Vec2& rhs) const;
	Vec2 operator * (const float c);
	Vec2 operator / (const float c);

	void operator += (const Vec2& rhs);
	void operator -= (const Vec2& rhs);
	void operator *= (const float c);
	void operator /= (const float c);

	// Distance between 2 vectors
	float dist(const Vec2& rhs) const;
};
