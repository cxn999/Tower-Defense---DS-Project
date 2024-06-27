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
#include <cmath>

Scene_Play::Scene_Play(GameEngine* gameEngine, const std::string& levelPath) 
	: Scene(gameEngine)
	, m_levelPath(levelPath) 
{
	srand(time(NULL));
	init(m_levelPath);
}

void Scene_Play::generateRoadRectangles() {
	for (int i = 64; i < m_game->window().getSize().x - 128; i += 128) {
		if (i != 128 * 6 + 64) {
			sf::RectangleShape roadRect(sf::Vector2f(128, 128));
			roadRect.setPosition(i, m_game->window().getSize().y / 2.f);
			roadRect.setOutlineThickness(1.f);
			roadRect.setOutlineColor(sf::Color::Magenta);
			roadRect.setFillColor(sf::Color(219, 116, 209, 80));
			m_roadRectanglesGrid.push_back(roadRect);
		}
	}
	for (int i = 64; i < m_game->window().getSize().y / 2.f; i += 128) {
		sf::RectangleShape roadRect(sf::Vector2f(128, 128));
		roadRect.setPosition(m_game->window().getSize().x / 2.f - 64, i);
		roadRect.setOutlineThickness(1.f);
		roadRect.setOutlineColor(sf::Color::Magenta);
		roadRect.setFillColor(sf::Color(219, 116, 209, 80));
		m_roadRectanglesGrid.push_back(roadRect);
	}
}

void Scene_Play::generateGrassRectangles() {
	int n = 0;
	for (int i = 0; i < m_game->window().getSize().x; i += 128) {
		if (i != 6 * 128 && i != 7 * 128) {
			sf::RectangleShape grassRect(sf::Vector2f(128, 128));
			grassRect.setPosition(i, 128 * 2);
			grassRect.setOutlineThickness(1.f);
			grassRect.setOutlineColor(sf::Color::Magenta);
			grassRect.setFillColor(sf::Color(219, 116, 209, 80));
			m_grassRectanglesGrid.push_back(grassRect);
			m_usedRectanglesIndex[n] = false;
		}
		n++;
	}
	for (int i = 0; i < m_game->window().getSize().y / 2.f - 128*2; i += 128) {
		sf::RectangleShape grassRect(sf::Vector2f(128, 128));
		grassRect.setPosition(128*5, i);
		grassRect.setOutlineThickness(1.f);
		grassRect.setOutlineColor(sf::Color::Magenta);
		grassRect.setFillColor(sf::Color(219, 116, 209, 80));
		m_grassRectanglesGrid.push_back(grassRect);
		grassRect.setPosition(128 * 8, i);
		m_grassRectanglesGrid.push_back(grassRect);
		m_usedRectanglesIndex[n] = false;
		n++;
	}
}

void Scene_Play::init(const std::string& levelPath) {
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE"); // Toggle drawing textures
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION"); // Toggle drawing collision boxes
	registerAction(sf::Keyboard::X, "UPGRADE");
	registerAction(sf::Mouse::Left, "CLICK");


	m_entityManager = EntityManager();
	spawnPlayer();

	for (int i = 0; i < 5; i++) {
		sf::RectangleShape rect(sf::Vector2f(75, 75));
		rect.setPosition(rect.getSize().x * i + 50.f * (i + 1), m_game->window().getSize().y - rect.getSize().y - 70.f);
		rect.setFillColor(sf::Color(0, 0, 0, 120));
		rect.setOutlineThickness(10.f);
		rect.setOutlineColor(sf::Color(0, 0, 0, 180));
		m_shopRectangles.push_back(rect);
	}

	generateRoadRectangles();
	generateGrassRectangles();
}

void Scene_Play::sDoAction(const Action& action) {
	if (action.type() == "START") {
		if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "PAUSE") { setPaused(!m_paused); }
		else if (action.name() == "QUIT") { onEnd(); }
		else if (action.name() == "CLICK") { m_player->getComponent<CInput>().click = true; }
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
		if (action.name() == "CLICK") { m_player->getComponent<CInput>().click = false; }
	}
}

