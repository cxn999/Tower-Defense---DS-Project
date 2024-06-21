#pragma once

#include <map>
#include <string>
#include "Action.h"
#include "EntityManager.h"

class GameEngine;

typedef std::map<int, std::string> ActionMap;

class Scene {
protected:
	GameEngine* m_game = nullptr;
	EntityManager m_entityManager;
	ActionMap m_actionMap;

	size_t m_currentFrame;
	bool m_paused = false;
	bool m_hasEnded = false;

	virtual void onEnd() = 0;

public:
	void setPaused(bool);

	Scene();
	Scene(GameEngine* gameEngine);

	// PURE VIRTUAL: ALL DERIVED CLASSES DEFINE THIS METHODS
	virtual void update() = 0;
	// Jump, move right, left, etc
	virtual void sDoAction(const Action & action) = 0;
	// Render the scene
	virtual void sRender() = 0;

	// Some derived classes may or may not define this method
	// what is thisss
	// THIS SHOULD BE VIRTUAL BUT CRASHES BECAUSE THERE IS NO DEFINITION I THINK
	virtual void doAction(const Action& action);
	// OK???
	void simulate(const size_t frames);
	// I MAYBE UNDERSTAND
	void registerAction(int inputKey, const std::string & actionName);

	size_t width() const;
	size_t height() const;
	size_t currentFrame() const;

	bool hasEnded() const;
	const ActionMap& getActionMap() const;
	// WHAT IS THIS
	void drawLine(const Vec2& p1, const Vec2& p2);
};