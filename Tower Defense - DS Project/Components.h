#pragma once

#include "Animation.h"
#include "Assets.h"
#include "Vec2.h"
#include <string>
#include <SFML/Graphics.hpp>
#include <memory>

class Entity;

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
	bool move = true;

	CTransform() {}
	CTransform(const Vec2 & p) : pos(p) {}
	CTransform(const Vec2& p, const Vec2& v, const Vec2& sc, float a, bool m) : pos(p), velocity(v), scale(sc), angle(a), move(m) {}
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
	bool rightClick = false;
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
	std::string direction = "default";
	std::string effect;
	CState() {}
	CState(std::string s, std::string d) : state(s), direction(d) {}
};

class CLevel : public Component {
public:
	size_t level = 1;
	size_t max_level = 4;
	CLevel() {}
	CLevel(size_t l) : level(l) {}
};

class CHealth : public Component {
public:
	float totalHealth = 0;
	float health = 0;
	float prevHealth = 0;
	CHealth() {}
	CHealth(float h) : totalHealth(h), health(h), prevHealth(h) {}
};

class CAttack : public Component {
public:
	float damage = 0;
	CAttack() {}
	CAttack(float d) : damage(d) {}
};

class CRange : public Component {
public:
	float radius = 0;
	bool target = false;
	CRange(float r) : radius(r) {}
	CRange() {}
};

class CType : public Component {
public:
	std::string type;

	CType(std::string t) : type(t) {}
	CType() {}
};

class CDelay : public Component {
public:
	size_t lastAttackFrame;
	size_t delay;

	CDelay(size_t l, size_t d) : lastAttackFrame(l), delay(d) {}
	CDelay() {}
};

class CFocus : public Component {
public:
	std::shared_ptr<Entity> entity;

	CFocus(std::shared_ptr<Entity> e) {
		entity = e;
	}
	CFocus() {}
};

class CFocusList : public Component {
public:
	std::vector<std::shared_ptr<Entity>> entities;

	CFocusList() {}
};