#pragma once
#include "Scene.h"
#include <vector>
#include <SFML/Graphics.hpp>

class Scene_Play : public Scene {
protected:
	sf::Text m_gridText;
	std::string m_levelPath;

	size_t m_coins = 700;
	sf::Text m_coinsText;
	sf::Text m_helpPauseText;

	std::vector<sf::Text> m_infoVector;

	bool m_drawInfo = false;
	bool m_drawTextures = true; // toggle textures
	bool m_drawCollision = false; // toggle hitboxes
	bool m_roadGrid = false; // draw the road grid depending on the type of item selected
	bool m_grassGrid = false; // draw the grass grid depending on the type of item selected
	bool m_mouseItem = false; // if the mouse has an item following it
	bool m_attack = false; // if player has selected an attack item and clicked in a square
	bool m_nightFall = true; // toggles change in the night filter
	bool m_pauseP = false; // bool to check if game was paused with P 

	Vec2 m_attackPos = { 0,0 };
	sf::RectangleShape m_lightningSquare; // Square of attack
	sf::RectangleShape m_defenseSquare; // Square of defense tower
	sf::RectangleShape m_nightFilter;	// Night filter square

	Vec2 m_gridSize = { 64,64 }; // size of the grid


	size_t m_currentFrame = 0; // Current frame
	size_t m_selectedItem = 0; // Selected item
	size_t m_lastFrameDefenseSpawn = 0; // Last frame in which a defense tower was placed
	size_t m_opacity = 0;	// Opacity property of the night filter
	size_t m_spawnRateFrame = 150; //Initial spawn rate of the enemies 

	std::vector<sf::RectangleShape> m_shopRectangles; // Rectangles of the shop for the items
	std::vector<sf::RectangleShape> m_roadRectanglesGrid; // Rectangles of the road for attacking
	std::vector<sf::RectangleShape> m_grassRectanglesGrid;
	std::map<size_t, bool> m_usedRectanglesIndex;

	std::shared_ptr<Entity> m_player; // Pointer to the player entity
	
	sf::Clock m_clock;			// Clock for calculating the enemy boss spawn rate
	sf::Clock m_weatherClock;	// Clock for moving the clouds
	sf::Clock m_nightClock;		// Clock for the day/night time

public:
	Scene_Play(GameEngine* gameEngine, const std::string& levelPath);

	// Check which is public or private smh
	void init(const std::string & path);
	void update();

	void onEnd();

	// Systems
	void sAnimation();
	void sMovement();
	void sEnemySpawner();
	void sSpawnEnemy(size_t line);
	void sCollision();
	void sRender();
	void sShop();
	void sHealth();
	void sDoAction(const Action& a);
	void sPlacement();
	void spawnPlayer();
	void sUpgrade();
	void spawnBarricade(const Vec2&, size_t);
	void sInfo();

	void attack(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	void generateRoadRectangles();
	void generateGrassRectangles();

	void spawnSpikes(const std::string& type, const Vec2& pos);
};