void Scene_Play::update() {

	m_entityManager.update();
	if (!m_paused) {
		sHealth();
		sCollision();
		sMovement();
		sEnemySpawner();
		sPlacement();
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
	m_player->addComponent<CTransform>(Vec2(m_game->window().getSize().x / 2.f, m_game->window().getSize().y / 2.f +50.f));

	m_player->addComponent<CState>("idle1", "default");
	m_player->addComponent<CLevel>();
	m_player->addComponent<CHealth>(600);
	m_player->addComponent<CInput>();
}

void Scene_Play::sRender() {
	auto& window = m_game->window();

	window.clear(sf::Color(166, 176, 79));

	window.draw(m_game->getAssets().getAnimation("background").getSprite());

	if (m_drawTextures) {
		for (auto e : m_entityManager.getEntities()) {
			e->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(255, 255, 255));
			if (e->getComponent<CHealth>().prevHealth > e->getComponent<CHealth>().health) 
				e->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(255, 0, 0, 200));
			window.draw(e->getComponent<CAnimation>().animation.getSprite());
		}
	}
	window.draw(m_player->getComponent<CAnimation>().animation.getSprite());

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
				window.draw(BoundingBox);
			}
		}
	}
	if (m_grassGrid && !m_paused) {
		int n = 0;
		for (auto& grassRect : m_grassRectanglesGrid) {
			if (m_usedRectanglesIndex[n] == true) {
				grassRect.setFillColor(sf::Color(255, 0, 0, 80));
				grassRect.setOutlineColor(sf::Color::Red);
			}
			window.draw(grassRect);
			n++;
		}
	}
	if (m_roadGrid && !m_paused) {
		for (auto& roadRect : m_roadRectanglesGrid) {
			window.draw(roadRect);
		}
	}
	
	sShop();

	if (m_mouseItem) {
		auto mouse_pos = sf::Mouse::getPosition(m_game->window());
		auto& item = m_game->getAssets().getAnimation("");
		switch (m_selectedItem) {
		case 0:
			item = m_game->getAssets().getAnimation("shopAreaTower");
			break;
		case 1:
			item = m_game->getAssets().getAnimation("shopFreezeTower");
			break;
		case 2:
			item = m_game->getAssets().getAnimation("shopTargetTower");

			break;
		case 3:
			item = m_game->getAssets().getAnimation("woodSpikesShop");
			break;
		case 4:
			item = m_game->getAssets().getAnimation("lightningShop");
			break;
		}

		if (m_selectedItem < 3) {
			sf::CircleShape c(300);
			c.setFillColor(sf::Color(219, 116, 209, 80));
			c.setOutlineThickness(1.f);
			c.setOutlineColor(sf::Color::Magenta);
			c.setPosition(mouse_pos.x, mouse_pos.y);
			c.setOrigin(c.getRadius(), c.getRadius());
			window.draw(c);
		}

		item.getSprite().setPosition(mouse_pos.x, mouse_pos.y);
		if (m_selectedItem != 4) item.getSprite().setScale(1.5, 1.5);
		else					 item.getSprite().setScale(0.75, 0.75);
		window.draw(item.getSprite());
	}

	window.display();
}

