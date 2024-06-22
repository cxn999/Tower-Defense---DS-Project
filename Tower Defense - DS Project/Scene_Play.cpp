#include "Scene_Play.h"
#include "Scene_Menu.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <time.h>

Scene_Play::Scene_Play(GameEngine* gameEngine, const std::string& levelPath) 
	: Scene(gameEngine)
	, m_levelPath(levelPath) 
{
	srand(time(NULL));
	init(m_levelPath);
}

void Scene_Play::init(const std::string& levelPath) {
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE"); // Toggle drawing textures
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION"); // Toggle drawing collision boxes
	registerAction(sf::Keyboard::G, "TOGGLE_GRID");
	registerAction(sf::Keyboard::X, "UPGRADE");

	// TODO: REGISTER ALL OTHER GAMEPLAY ACTIONS

	m_gridText.setCharacterSize(12);
	// m_gridText.setFont(m_game->getAssets().getFont("NAMEFONT"))
	loadLevel(levelPath);
}

void Scene_Play::sDoAction(const Action& action) {
	if (action.type() == "START") {
		if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
		else if (action.name() == "PAUSE") { setPaused(!m_paused); }
		else if (action.name() == "QUIT") { onEnd(); }
		else if (action.name() == "UPGRADE") {
			if (m_player->getComponent<CLevel>().level < 4 && m_player->getComponent<CState>().state != "upgrade") {
				m_player->getComponent<CLevel>().level++;
				m_player->getComponent<CState>().state = "upgrade";
				m_player->getComponent<CHealth>().health += 200;
			}
		}
		// ADD REMAINING ACTIONS
	}
	else if (action.type() == "END") {
	}
}

void Scene_Play::update() {
	m_entityManager.update();
	if (!m_paused) {
		sCollision();
		sMovement();
		sEnemySpawner();
		sAnimation();
	}
	sRender();
	m_currentFrame++;
}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity) {
	// Bullet should go in direction player is facing
}

void Scene_Play::spawnPlayer() {
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CAnimation>(m_game->getAssets().getAnimation("towerIdle1"), false);
	m_player->getComponent<CAnimation>().animation.getSprite().setScale(2, 2);
	auto size = m_player->getComponent<CAnimation>().animation.getSprite().getGlobalBounds();
	m_player->addComponent<CBoundingBox>(Vec2(size.getSize().x/2.f, size.getSize().y/2.f - 10));
	m_player->addComponent<CTransform>(Vec2(m_game->window().getSize().x / 2.f, m_game->window().getSize().y / 2.f));

	m_player->addComponent<CState>();
	m_player->getComponent<CState>().state = "idle1";
	m_player->addComponent<CLevel>();
	m_player->addComponent<CHealth>(300);
}

void Scene_Play::loadLevel(const std::string& levelpath) {
	// reset the entity manager every time we load a level
	m_entityManager = EntityManager();

	std::ifstream file(levelpath);
	std::string identifier;

	if (!file.is_open()) {
		std::cerr << "Error opening file...\n";
		exit(EXIT_FAILURE);
	}

	while (file >> identifier) {
		std::string name;
		int x, y;;
		if (identifier == "tile") {
			file >> name >> x >> y;
			auto e = m_entityManager.addEntity(identifier);
			e->addComponent<CAnimation>(m_game->getAssets().getAnimation(name), false);
			e->getComponent<CAnimation>().animation.getSprite().setScale(2, 2);
			auto size = e->getComponent<CAnimation>().animation.getSprite().getGlobalBounds();
			//e->addComponent<CBoundingBox>(Vec2(size.getSize().x,size.getSize().y));
			e->addComponent<CTransform>(Vec2(x*m_gridSize.x+m_gridSize.x/2,m_game->window().getSize().y-y*m_gridSize.y-m_gridSize.y / 2));
			auto pos = e->getComponent<CTransform>().pos;
			e->getComponent<CAnimation>().animation.getSprite().setPosition(pos.x, pos.y);
		}
	}

	spawnPlayer();
}

