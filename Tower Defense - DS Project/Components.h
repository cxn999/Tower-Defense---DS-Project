#pragma once

#include "Animation.h"
#include "Assets.h"
#include "Vec2.h"
#include <string>

class Component {
public:
	bool has = false;
};

class CTransform : public Component {
public:
	Vec2 pos = { 0.0, 0.0 };
	Vec2 prevPos = { 0.0, 0.0 };
	Vec2 scale = { 1.0, 1.0 };
	Vec2 velocity = { 0.0, 0.0 };
	float angle = 0;

	CTransform() {}
	CTransform(const Vec2 & p) : pos(p) {}
	CTransform(const Vec2& p, const Vec2& v, const Vec2& sc, float a) : pos(p), velocity(v), scale(sc), angle(a) {}
};

class CLifeSpan : public Component {
public:
	int lifespan = 0;
	int frameCreated = 0;
	CLifeSpan() {}
	CLifeSpan(int duration, int frame) : lifespan(duration), frameCreated(frame) {}
};

class CInput : public Component {
public:
	bool click = false;
};

class CBoundingBox : public Component {
public:
	Vec2 size;
	Vec2 halfSize;
	CBoundingBox() = default;
	CBoundingBox(const Vec2 & s) : size(s), halfSize(s.x/2, s.y/2) {}
};

class CAnimation : public Component {
public:
	Animation animation;
	bool repeat = false;
	CAnimation() {}
	CAnimation(const Animation & animation, bool r) : animation(animation), repeat(r) {}
};

class CGravity : public Component {
public:
	float gravity = 0;
	CGravity() {}
	CGravity(float g) : gravity(g) {}
};

class CState : public Component {
public:
	std::string state = "idle";
	CState() {}
	CState(std::string s) : state(s) {}
};

class CLevel : public Component {
public:
	size_t level = 1;
	CLevel() {}
	CLevel(size_t l) : level(l) {}
};

class CHealth : public Component {
public:
	float totalHealth = 0;
	float health = 0;
	CHealth() {}
	CHealth(float h) : totalHealth(h), health(h) {}
};

class CAttack : public Component {
public:
	float damage = 0;
	CAttack() {}
	CAttack(float d) : damage(d) {}
};