void Scene_Play::sAnimation() {
	auto& player = m_player->getComponent<CTransform>();
	auto& player_state = m_player->getComponent<CState>().state;
	auto& animation = m_player->getComponent<CAnimation>().animation;
	
	if (player_state == "idle1" && (animation.getName().find("Idle1") == std::string::npos))
			animation = m_game->getAssets().getAnimation("towerIdle1");

	if (player_state == "idle2" && (animation.getName().find("Idle2") == std::string::npos))
			animation = m_game->getAssets().getAnimation("towerIdle2");

	if (player_state == "idle3" && (animation.getName().find("Idle3") == std::string::npos))
			animation = m_game->getAssets().getAnimation("towerIdle3");

	if (player_state == "idle4" && (animation.getName().find("Idle4") == std::string::npos))
			animation = m_game->getAssets().getAnimation("towerIdle4");

	if (player_state == "upgrade") {
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
	m_player->getComponent<CAnimation>().animation.getSprite().setPosition(player.pos.x, player.pos.y-50.f);
	m_player->getComponent<CAnimation>().animation.update();


	for (auto& e : m_entityManager.getEntities("enemy")) {
		auto& e_transform = e->getComponent<CTransform>();
		auto& e_state = e->getComponent<CState>().state;
		auto& animation = e->getComponent<CAnimation>().animation;
		auto type = e->getComponent<CType>().type;
		auto direction = e->getComponent<CState>().direction;

		if (e_state == "attack" && (animation.getName().find("Attack") == std::string::npos)) {
			if (type == "goblin") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_goblinAttack");
				else animation = m_game->getAssets().getAnimation("S_goblinAttack");
			}
			else if (type == "wolf") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_wolfAttack");
				else animation = m_game->getAssets().getAnimation("S_wolfAttack");
			}
			else if (type == "slime") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_slimeAttack");
				else animation = m_game->getAssets().getAnimation("S_slimeAttack");
			}
		}
		if (e_state == "death" && (animation.getName().find("Death") == std::string::npos)) {
			if (type == "goblin") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_goblinDeath");
				else animation = m_game->getAssets().getAnimation("S_goblinDeath");
			}
			if (type == "wolf") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_wolfDeath");
				else animation = m_game->getAssets().getAnimation("S_wolfDeath");
			}
			if (type == "slime") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_slimeDeath");
				else animation = m_game->getAssets().getAnimation("S_slimeDeath");
			}
			if (type == "bee") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_beeDeath");
				else animation = m_game->getAssets().getAnimation("S_beeDeath");
			}
			e_transform.velocity = { 0,0 };
		}

		if (e_state == "death" && animation.hasEnded()) {
			e->destroy();
		}

		animation.getSprite().setScale(2, 2);

		if (e_transform.pos.x > e_transform.prevPos.x && animation.getSprite().getScale().x > 0) {
			animation.getSprite().scale(-1, 1);
		}

		e->getComponent<CAnimation>().animation.getSprite().setPosition(e_transform.pos.x, e_transform.pos.y);
		e->getComponent<CAnimation>().animation.update();
	}

	if (m_attack) {
		auto& attacks = m_entityManager.getEntities("attack");
		for (auto& ent : attacks) {
			if (!ent->hasComponent<CAnimation>()) {
				ent->addComponent<CAnimation>();
				if (m_selectedItem == 2) {
					ent->getComponent<CAnimation>().animation = m_game->getAssets().getAnimation("iceSpike");
				}
				else if (m_selectedItem == 3) {
					ent->getComponent<CAnimation>().animation = m_game->getAssets().getAnimation("woodSpike");
				}
				else if (m_selectedItem == 4) {
					ent->getComponent<CAnimation>().animation = m_game->getAssets().getAnimation("lightning");
				}
				else {
					continue;
				}
			}
			else {
				if (ent->getComponent<CAnimation>().animation.hasEnded()) {
					ent->destroy();
					m_attack = false;
				}
			}
			auto& ent_pos = ent->getComponent<CTransform>().pos;
			if (m_selectedItem != 4) {
				ent->getComponent<CAnimation>().animation.getSprite().setScale(2.5, 2.5);
				ent->getComponent<CAnimation>().animation.getSprite().setPosition(ent_pos.x, ent_pos.y);
			}
			else {
				ent->getComponent<CAnimation>().animation.getSprite().setScale(0.75, 0.75);
				ent->getComponent<CAnimation>().animation.getSprite().setPosition(ent_pos.x, ent_pos.y-50);
			}
			ent->getComponent<CAnimation>().animation.update();
		}
	}

	for (auto& defense : m_entityManager.getEntities("defense")) {
		auto& d = defense->getComponent<CAnimation>();
		auto& d_pos = defense->getComponent<CTransform>().pos;
		auto d_type = defense->getComponent<CType>().type;

		if (d.animation.getName() == "constructionTower" && m_lastFrameDefenseSpawn+300==m_currentFrame) {

			if (d_type == "area") {
				d.animation = (m_game->getAssets().getAnimation("upgradeAreaTower1"));
			}
			else if (d_type == "freeze") {
				d.animation = (m_game->getAssets().getAnimation("upgradeFreezeTower1"));
			}
			else if (d_type == "target") {
				d.animation = (m_game->getAssets().getAnimation("upgradeTargetTower1"));
			}
			d.animation.getSprite().setPosition(d_pos.x, d_pos.y);
		}
		if (d.animation.getName().find("upgrade") != std::string::npos && d.animation.hasEnded()) {

			if (d_type == "area") {
				d.animation = (m_game->getAssets().getAnimation("areaTower2"));
			}
			else if (d_type == "freeze") {
				d.animation = (m_game->getAssets().getAnimation("freezeTower2"));
			}
			else if (d_type == "target") {
				d.animation = (m_game->getAssets().getAnimation("targetTower2"));
			}

			d.animation.getSprite().setPosition(d_pos.x, d_pos.y);
			auto archer = m_entityManager.addEntity("archer");

			archer->addComponent<CType>(d_type);
			archer->addComponent<CAnimation>();

			auto type = archer->getComponent<CType>().type;

			if (type == "area") {
				archer->getComponent<CAnimation>().animation = (m_game->getAssets().getAnimation("D_archerAreaIdle"));
			}
			else if (type == "freeze") {
				archer->getComponent<CAnimation>().animation = (m_game->getAssets().getAnimation("D_archerFreezeIdle"));
			}
			else if (type == "target") {
				archer->getComponent<CAnimation>().animation = (m_game->getAssets().getAnimation("D_archerTargetIdle"));
			}

			archer->addComponent<CTransform>(d_pos);
			archer->addComponent<CRange>(300);
			archer->addComponent<CAttack>(3);
			archer->addComponent<CState>("idle", "vertical");
		}
		d.animation.getSprite().setScale(2, 2);		
		d.animation.update();
	}

	for (auto& archer : m_entityManager.getEntities("archer")) {
		auto& d = archer->getComponent<CAnimation>();
		auto& d_pos = archer->getComponent<CTransform>().pos;
		auto type = archer->getComponent<CType>().type;

		if (archer->getComponent<CState>().state == "attack") {
			if (animation.hasEnded()) {
				if (type == "area") {
					d.animation = (m_game->getAssets().getAnimation("D_archerAreaIdle"));
				}
				else if (type == "freeze") {
					d.animation = (m_game->getAssets().getAnimation("D_archerFreezeIdle"));
				}
				else if (type == "target") {
					d.animation = (m_game->getAssets().getAnimation("D_archerTargetIdle"));
				}

				archer->getComponent<CRange>().target = false;
				archer->getComponent<CState>().state = "idle";
			}
			else if (d.animation.getName().find("Attack") == std::string::npos) {

				if (type == "area") {
					d.animation = (m_game->getAssets().getAnimation("D_archerAreaAttack"));
				}
				else if (type == "freeze") {
					d.animation = (m_game->getAssets().getAnimation("D_archerFreezeAttack"));
				}
				else if (type == "target") {
					d.animation = (m_game->getAssets().getAnimation("D_archerTargetAttack"));
				}

			}
		}
		d.animation.getSprite().setPosition(d_pos.x, d_pos.y-20);
		d.animation.getSprite().setScale(1.5, 1.5);
		d.animation.getSprite().setColor(sf::Color(255,0,0));
		d.animation.update();
	}
}