void Scene_Play::sRender() {
	m_game->window().clear(sf::Color(166, 176, 79));
	m_player->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(255, 255, 255));
	if (m_drawTextures) {
		for (auto e : m_entityManager.getEntities()) {
			if (e->tag() != "player") {
				m_game->window().draw(e->getComponent<CAnimation>().animation.getSprite());
				if (e->hasComponent<CHealth>() && e->tag() == "enemy") {
					sf::RectangleShape rect(sf::Vector2f(70.f, 8.f));
					rect.setFillColor(sf::Color(255,0,0,180));
					rect.setOutlineThickness(3.f);
					rect.setOutlineColor(sf::Color(33,31,31,120));
					rect.setOrigin(rect.getSize().x / 2.f, rect.getSize().y / 2.f);
					auto& e_pos = e->getComponent<CTransform>().pos;
					auto& e_size = e->getComponent<CBoundingBox>().size;
					rect.setPosition(e_pos.x, e_pos.y-e_size.y/1.5f);
					m_game->window().draw(rect);
				}
				if (e->getComponent<CState>().state == "attack" && e->getComponent<CAnimation>().animation.hasEnded()) {
					m_player->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(255, 0, 0,200));
				}
			}
		}
	}
	
	m_game->window().draw(m_player->getComponent<CAnimation>().animation.getSprite());

	if (m_drawCollision) {
		for (auto e : m_entityManager.getEntities()) {
			if (e->hasComponent<CBoundingBox>()) {
				auto size_x = e->getComponent<CBoundingBox>().size.x;
				auto size_y = e->getComponent<CBoundingBox>().size.y;
				sf::RectangleShape BoundingBox(sf::Vector2f(size_x, size_y));
				BoundingBox.setOrigin(e->getComponent<CBoundingBox>().halfSize.x, e->getComponent<CBoundingBox>().halfSize.y);
				BoundingBox.setPosition(e->getComponent<CTransform>().pos.x, e->getComponent<CTransform>().pos.y);
				BoundingBox.setOutlineColor(sf::Color::Blue);
				BoundingBox.setFillColor(sf::Color::Transparent);
				BoundingBox.setOutlineThickness(1);
				m_game->window().draw(BoundingBox);
			}
		}
	}
	if (m_drawGrid) {
		int m = m_game->window().getSize().x / 128;
		int n = m_game->window().getSize().y / 128;
		for (int i = 1; i<n; i++) {
			sf::RectangleShape linex(sf::Vector2f(m_game->window().getSize().x, 1));
			linex.setFillColor(sf::Color::White);
			linex.setPosition(0, 128*i);
			m_game->window().draw(linex);
		}
		for (int i = 1; i < m; i++) {
			sf::RectangleShape liney(sf::Vector2f(m_game->window().getSize().y, 1));
			liney.rotate(90);
			liney.setFillColor(sf::Color::White);
			liney.setPosition(128*i, 0);
			m_game->window().draw(liney);
		}
	}

	m_game->window().display();
}

void Scene_Play::sAnimation() {
	auto& player = m_player->getComponent<CTransform>();
	auto& player_state = m_player->getComponent<CState>().state;
	auto& animation = m_player->getComponent<CAnimation>().animation;
	
	std::cout << m_player->getComponent<CHealth>().health << std::endl;

	if (player_state == "idle1") {
		if (animation.getName() != "towerIdle1") {
			animation = m_game->getAssets().getAnimation("towerIdle1");
		}
	}
	if (player_state == "idle2") {
		if (animation.getName() != "towerIdle2") {
			animation = m_game->getAssets().getAnimation("towerIdle2");
		}
	}
	if (player_state == "idle3") {
		if (animation.getName() != "towerIdle3") {
			animation = m_game->getAssets().getAnimation("towerIdle3");
		}
	}
	if (player_state == "idle4") {
		if (animation.getName() != "towerIdle4") {
			animation = m_game->getAssets().getAnimation("towerIdle4");
		}
	}
	else if (player_state == "upgrade") {
		if (animation.getName() == "towerIdle1") {
			animation = m_game->getAssets().getAnimation("towerUp1");
		}
		if (animation.getName() == "towerIdle2") {
			animation = m_game->getAssets().getAnimation("towerUp2");
		}
		if (animation.getName() == "towerIdle3") {
			animation = m_game->getAssets().getAnimation("towerUp3");
		}
		if (animation.getName() == "towerUp1" && animation.hasEnded()) {
			player_state = "idle2";
		}
		if (animation.getName() == "towerUp2" && animation.hasEnded()) {
			player_state = "idle3";
		}
		if (animation.getName() == "towerUp3" && animation.hasEnded()) {
			player_state = "idle4";
		}
	}

	animation.getSprite().setScale(2, 2);
	m_player->getComponent<CAnimation>().animation.getSprite().setPosition(player.pos.x, player.pos.y);
	m_player->getComponent<CAnimation>().animation.update();


	for (auto& e : m_entityManager.getEntities("enemy")) {
		auto& e_transform = e->getComponent<CTransform>();
		auto& e_state = e->getComponent<CState>().state;
		auto& animation = e->getComponent<CAnimation>().animation;

		if (e_state == "attack") {
			if (animation.getName() == "D_goblinWalk") animation = m_game->getAssets().getAnimation("D_goblinAttack");
			if (animation.getName() == "S_goblinWalk") animation = m_game->getAssets().getAnimation("S_goblinAttack");
			if (animation.getName() == "D_wolfWalk") animation = m_game->getAssets().getAnimation("D_wolfAttack");
			if (animation.getName() == "S_wolfWalk") animation = m_game->getAssets().getAnimation("S_wolfAttack");
		}

		animation.getSprite().setScale(2, 2);

		if (e_transform.pos.x > e_transform.prevPos.x && animation.getSprite().getScale().x > 0) {
			animation.getSprite().scale(-1, 1);
		}

		e->getComponent<CAnimation>().animation.getSprite().setPosition(e_transform.pos.x, e_transform.pos.y);
		e->getComponent<CAnimation>().animation.update();
	}
}

