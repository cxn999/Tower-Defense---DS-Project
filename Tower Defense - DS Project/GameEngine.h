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

	float m_volume = 50;
	bool m_music = false;

	// Should load the assets
	void init(const std::string& path);
	void update();

	void sUserInput();
	std::shared_ptr<Scene> currentScene();
	sf::Music* m_currentMusic; // Pointer to the current music track
public:
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

	/// methods to get configurations
	float getVolume();
	bool getMusic();

	/// methods to set configurations
	void setVolume(float volume);
	void setMusic(bool music);

	/// methods for musics
	void startMusic(const std::string& musicName);
	void stopMusic();
	void setMusicState(bool musicOn);
	void switchToMusic(const std::string& musicName);
};