void Scene_Play::attack(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
	if (a->hasComponent<CAttack>()) {
		b->getComponent<CHealth>().health -= a->getComponent<CAttack>().damage;
		if (b->getComponent<CHealth>().health < 0) {
			// tower->destroy();
			// setPaused(true);
		}
	}
}

void Scene_Play::sCollision() {
	// a little bit important
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
		auto entity_bounds = e->getComponent<CAnimation>().animation.getSprite().getGlobalBounds();
		if (m_attack && m_attackSquare.getGlobalBounds().intersects(entity_bounds)) {
			// Depending on attack subtract less or more life
			e->getComponent<CHealth>().health -= 2;
		}
		if (e->getComponent<CHealth>().health <= 0) {
			e->getComponent<CState>().state = "death";
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
		for (auto& archer : m_entityManager.getEntities("archer")) {
			auto& archer_pos = archer->getComponent<CTransform>().pos;
			auto r = archer->getComponent<CRange>().radius;
			if (archer_pos.dist(e_transform.pos) <= r && archer->getComponent<CRange>().target == false) {
				attack(archer, e);
				archer->getComponent<CRange>().target = true;
				archer->getComponent<CState>().state = "attack";
			}
		}

	}
}

void Scene_Play::onEnd() {
	m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game), true);
}

void Scene_Play::sEnemySpawner() {
	if (m_currentFrame%20==0) {
		sSpawnEnemy(rand()%3+1);
	}
}