void Scene_Play::attack(std::shared_ptr<Entity> enemy, std::shared_ptr<Entity> tower) {
	if (enemy->hasComponent<CAttack>()) {
		tower->getComponent<CHealth>().health -= enemy->getComponent<CAttack>().damage;
		if (tower->getComponent<CHealth>().health < 0) {
			tower->destroy();
			setPaused(true);
		}
	}
}

void Scene_Play::sCollision() {
	auto& player = m_player->getComponent<CTransform>();
	for (auto& e : m_entityManager.getEntities("enemy")) {
		auto& tile = e->getComponent<CTransform>();

		Vec2 overlap = GetOverlap(e, m_player);
		Vec2 prevOverlap = GetPreviousOverlap(e, m_player);

		float dy = tile.pos.y - player.pos.y;

		// check if player hits the tile from the bottom
		if (0 < overlap.x && -m_gridSize.y < overlap.y && dy < 0) {
			if (0 <= overlap.y && prevOverlap.y <= 0) {
				tile.pos.y += overlap.y;
				tile.velocity.y = 0;
				e->getComponent<CState>().state = "attack";
			}
		}
		// check player and enemy side collide
		float dx = tile.pos.x - player.pos.x;
		if (0 < overlap.y && -m_gridSize.x < overlap.x) {
			if (0 <= overlap.x && prevOverlap.x <= 0) {
				if (dx > 0) {
					// tile is right of player
					tile.pos.x += overlap.x;
					e->getComponent<CState>().state = "attack";
					attack(e, m_player);
				}
				else {
					// tile is left of player
					tile.pos.x -= overlap.x;
					e->getComponent<CState>().state = "attack";
				}
			}
		}
	}
}

void Scene_Play::sMovement() {
	for (auto& e : m_entityManager.getEntities("enemy")) {
		auto & e_transform = e->getComponent<CTransform>();
		e_transform.prevPos = e_transform.pos;
		e_transform.pos += e_transform.velocity;
		e->getComponent<CAnimation>().animation.getSprite().setPosition(e_transform.pos.x,e_transform.pos.y);
		if (e->getComponent<CState>().state == "attack" && e->getComponent<CAnimation>().animation.hasEnded()) {
			attack(e, m_player);
		}
	}
}

void Scene_Play::onEnd() {
	m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game), true);
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity) {
	return Vec2(0, 0);
}

void Scene_Play::sEnemySpawner() {
	if (m_currentFrame % 200 == 0) {
		sSpawnEnemy(rand()%3+1);
	}
}

void Scene_Play::sSpawnEnemy(size_t line) {
	// create enemy with "enemy" tag
	auto entity = m_entityManager.addEntity("enemy");

	std::string animation;
	Vec2 pos, velocity;
	auto type = rand() % 4;
	float damage;

	if (line == 1) {
		pos = { -100,484 };
		velocity = { 2,0 };
	}
	else if (line == 2) {
		pos = { 894, -100 };
		velocity = { 0, 2 };
	}
	else {
		pos = { 1900, 484 };
		velocity = { -2, 0 };
	}

	if (type == 0) {
		damage = 0.8f;
		velocity *= 0.8;
		if (line == 2) {
			animation = "D_goblinWalk";
		}
		else {
			animation = "S_goblinWalk";
		}
	}
	else if (type == 1) {
		damage = 0.6f;
		velocity*=1.5;
		if (line == 2) {
			animation = "D_wolfWalk";
		}
		else {
			animation = "S_wolfWalk";
		}
	}
	else if (type == 2) {
		damage = 0.3f;
		animation = "D_beeWalk";
		if (line == 2) {
			animation = "D_beeWalk";
		}
		else {
			animation = "S_beeWalk";
		}
	}
	else if (type == 3) {
		damage = 0.2f;
		animation = "D_slimeWalk";
		if (line == 2) {
			animation = "D_slimeWalk";
		}
		else {
			animation = "S_slimeWalk";
		}
	}

	entity->addComponent<CAnimation>(m_game->getAssets().getAnimation(animation), false);
	entity->getComponent<CAnimation>().animation.getSprite().setScale(2, 2);
	auto size = entity->getComponent<CAnimation>().animation.getSprite().getGlobalBounds();
	entity->addComponent<CBoundingBox>(Vec2(size.getSize().x, size.getSize().y/2.f));
	entity->addComponent<CTransform>(pos);
	entity->getComponent<CTransform>().velocity = velocity;
	entity->addComponent<CState>();
	entity->getComponent<CState>().state = "walk";
	entity->addComponent<CHealth>(60);
	entity->addComponent<CAttack>(damage);
}