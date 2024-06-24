#pragma once
#include "Scene.h"
#include <vector>
#include <SFML/Graphics.hpp>

class Scene_Play : public Scene {
protected:
	sf::Text m_gridText;
	std::string m_levelPath;

	bool m_drawTextures = true; // toggle textures
	bool m_drawCollision = false; // toggle hitboxes
	bool m_roadGrid = false; // draw the road grid depending on the type of item selected
	bool m_grassGrid = false; // draw the grass grid depending on the type of item selected
	bool m_mouseItem = false; // if the mouse has an item following it
	bool m_attack = false; // if player has selected an attack item and clicked in a square

	Vec2 m_attackPos = { 0,0 };
	sf::RectangleShape m_attackSquare;

	Vec2 m_gridSize = { 64,64 }; // size of the grid

	size_t m_currentFrame = 0; // Current frame
	size_t m_selectedItem = 0; // Selected item

	std::vector<sf::RectangleShape> m_shopRectangles; // Rectangles of the shop for the items
	std::vector<sf::RectangleShape> m_roadRectanglesGrid; // Rectangles of the road for attacking

	std::shared_ptr<Entity> m_player; // Pointer to the player entity
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
	void sGrid(int n);
	void sDoAction(const Action& a);
	void sPlacement();
	void spawnPlayer();
	void spawnBullet(std::shared_ptr<Entity> entity);

	// LOAD FROM FILE EACH BLOCK POSITION ETC
	void loadLevel(const std::string & levelpath);

	Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
	void attack(std::shared_ptr<Entity> enemy, std::shared_ptr<Entity> tower);
	void generateRoadRectangles();
};