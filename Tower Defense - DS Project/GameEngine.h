#pragma once

#include "Assets.h"
#include <memory>
#include <map>
#include <string>

#include <iostream>

class Scene;

typedef std::map<std::string, std::shared_ptr<Scene>> SceneMap;

class GameEngine {
protected:
	sf::RenderWindow m_window;
	Assets m_assets;
	std::string m_currentScene;
	SceneMap m_sceneMap;
	size_t m_simulationSpeed = 1;
	bool m_running = true;


	// Should load the assets
	void init(const std::string& path);
	void update();

	void sUserInput();
	std::shared_ptr<Scene> currentScene();

public:
	sf::Clock m_clock;
	float m_fps;

	GameEngine(const std::string& path);

	// Change scene  
	void changeScene(const std::string &, std::shared_ptr<Scene> scene, bool endCurrentScene);

	// Main loop of the game
	void run();
	// Maybe quit the game?
	void quit();
	
	Assets& getAssets();
	sf::RenderWindow& window();
	// const Assets & assets() const;
	bool isRunning();
};