void Scene_Play::sSpawnEnemy(size_t line) {
	// create enemy with "enemy" tag
	auto entity = m_entityManager.addEntity("enemy");
	int offsetConstant = 50;

	std::string animation;
	Vec2 pos, velocity;
	auto type = rand() % 4;
	float damage;
	size_t health = 0;

	auto window_size = m_game->window().getSize();

	if (line == 1) {
		pos = Vec2(-offsetConstant, rand() % offsetConstant*2 + window_size.y / 2.f);
		velocity = { 2,0 };
		entity->addComponent<CState>("walk", "horizontal");
	}
	else if (line == 2) {
		pos = Vec2(window_size.x / 2.f + rand() % offsetConstant*2 -50 , -offsetConstant);
		velocity = { 0, 2 };
		entity->addComponent<CState>("walk", "vertical");
	}
	else {
		pos = Vec2(window_size.x + offsetConstant , rand() % offsetConstant*2 + window_size.y / 2.f);
		velocity = { -2, 0 };
		entity->addComponent<CState>("walk", "horizontal");
	}

	if (type == 0) {
		entity->addComponent<CType>("goblin");
		damage = 0.8f;
		velocity *= 0.8;
		health = 80;
		if (line == 2) {
			animation = "D_goblinWalk";
		}
		else {
			animation = "S_goblinWalk";
		}
	}
	else if (type == 1) {
		entity->addComponent<CType>("wolf");
		damage = 0.6f;
		velocity*=1.5;
		health = 50;
		if (line == 2) {
			animation = "D_wolfWalk";
		}
		else {
			animation = "S_wolfWalk";
		}
	}
	else if (type == 2) {
		entity->addComponent<CType>("bee");
		damage = 0.3f;
		animation = "D_beeWalk";
		health = 30;
		if (line == 2) {
			animation = "D_beeWalk";
		}
		else {
			animation = "S_beeWalk";
		}
	}
	else if (type == 3) {
		entity->addComponent<CType>("slime");
		damage = 0.2f;
		health = 40;
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
	entity->addComponent<CHealth>(health);
	entity->addComponent<CAttack>(damage);
}

void Scene_Play::sShop() {
	int i = 0;
	for (auto & rect : m_shopRectangles) {
		bool& click = m_player->getComponent<CInput>().click;
		auto mouse_pos = sf::Mouse::getPosition(m_game->window());
		if (click && rect.getGlobalBounds().contains(mouse_pos.x, mouse_pos.y) && !m_mouseItem && !m_attack) {
			m_selectedItem = i;

			if (m_selectedItem < 3 && m_lastFrameDefenseSpawn + 305 > m_currentFrame && m_lastFrameDefenseSpawn != 0) continue;

			click = false;
			m_mouseItem = true;
			
			if (i == 0 || i==1 || i==2) {
				m_grassGrid = true;
			} else {
				m_roadGrid = true;
			}
		}
		i++;
	}
}

void Scene_Play::sPlacement() {
	bool& click = m_player->getComponent<CInput>().click;
	auto mouse_pos = sf::Mouse::getPosition(m_game->window());
	if (m_roadGrid) {
		for (auto& roadRect : m_roadRectanglesGrid) {
			if (click && roadRect.getGlobalBounds().contains(mouse_pos.x, mouse_pos.y)) {
				click = false;
				m_mouseItem = false;
				m_roadGrid = false;

				m_attack = true;

				m_attackPos = { roadRect.getPosition().x + roadRect.getSize().x / 2.f,
								roadRect.getPosition().y + roadRect.getSize().y / 2.f };

				m_attackSquare = roadRect;

				auto attack = m_entityManager.addEntity("attack");
				attack->addComponent<CTransform>(m_attackPos);
			}
		}
	}
	else if (m_grassGrid) {
		int i = 0;
		for (auto& grassRect : m_grassRectanglesGrid) {
			if (click && grassRect.getGlobalBounds().contains(mouse_pos.x, mouse_pos.y)) {

				if (m_usedRectanglesIndex[i] == true) continue;

				click = false;
				m_mouseItem = false;
				m_grassGrid = false;
				m_usedRectanglesIndex[i] = true;

				Vec2 m_defensePos = { grassRect.getPosition().x + grassRect.getSize().x / 2.f,
								grassRect.getPosition().y + grassRect.getSize().y / 2.f };

				m_defenseSquare = grassRect;
				m_lastFrameDefenseSpawn = m_currentFrame;

				auto defense = m_entityManager.addEntity("defense");
				defense->addComponent<CTransform>(m_defensePos);
				defense->addComponent<CAnimation>(m_game->getAssets().getAnimation("constructionTower"), false);
				defense->getComponent<CAnimation>().animation.getSprite().setScale(2, 2);
				defense->getComponent<CAnimation>().animation.getSprite().setPosition(m_defensePos.x,m_defensePos.y);

				std::string type;

				if (m_selectedItem == 0) {
					type = "area";
				}
				else if (m_selectedItem == 1) {
					type = "freeze";
				}
				else if (m_selectedItem == 2) {
					type = "target";
				}

				defense->addComponent<CType>().type = type;
			}
			i++;
		}
	}
}

void Scene_Play::sHealth() {
	for (auto& e : m_entityManager.getEntities()) {
		if (e->hasComponent<CHealth>()) {
			e->getComponent<CHealth>().prevHealth = e->getComponent<CHealth>().health;
		}
	}
}