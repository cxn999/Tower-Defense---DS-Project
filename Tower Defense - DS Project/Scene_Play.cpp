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
	registerAction(sf::Mouse::Right, "UPGRADE");
	registerAction(sf::Mouse::Left, "CLICK");

	spawnPlayer();

	for (int i = 0; i < 3; i++) {
		sf::RectangleShape rect(sf::Vector2f(36, 36));
		rect.setPosition(rect.getSize().x * i + 28.f * (i + 1) + 22.f, m_game->window().getSize().y - rect.getSize().y - 70.f);
		rect.setFillColor(sf::Color::Transparent);
		rect.setOutlineThickness(10.f);
		rect.setOutlineColor(sf::Color(0, 0, 0, 180));
		rect.move(sf::Vector2f(0, -90));
		m_shopRectangles.push_back(rect);
	}
	for (int i = 0; i < 3; i++) {
		sf::RectangleShape rect(sf::Vector2f(36, 36));
		rect.setPosition(rect.getSize().x * i + 28.f * (i + 1) + 22.f, m_game->window().getSize().y - rect.getSize().y - 70.f);
		rect.setFillColor(sf::Color::Transparent);
		rect.setOutlineThickness(10.f);
		rect.setOutlineColor(sf::Color(0, 0, 0, 180));
		m_shopRectangles.push_back(rect);
	}

	m_coinsText.setFont(m_game->getAssets().getFont("RETROGAMING"));
	m_coinsText.setOutlineColor(sf::Color::Black);
	m_coinsText.setOutlineThickness(4.f);

	generateRoadRectangles();
	generateGrassRectangles();

	m_nightFilter.setSize(sf::Vector2f(m_game->window().getSize().x, m_game->window().getSize().y * 0.72));
	m_nightFilter.setFillColor(sf::Color(0, 0, 0, 0));
}

void Scene_Play::sDoAction(const Action& action) {
	if (action.type() == "START") {
		if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "PAUSE") { 
			setPaused(!m_paused); 
			if (m_pauseP)
				m_pauseP = false;
			else
				m_pauseP = true;
		}
		else if (action.name() == "QUIT") { onEnd(); }
		else if (action.name() == "CLICK") { m_player->getComponent<CInput>().click = true; }
		else if (action.name() == "UPGRADE") { m_player->getComponent<CInput>().rightClick = true; }
		// ADD REMAINING ACTIONS
	}
	else if (action.type() == "END") {
		if (action.name() == "CLICK") { m_player->getComponent<CInput>().click = false; }
		else if (action.name() == "UPGRADE") { m_player->getComponent<CInput>().rightClick = false; }
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
		sInfo();
		sUpgrade();
	}
	sRender();
	m_currentFrame++;
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

void Scene_Play::spawnBarricade(const Vec2& position, size_t line) {
	auto m_barricade = m_entityManager.addEntity("barricade");
	m_barricade->addComponent<CState>();
	m_barricade->getComponent<CState>().state = "build";
	auto& barricadeDirection = m_barricade->getComponent<CState>().direction; 
	
	size_t offset = 15;

	if (line == 1) {
		m_barricade->addComponent<CAnimation>(m_game->getAssets().getAnimation("S_barricadeBuild"), false);
		barricadeDirection = "Left";
		
	}
	else if (line == 2) {
		m_barricade->addComponent<CAnimation>(m_game->getAssets().getAnimation("U_barricadeBuild"), false);
		barricadeDirection = "Up";
		offset = 0;
	}
	else {
		m_barricade->addComponent<CAnimation>(m_game->getAssets().getAnimation("S_barricadeBuild"), false);
		barricadeDirection = "Right";
	}
	
	m_barricade->getComponent<CAnimation>().animation.getSprite().setScale(3, 3);
	auto size = m_barricade->getComponent<CAnimation>().animation.getSprite().getGlobalBounds();
	m_barricade->addComponent<CBoundingBox>(Vec2(size.getSize().x / 2.f + 15.f, size.getSize().y / 2.f + 15.f));
	m_barricade->addComponent<CTransform>(Vec2(m_game->window().getSize().x / 2.f, m_game->window().getSize().y / 2.f + 50.f));
	m_barricade->getComponent<CTransform>().pos = { position.x, position.y - offset};
	m_barricade->addComponent<CHealth>(200);
}

void Scene_Play::sRender() {
	auto& window = m_game->window();

	window.clear(sf::Color(166, 176, 79));

	window.draw(m_game->getAssets().getAnimation("background").getSprite());

	if (m_drawTextures) {
		for (auto e : m_entityManager.getEntities()) {
			if (e->tag() == "barricade") continue;
			if (e->getComponent<CType>().type == "bee") continue;
			if (e->tag() != "enemyBoss")
				if (e->getComponent<CState>().effect == "freeze")
				e->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(110, 133, 255, 200));
				else
				e->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(255, 255, 255));
			else if(e->tag() == "enemyBoss")
				if (e->getComponent<CState>().effect == "freeze")
					e->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(110, 133, 255,200));
				else
					e->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(255, 0, 155));
			if (e->hasComponent<CHealth>() && e->getComponent<CHealth>().prevHealth > e->getComponent<CHealth>().health)
				e->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(255, 0, 0, 200));
			window.draw(e->getComponent<CAnimation>().animation.getSprite());
		}

		for (auto e : m_entityManager.getEntities("barricade")) {
			if (e->tag() != "enemyBoss")
					e->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(255, 255, 255));
			if (e->hasComponent<CHealth>() && e->getComponent<CHealth>().prevHealth > e->getComponent<CHealth>().health)
				e->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(255, 0, 0, 200));
			window.draw(e->getComponent<CAnimation>().animation.getSprite());
		}
		for (auto e : m_entityManager.getEntities("enemy")) {
			if (e->getComponent<CType>().type == "bee") {
				if (e->tag() != "enemyBoss")
					if (e->getComponent<CState>().effect == "freeze")
						e->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(110, 133, 255, 200));
					else
						e->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(255, 255, 255));
				else if (e->tag() == "enemyBoss")
					if (e->getComponent<CState>().effect == "freeze")
						e->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(110, 133, 255, 200));
					else
						e->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(255, 0, 155));
				if (e->hasComponent<CHealth>() && e->getComponent<CHealth>().prevHealth > e->getComponent<CHealth>().health)
					e->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(255, 0, 0, 200));
				window.draw(e->getComponent<CAnimation>().animation.getSprite());
			}
		}
	}
	

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
			item = m_game->getAssets().getAnimation("lightningShop");
			break;
		case 5:
			item = m_game->getAssets().getAnimation("U_barricadeIdle");
			break;
		}

		if (m_selectedItem < 3) {
			sf::CircleShape c(265);
			c.setFillColor(sf::Color(219, 116, 209, 80));
			c.setOutlineThickness(1.f);
			c.setOutlineColor(sf::Color::Magenta);
			c.setPosition(mouse_pos.x, mouse_pos.y);
			c.setOrigin(c.getRadius(), c.getRadius());
			window.draw(c);
		}

		item.getSprite().setPosition(mouse_pos.x, mouse_pos.y);
		if (m_selectedItem != 3) item.getSprite().setScale(1.5, 1.5);
		else					 item.getSprite().setScale(0.75, 0.75);
		window.draw(item.getSprite());
	}

	m_coinsText.setString(std::to_string(m_coins));
	m_coinsText.setPosition(380, 790);

	/*
	int array[6] = { 20,30,25,150,30,40 };

	for (int i = 0; i < 3; i++) {
		sf::Text k(std::to_string(array[i]), m_game->getAssets().getFont("RETROGAMING"), 15);
		k.setOutlineColor(sf::Color::Black);
		k.setOutlineThickness(2.f);
		k.setPosition(45 + 65*i, m_game->window().getSize().y - 147.f);
		window.draw(k);
	}
	for (int i = 1; i < 4; i++) {
		sf::Text k(std::to_string(array[i+2]), m_game->getAssets().getFont("RETROGAMING"), 15);
		k.setOutlineColor(sf::Color::Black);
		k.setOutlineThickness(2.f);
		k.setPosition(35 + 70 * (i-1), m_game->window().getSize().y - 57.f);
		window.draw(k);
	}
	*/

	auto health = m_player->getComponent<CHealth>().health;
	auto totalHealth = m_player->getComponent<CHealth>().totalHealth;

	sf::RectangleShape r(sf::Vector2f(482*(health/totalHealth), 35));
	
	r.setFillColor(sf::Color(255, 0, 0, 150));
	r.setPosition(409, 711);
	window.draw(r);

	//Clouds block
	sf::Sprite clouds;
	clouds = m_game->getAssets().getAnimation("clouds").getSprite(); 
	clouds.setOrigin(0.0f, 0.0f);
	clouds.setColor(sf::Color(128, 128, 128, 50)); 
	clouds.setScale(2.7f, 2.5f);
	//clouds movement
	clouds.setPosition(sf::Vector2f(-1800.f + m_weatherClock.getElapsedTime().asSeconds()*10, 0.f));
	window.draw(clouds);



	window.draw(m_coinsText);

	if (m_drawInfo) {
		for (auto& i : m_infoVector) {
			window.draw(i);
		}
	}

	// Day & Night shift block

	float timePassed = m_nightClock.getElapsedTime().asSeconds();

	if (timePassed >= 1.f) {
		if (m_nightFall) {
			if (m_opacity < 155) {
				m_opacity++;
			}
			else {
				m_nightFall = false;
			}
		}
		else {
			if (m_opacity > 0) {
				m_opacity--;
			}
			else {
				m_nightFall = true;
			}
		}
		m_nightFilter.setFillColor(sf::Color(5, 19, 41, m_opacity));
		m_nightClock.restart();
	}
	window.draw(m_nightFilter);

	window.draw(m_player->getComponent<CAnimation>().animation.getSprite());

	 m_helpPauseText = sf::Text("PRESS 'P' TO PAUSE", m_game->getAssets().getFont("RETROGAMING"), 20);
	 m_helpPauseText.setPosition(window.getSize().x * 0.75f, window.getSize().y * 0.92f);
	 m_helpPauseText.setOutlineColor(sf::Color::Black);
	 m_helpPauseText.setOutlineThickness(3.f);
	 window.draw(m_helpPauseText);
	

	 if (!m_player->isActive() || m_paused) {
		 sf::Text replay = sf::Text("Replay: R", m_game->getAssets().getFont("RETROGAMING"), 40);
		 sf::Text quit = sf::Text("Quit: ESC", m_game->getAssets().getFont("RETROGAMING"), 40);
		 sf::Text gameOver = sf::Text("GAME OVER", m_game->getAssets().getFont("RETROGAMING"), 40);
		 gameOver.setFillColor(sf::Color::Red);

		 auto rect = sf::RectangleShape(sf::Vector2f(window.getSize().x, window.getSize().y * 0.29f));
		 rect.setFillColor(sf::Color(0,0,0));
		 rect.setPosition(0.f, window.getSize().y * 0.71f); 

		 replay.setPosition(window.getSize().x * 0.1f, window.getSize().y * 0.83f);
		 gameOver.setPosition(window.getSize().x * 0.4f, window.getSize().y * 0.83f);
		 quit.setPosition(window.getSize().x * 0.7f, window.getSize().y * 0.83f);

		 gameOver.setOutlineThickness(3.f);
		 replay.setOutlineThickness(3.f);
		 quit.setOutlineThickness(3.f);

		 gameOver.setOutlineColor(sf::Color::Black);
		 replay.setOutlineColor(sf::Color::Black);
		 quit.setOutlineColor(sf::Color::Black);

		 window.draw(rect);
		 window.draw(replay);

		 window.draw(gameOver);

		 window.draw(quit);
	}

	if (m_pauseP){
		
		sf::Text replay = sf::Text("Replay: R", m_game->getAssets().getFont("RETROGAMING"), 40); 
		sf::Text quit = sf::Text("Quit: ESC", m_game->getAssets().getFont("RETROGAMING"), 40); 
		m_helpPauseText = sf::Text("Resume: P", m_game->getAssets().getFont("RETROGAMING"), 40);

		auto rect = sf::RectangleShape(sf::Vector2f(window.getSize().x , window.getSize().y * 0.29f)); 
		rect.setFillColor(sf::Color(0,0,0));
		rect.setPosition(0.f, window.getSize().y * 0.71f); 

		replay.setPosition(window.getSize().x * 0.1f, window.getSize().y * 0.83f);
		quit.setPosition(window.getSize().x * 0.4f, window.getSize().y * 0.83f);
		m_helpPauseText.setPosition(window.getSize().x * 0.7f, window.getSize().y * 0.83f);

		m_helpPauseText.setOutlineThickness(3.f);
		replay.setOutlineThickness(3.f);
		quit.setOutlineThickness(3.f);

		m_helpPauseText.setOutlineColor(sf::Color::Black);
		replay.setOutlineColor(sf::Color::Black);
		quit.setOutlineColor(sf::Color::Black);

		window.draw(rect); 
		window.draw(replay); 
		
		window.draw(m_helpPauseText); 

		window.draw(quit); 

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
			else if (type == "bee") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_beeAttack");
				else animation = m_game->getAssets().getAnimation("S_beeAttack");
			}
		}
		if (e_state == "death" && (animation.getName().find("Death") == std::string::npos)) {
			if (type == "goblin") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_goblinDeath");
				else animation = m_game->getAssets().getAnimation("S_goblinDeath");
				m_coins += 7;
			}
			if (type == "wolf") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_wolfDeath");
				else animation = m_game->getAssets().getAnimation("S_wolfDeath");
				m_coins += 6;
			}
			if (type == "slime") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_slimeDeath");
				else animation = m_game->getAssets().getAnimation("S_slimeDeath");
				m_coins += 5;
			}
			if (type == "bee") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_beeDeath");
				else animation = m_game->getAssets().getAnimation("S_beeDeath");
				m_coins += 4;
			}
			e->getComponent<CFocus>().entity = nullptr;
			e_transform.velocity = { 0,0 };
		}
		if (e_state == "idle" && animation.getName().find("Walk") == std::string::npos) {
			if (type == "goblin") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_goblinWalk");
				else animation = m_game->getAssets().getAnimation("S_goblinWalk");
			}
			if (type == "wolf") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_wolfWalk");
				else animation = m_game->getAssets().getAnimation("S_wolfWalk");
			}
			if (type == "slime") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_slimeWalk");
				else animation = m_game->getAssets().getAnimation("S_slimeWalk");
			}
			if (type == "bee") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_beeWalk");
				else animation = m_game->getAssets().getAnimation("S_beeWalk");
			}
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

	for (auto& e : m_entityManager.getEntities("enemyBoss")) {
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
			else if (type == "bee") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_beeAttack");
				else animation = m_game->getAssets().getAnimation("S_beeAttack");
			}
		}
		if (e_state == "death" && (animation.getName().find("Death") == std::string::npos)) {
			if (type == "goblin") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_goblinDeath");
				else animation = m_game->getAssets().getAnimation("S_goblinDeath");
				m_coins += 25;
			}
			if (type == "wolf") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_wolfDeath");
				else animation = m_game->getAssets().getAnimation("S_wolfDeath");
				m_coins += 20;
			}
			if (type == "slime") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_slimeDeath");
				else animation = m_game->getAssets().getAnimation("S_slimeDeath");
				m_coins += 20;
			}
			if (type == "bee") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_beeDeath");
				else animation = m_game->getAssets().getAnimation("S_beeDeath");
				m_coins += 20;
			}
			e->getComponent<CFocus>().entity = nullptr;
			e_transform.velocity = { 0,0 };
		}
		if (e_state == "idle" && animation.getName().find("Walk") == std::string::npos) {
			if (type == "goblin") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_goblinWalk");
				else animation = m_game->getAssets().getAnimation("S_goblinWalk");
			}
			if (type == "wolf") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_wolfWalk");
				else animation = m_game->getAssets().getAnimation("S_wolfWalk");
			}
			if (type == "slime") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_slimeWalk");
				else animation = m_game->getAssets().getAnimation("S_slimeWalk");
			}
			if (type == "bee") {
				if (direction == "vertical") animation = m_game->getAssets().getAnimation("D_beeWalk");
				else animation = m_game->getAssets().getAnimation("S_beeWalk");
			}
		}
		if (e_state == "death" && animation.hasEnded()) {
			e->destroy();
		}

		animation.getSprite().setScale(4, 4);

		if (e_transform.pos.x > e_transform.prevPos.x && animation.getSprite().getScale().x > 0) {
			animation.getSprite().scale(-1, 1);
		}

		e->getComponent<CAnimation>().animation.getSprite().setPosition(e_transform.pos.x, e_transform.pos.y);
		e->getComponent<CAnimation>().animation.update();
	}

	for (auto& attack : m_entityManager.getEntities("attack")) {
		auto & attack_animation = attack->getComponent<CAnimation>().animation;

		if (attack_animation.hasEnded()) {
			attack->destroy();
		}

		auto& attack_pos = attack->getComponent<CTransform>().pos;

		attack_animation.getSprite().setPosition(attack_pos.x, attack_pos.y);
		attack_animation.getSprite().setScale(4, 4);
		attack_animation.update();
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
				archer->addComponent<CDelay>(0, 200);
			}
			else if (type == "freeze") {
				archer->getComponent<CAnimation>().animation = (m_game->getAssets().getAnimation("D_archerFreezeIdle"));
				archer->addComponent<CDelay>(0, 200);
			}
			else if (type == "target") {
				archer->getComponent<CAnimation>().animation = (m_game->getAssets().getAnimation("D_archerTargetIdle"));
			}

			archer->addComponent<CTransform>(d_pos);
			archer->addComponent<CRange>(265);
			archer->addComponent<CAttack>(15);
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
			else if (d.animation.getName().find("Idle") != std::string::npos) {
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

	for (auto& barricade : m_entityManager.getEntities("barricade")) {
		auto& b = barricade->getComponent<CAnimation>();
		auto& b_pos = barricade->getComponent<CTransform>().pos;
		auto& b_state = barricade->getComponent<CState>(); 
		

		
		if (b_state.state == "idle" && b.animation.getName().find("Idle") == std::string::npos) {
			if (b_state.direction == "Up")
			{
				b.animation = m_game->getAssets().getAnimation("U_barricadeIdle");
				b.animation.getSprite().setPosition(b_pos.x, b_pos.y);
			}
			else {
				b.animation = m_game->getAssets().getAnimation("S_barricadeIdle");
				if(b_state.direction == "Left")
					b.animation.getSprite().setPosition(b_pos.x-30.4, b_pos.y);
				else
					b.animation.getSprite().setPosition(b_pos.x+30.4, b_pos.y);
			}
		}
		else if (b.animation.getName().find("Build") != std::string::npos) {
			if (b_state.direction == "Left")
				b.animation.getSprite().setPosition(b_pos.x - 30.4, b_pos.y);
			else if (b_state.direction == "Right")
				b.animation.getSprite().setPosition(b_pos.x + 30.4, b_pos.y);
			else {
				b.animation.getSprite().setPosition(b_pos.x, b_pos.y);
			}
		}
		
		if (b.animation.getName().find("Build") != std::string::npos && b.animation.hasEnded()) {
			if (b_state.direction == "Up")
			{
				b.animation = m_game->getAssets().getAnimation("U_barricadeIdle");
				b.animation.getSprite().setPosition(b_pos.x, b_pos.y);
			}
			else {
				b.animation = m_game->getAssets().getAnimation("S_barricadeIdle");
				if(b_state.direction == "Left")
					b.animation.getSprite().setPosition(b_pos.x-30.4, b_pos.y);
				else
					b.animation.getSprite().setPosition(b_pos.x+30.4, b_pos.y);
			}
		}
		else if (b.animation.getName().find("Build") != std::string::npos) {
			if (b_state.direction == "Left")
				b.animation.getSprite().setPosition(b_pos.x - 30.4, b_pos.y);
			else if (b_state.direction == "Right")
				b.animation.getSprite().setPosition(b_pos.x + 30.4, b_pos.y);
			else {
				b.animation.getSprite().setPosition(b_pos.x, b_pos.y);
			}
		}

		if (b_state.state == "death" && b.animation.getName().find("Destroy") == std::string::npos) {
			if (b_state.direction == "Up")
			{
				b.animation = m_game->getAssets().getAnimation("U_barricadeDestroy");
				b.animation.getSprite().setPosition(b_pos.x, b_pos.y);
			}
			else {
				b.animation = m_game->getAssets().getAnimation("S_barricadeDestroy");
				if (b_state.direction == "Left")
					b.animation.getSprite().setPosition(b_pos.x - 30.4, b_pos.y);
				else
					b.animation.getSprite().setPosition(b_pos.x + 30.4, b_pos.y);
			}
		}
		else if (b.animation.getName().find("Build") != std::string::npos && b.animation.hasEnded()) {
			if (b_state.direction == "Up")
			{
				b.animation = m_game->getAssets().getAnimation("U_barricadeBuild");
				b.animation.getSprite().setPosition(b_pos.x, b_pos.y);
			}
			else {
				b.animation = m_game->getAssets().getAnimation("S_barricadeDestroy");
				if (b_state.direction == "Left")
					b.animation.getSprite().setPosition(b_pos.x - 30.4, b_pos.y);
				else
					b.animation.getSprite().setPosition(b_pos.x + 30.4, b_pos.y);
			}
		}
		if(b_state.direction == "Right")
			b.animation.getSprite().setScale(-3, 3);
		else
			b.animation.getSprite().setScale(3, 3);

		if (b.animation.getName().find("Destroy") != std::string::npos && b.animation.hasEnded()) {
			barricade->destroy();
		}
		b.animation.update();
	}
}

void Scene_Play::attack(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
	if (a->hasComponent<CAttack>()) {
		b->getComponent<CHealth>().health -= a->getComponent<CAttack>().damage;
		if (b->getComponent<CHealth>().health < 0) {
			if (b->tag() == "player") {
				m_paused = true;
			}
		}
	}
}

void Scene_Play::sCollision() {
	auto& player = m_player->getComponent<CTransform>();

	// COLLISIONS OF PLAYER WITH ENEMIES

	// Define the search area around the player
	sf::FloatRect searchArea(player.pos.x - 100, player.pos.y - 100, 200, 200); // Adjust size as needed

	// Query the quadtree for entities within the search area
	auto nearbyEntities = m_entityManager.queryRange(searchArea);

	for (auto& e : nearbyEntities) {
		if (e->tag() != "enemy" && e->tag() != "enemyBoss") continue;

		auto& tile = e->getComponent<CTransform>();

		Vec2 overlap = GetOverlap(e, m_player);
		Vec2 prevOverlap = GetPreviousOverlap(e, m_player);

		float dy = tile.pos.y - player.pos.y;

		// Check if player hits the tile from the bottom
		if (0 < overlap.x && -m_gridSize.y < overlap.y && dy < 0) {
			if (0 <= overlap.y && prevOverlap.y <= 0) {
				tile.move = false;
				tile.pos.y -= overlap.y;
				e->getComponent<CState>().state = "attack";
				e->getComponent<CFocus>().entity = m_player;

			}
		}

		// Check player and enemy side collision
		float dx = tile.pos.x - player.pos.x;
		if (0 < overlap.y && -m_gridSize.x < overlap.x) {
			if (0 <= overlap.x && prevOverlap.x <= 0) {
				if (dx > 0) {
					// Tile is right of player
					tile.pos.x += overlap.x;
					tile.move = false;
					e->getComponent<CState>().state = "attack";
					e->getComponent<CFocus>().entity = m_player;
				}
				else {
					// Tile is left of player
					tile.pos.x -= overlap.x;
					tile.move = false;
					e->getComponent<CState>().state = "attack";
					e->getComponent<CFocus>().entity = m_player;
				}
			}
		}
	}

	// COLLISIONS OF BARRICADES WITH ENEMIES
	
	for (auto& barricade : m_entityManager.getEntities("barricade")) {
		auto& barricadeTransform = barricade->getComponent<CTransform>();

		// Define the search area around the barricade
		sf::FloatRect searchArea(barricadeTransform.pos.x - 100, barricadeTransform.pos.y - 100, 200, 200);		// Adjust size as needed

		// Query the quadtree for entities within the search area
		auto nearbyEntities = m_entityManager.queryRange(searchArea);

		for (auto& e : nearbyEntities) {
			if (e->tag() != "enemy" && e->tag() != "enemyBoss") continue;

			if (e->getComponent<CType>().type == "bee") continue;

			auto& tile = e->getComponent<CTransform>(); 

			Vec2 overlap = GetOverlap(e, barricade);
			Vec2 prevOverlap = GetPreviousOverlap(e, barricade);

			float dy = tile.pos.y - barricadeTransform.pos.y;

			// Check if player hits the tile from the bottom
			if (0 < overlap.x && -m_gridSize.y < overlap.y && dy < 0) { 
				if (0 <= overlap.y && prevOverlap.y <= 0) { 
					tile.move = false;
					tile.pos.y -= overlap.y; 
					e->getComponent<CState>().state = "attack"; 
					e->getComponent<CFocus>().entity = barricade;
				}
			}

			// Check player and enemy side collision
			float dx = tile.pos.x - barricadeTransform.pos.x; 
			if (0 < overlap.y && -m_gridSize.x < overlap.x) {
				if (0 <= overlap.x && prevOverlap.x <= 0) {
					if (dx > 0) {
						// Tile is right of player
						tile.move = false;
						tile.pos.x += overlap.x;
						e->getComponent<CState>().state = "attack";
						e->getComponent<CFocus>().entity = barricade;
					}
					else {
						// Tile is left of player
						tile.pos.x -= overlap.x;
						tile.move = false;
						e->getComponent<CState>().state = "attack";
						e->getComponent<CFocus>().entity = barricade;
					}
				}
			}
		}
		if (barricade->getComponent<CHealth>().health <= 0) {
			barricade->getComponent<CState>().state = "death";
		}
	}
}

void Scene_Play::sMovement() {
	// Update positions of enemies
	for (auto& e : m_entityManager.getEntities("enemy")) {
		auto& e_transform = e->getComponent<CTransform>();

		if (e_transform.move) {
			e_transform.prevPos = e_transform.pos;

			if (e->getComponent<CState>().effect == "freeze") {
				e_transform.pos += e_transform.velocity / 2.f;
			}
			else {
				e_transform.pos += e_transform.velocity;
			}
		}

		
		auto& animation = e->getComponent<CAnimation>().animation;
		animation.getSprite().setPosition(e_transform.pos.x, e_transform.pos.y);
		if (e->getComponent<CState>().state == "attack" && animation.hasEnded()) {
			if (e->getComponent<CFocus>().entity->getComponent<CState>().state == "death") {
				e->getComponent<CFocus>().entity = nullptr;
				e->getComponent<CState>().state = "idle";
				e_transform.move = true;
			}
			else {
				attack(e, e->getComponent<CFocus>().entity);
			}
		}
		if (e->getComponent<CHealth>().health <= 0) {
			e->getComponent<CHealth>().health = 0;
			e->getComponent<CState>().state = "death";
		}

		for (auto& a : m_entityManager.getEntities("attack")) {
			auto& attack_animation = a->getComponent<CAnimation>().animation;

			if (attack_animation.getSprite().getGlobalBounds().contains(e_transform.pos.x, e_transform.pos.y)) {
				if (a->getComponent<CType>().type == "freeze") {
					e->getComponent<CState>().effect = "freeze";
				}
				else if (a->getComponent<CType>().type == "area") {
					attack(a, e);
				}
			}
			if (a->getComponent<CType>().type == "lightning") {
				if (m_lightningSquare.getGlobalBounds().contains(e_transform.pos.x, e_transform.pos.y)) {
					attack(a, e);
				}
			}
		}
	}
	for (auto& e : m_entityManager.getEntities("enemyBoss")) {
		auto& e_transform = e->getComponent<CTransform>();
		e_transform.prevPos = e_transform.pos;
		e_transform.pos += e_transform.velocity;
		auto& animation = e->getComponent<CAnimation>().animation;
		animation.getSprite().setPosition(e_transform.pos.x, e_transform.pos.y);
		if (e->getComponent<CState>().state == "attack" && animation.hasEnded()) {
			attack(e, m_player);
		}
		if (e->getComponent<CHealth>().health <= 0) {
			e->getComponent<CHealth>().health = 0;
			e->getComponent<CState>().state = "death";
		}

		for (auto& a : m_entityManager.getEntities("attack")) {
			auto& attack_animation = a->getComponent<CAnimation>().animation;

			if (attack_animation.getSprite().getGlobalBounds().contains(e_transform.pos.x, e_transform.pos.y)) {
				if (a->getComponent<CType>().type == "freeze") {
					e->getComponent<CState>().effect = "freeze";
					auto & type = e->getComponent<CType>().type;
				}
				else if (a->getComponent<CType>().type == "area") {
					attack(a, e);
				}
			}
			if (a->getComponent<CType>().type == "lightning") {
				if (m_lightningSquare.getGlobalBounds().contains(e_transform.pos.x, e_transform.pos.y)) {
					attack(a, e);
				}
			}
		}
	}

	// Update archers and check for enemies in range
	for (auto& archer : m_entityManager.getEntities("archer")) {
		auto& archer_pos = archer->getComponent<CTransform>().pos;
		auto r = archer->getComponent<CRange>().radius;

		// Define the search area for the quadtree query
		sf::FloatRect searchArea(archer_pos.x - r, archer_pos.y - r, 2 * r, 2 * r);

		// Query the quadtree for enemies within the archer's range
		auto nearbyEnemies = m_entityManager.queryRange(searchArea);
		
		for (auto& e : nearbyEnemies) {
			if (e->tag() != "enemy" && e->tag() != "enemyBoss") continue;

			auto& e_transform = e->getComponent<CTransform>();
			if (archer_pos.dist(e_transform.pos) <= r && !archer->getComponent<CRange>().target) {
				auto & archer_type = archer->getComponent<CType>().type;
				if (archer_type == "target") {
					attack(archer, e);
				}
				else if (archer_type == "freeze") {
					auto& cDelay = archer->getComponent<CDelay>();
					if (cDelay.lastAttackFrame == 0 || cDelay.lastAttackFrame + cDelay.delay <= m_currentFrame) {
						cDelay.lastAttackFrame = m_currentFrame;
						spawnSpikes("freeze", e_transform.pos);
					}
				}
				else if (archer_type == "area") {
					auto& cDelay = archer->getComponent<CDelay>();
					if (cDelay.lastAttackFrame == 0 || cDelay.lastAttackFrame + cDelay.delay <= m_currentFrame) {
						cDelay.lastAttackFrame = m_currentFrame;
						spawnSpikes("area", e_transform.pos);
					}
				}
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
	int t = m_clock.getElapsedTime().asSeconds();
	
	if (m_currentFrame % m_spawnRateFrame == 0) {		//Determines if an enemy spawns or not each n number of frames that gets decreased in the function down below
		sSpawnEnemy(rand() % 3 + 1);	
	}
	if (t >= 1) {
		if (t % 1 == 0 && m_spawnRateFrame >= 15) {		//Each second that passes by, the n of frames needed for an enemy to spawn gets reduced by 2 
			m_spawnRateFrame -= 1;
			m_clock.restart();
		}
	}

	/*if (t < 30.f) {
		if (m_currentFrame % 150 == 0) {
			sSpawnEnemy(rand() % 3 + 1);
		}
	}
	else if (t > 30.f && t < 60.f) {
		if (m_currentFrame % 120 == 0) {
			sSpawnEnemy(rand() % 3 + 1);
		}
	}
	else if (t > 60.f && t < 90.f){
		if (m_currentFrame % 90 == 0) {
			sSpawnEnemy(rand() % 3 + 1);
		}
	}
	else {
		if (m_currentFrame % 60 == 0) {
			sSpawnEnemy(rand() % 3 + 1);
		}
	}*/
}

void Scene_Play::sSpawnEnemy(size_t line) {
	// create enemy with "enemy" tag

	std::string tag;
	if (int(m_clock.getElapsedTime().asSeconds()) == 0 || int(m_clock.getElapsedTime().asSeconds()) % 15 != 0.0f) 
		tag = "enemy";
	else {
		tag = "enemyBoss";

	}
	auto entity = m_entityManager.addEntity(tag); 
	


	int offsetConstant = 50;
	

	std::string animation;
	Vec2 pos, velocity;
	auto type = rand() % 4;
	float damage;
	size_t health = 0;

	auto window_size = m_game->window().getSize();

	if (line == 1) {
		pos = Vec2(-offsetConstant, rand() % offsetConstant * 2 + window_size.y / 2.f);
		velocity = { 2,0 };
		entity->addComponent<CState>("walk", "horizontal");
	}
	else if (line == 2) {
		pos = Vec2(window_size.x / 2.f + rand() % offsetConstant * 2 - 50, -offsetConstant);
		velocity = { 0, 2 };
		entity->addComponent<CState>("walk", "vertical");
	}
	else {
		pos = Vec2(window_size.x + offsetConstant, rand() % offsetConstant * 2 + window_size.y / 2.f);
		velocity = { -2, 0 };
		entity->addComponent<CState>("walk", "horizontal");
	}

	if (int(m_clock.getElapsedTime().asSeconds()) == 0 || int(m_clock.getElapsedTime().asSeconds()) % 15 != 0.0f) {
		if (type == 0) {
			entity->addComponent<CType>("goblin");
			damage = 5;
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
			damage = 3;
			velocity *= 1.5;
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
			damage = 2;
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
			damage = 2.5f;
			health = 40;
			animation = "D_slimeWalk";
			if (line == 2) {
				animation = "D_slimeWalk";
			}
			else {
				animation = "S_slimeWalk";
			}
		}
	}
	else {
		if (type == 0) {
			entity->addComponent<CType>("goblin");
			damage = 50.f;
			velocity *= 0.3;
			health = 400;
			if (line == 2) {
				animation = "D_goblinWalk";
			}
			else {
				animation = "S_goblinWalk";
			}
		}
		else if (type == 1) {
			entity->addComponent<CType>("wolf");
			damage = 30.f;
			velocity *= 1.2f;
			health = 200;
			if (line == 2) {
				animation = "D_wolfWalk";
			}
			else {
				animation = "S_wolfWalk";
			}
		}
		else if (type == 2) {
			entity->addComponent<CType>("bee");
			damage = 40.f;
			animation = "D_beeWalk";
			health = 300;
			if (line == 2) {
				animation = "D_beeWalk";
			}
			else {
				animation = "S_beeWalk";
			}
		}
		else if (type == 3) {
			entity->addComponent<CType>("slime");
			damage = 15.f;
			health = 100;
			animation = "D_slimeWalk";
			if (line == 2) {
				animation = "D_slimeWalk";
			}
			else {
				animation = "S_slimeWalk";
			}
		}
	}
	
	entity->addComponent<CAnimation>(m_game->getAssets().getAnimation(animation), false);

	if (tag == "enemy") 
		entity->getComponent<CAnimation>().animation.getSprite().setScale(2, 2);
	else {
		entity->getComponent<CAnimation>().animation.getSprite().setScale(4, 4);
		entity->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(255, 0, 155));
	}

	auto size = entity->getComponent<CAnimation>().animation.getSprite().getGlobalBounds();
	entity->addComponent<CBoundingBox>(Vec2(size.getSize().x, size.getSize().y/2.f));
	entity->addComponent<CTransform>(pos);
	entity->getComponent<CTransform>().velocity = velocity;
	entity->addComponent<CHealth>(health);
	entity->addComponent<CAttack>(damage);
	entity->addComponent<CFocus>();
}

void Scene_Play::sShop() {
	int i = 0;
	for (auto & rect : m_shopRectangles) {
		bool& click = m_player->getComponent<CInput>().click;
		auto mouse_pos = sf::Mouse::getPosition(m_game->window());
		if (click && rect.getGlobalBounds().contains(mouse_pos.x, mouse_pos.y) && !m_mouseItem) {

			if (i < 3) {
				if (m_lastFrameDefenseSpawn + 305 > m_currentFrame && m_lastFrameDefenseSpawn != 0)
					continue;
			}
			if (i == 0) {
				if (m_coins < 20) { continue; }
				m_mouseItem = true;
			}
			else if (i == 1) {
				if (m_coins < 30) { continue; }
				m_mouseItem = true;
			}
			else if (i == 2) {
				if (m_coins < 25) { continue; }
				m_mouseItem = true;
			}
			else if (i == 3) {
				if (m_coins < 150) { continue; }
				m_mouseItem = true;
			}
			else if (i == 4) {
				m_mouseItem = false;
				if (m_coins < 25) { continue; }
				else {
					m_player->getComponent<CHealth>().health += 50;
					if (m_player->getComponent<CHealth>().health > m_player->getComponent<CHealth>().totalHealth)
						m_player->getComponent<CHealth>().health = m_player->getComponent<CHealth>().totalHealth;
					m_coins -= 25;
				}
			}
			else if (i == 5) {
				if (m_coins < 25) { continue; }
				m_mouseItem = true;
			}

			m_selectedItem = i;

			click = false;
			
			if (i == 0 || i==1 || i==2) {
				m_grassGrid = true;
			} else if (i==3 || i==5) {
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
		size_t i = 0; 
		for (auto& roadRect : m_roadRectanglesGrid) {
			if (click && roadRect.getGlobalBounds().contains(mouse_pos.x, mouse_pos.y)) {
				click = false;
				m_mouseItem = false;
				m_roadGrid = false;

				if (m_selectedItem == 5) {
					m_coins -= 25;
					auto pos = Vec2(roadRect.getPosition().x + roadRect.getSize().x / 2.f, roadRect.getPosition().y + roadRect.getSize().y / 2.f);
					
					int line;
					if (i < 6)
						line = 1;
					else if (i < 12)
						line = 3;
					else
						line = 2;

					spawnBarricade(pos, line);
				}

				// LIGHTNING SPAWN
				if (m_selectedItem == 3) {
					m_coins -= 150;
					auto lightning = m_entityManager.addEntity("attack");
					lightning->addComponent<CTransform>();
					lightning->getComponent<CTransform>().pos = { roadRect.getPosition().x + roadRect.getSize().x / 2.f - 15, roadRect.getPosition().y + roadRect.getSize().y / 2.f - 310 };
					m_lightningSquare = roadRect;
					lightning->addComponent<CType>();
					lightning->getComponent<CType>().type = "lightning";
					lightning->getComponent<CAnimation>().animation = m_game->getAssets().getAnimation("lightning");
					lightning->addComponent<CAttack>();
					lightning->getComponent<CAttack>().damage = 2;
				}
			}
			i++;
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


				if (m_selectedItem == 0) {
					m_coins -= 20;
				}
				else if (m_selectedItem == 1) {
					m_coins -= 30;
				}
				else if (m_selectedItem == 2) {
					m_coins -= 25;
				}

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

void Scene_Play::sInfo() {
	m_drawInfo = false;
	m_infoVector.clear();
	for (auto& rect : m_shopRectangles) {
		auto mouse_pos = sf::Mouse::getPosition(m_game->window());
		if (rect.getGlobalBounds().contains(mouse_pos.x, mouse_pos.y)) {
			m_drawInfo = true;
			auto& f = m_game->getAssets().getFont("RETROGAMING");
			m_infoVector.push_back(sf::Text("Attack damage: 15", f, 20));
			m_infoVector[0].setPosition(1268, 700);
			m_infoVector.push_back(sf::Text("Attack speed: 5", f, 20));
			m_infoVector[1].setPosition(1268, 725);
			m_infoVector.push_back(sf::Text("Cost: 25", f, 20));
			m_infoVector[2].setPosition(1268, 750);
		}
	}
	for (auto& ent : m_entityManager.getEntities()) {
		auto mouse_pos = sf::Mouse::getPosition(m_game->window());
		if (ent->getComponent<CAnimation>().animation.getSprite().getGlobalBounds().contains(mouse_pos.x, mouse_pos.y)) {
			m_drawInfo = true;
			auto& f = m_game->getAssets().getFont("RETROGAMING");

			if (ent->tag() == "player") {
				auto& ent_health = ent->getComponent<CHealth>();
				auto& ent_level = ent->getComponent<CLevel>();
				std::string health = "Health: " + std::to_string((int)ent_health.health) + "/" + std::to_string((int)ent_health.totalHealth);
				m_infoVector.push_back(sf::Text(health, f, 20));
				m_infoVector[0].setPosition(1268, 700);
				std::string level = "Level: " + std::to_string((int)ent_level.level) + "/" + std::to_string((int)ent_level.max_level);
				m_infoVector.push_back(sf::Text(level , f, 20));
				m_infoVector[1].setPosition(1268, 725);

				if (ent_level.level != ent_level.max_level) {
					int upgradePrice = ent_health.totalHealth * 0.6 + 300;
					std::string upgrade = "Upgrade: " + std::to_string(m_coins) +"/" + std::to_string(upgradePrice);
					m_infoVector.push_back(sf::Text(upgrade, f, 20));
					if (upgradePrice < m_coins) m_infoVector[2].setFillColor(sf::Color::Green);
					else m_infoVector[2].setFillColor(sf::Color(64, 4, 4));
					m_infoVector[2].setPosition(1268, 750);
				}

				break;
			}
			if (ent->tag() == "enemy" || ent->tag() == "enemyBoss") {
				auto& ent_health = ent->getComponent<CHealth>();
				auto& ent_damage = ent->getComponent<CAttack>();
				std::string health = "Health: " + std::to_string((int)ent_health.health) + "/" + std::to_string((int)ent_health.totalHealth);
				m_infoVector.push_back(sf::Text(health, f, 20));
				m_infoVector[0].setPosition(1268, 700);
				std::string damage = "Damage: " + std::to_string((int)ent_damage.damage);
				m_infoVector.push_back(sf::Text(damage, f, 20));
				m_infoVector[1].setPosition(1268, 725);
				auto& speed = ent->getComponent<CTransform>().velocity;
				auto m = ent->getComponent<CAnimation>().animation.getFrameCount();
				std::string velocity;
				if (speed.x != 0)
					velocity = "Speed: " + std::to_string((int)abs(speed.x*m));
				else
					velocity = "Speed: " + std::to_string((int)abs(speed.y*m));
				m_infoVector.push_back(sf::Text(velocity, f, 20));
				m_infoVector[2].setPosition(1268, 750);
				break;
			}
			if (ent->tag() == "archer") {
				auto& f = m_game->getAssets().getFont("RETROGAMING");
				m_infoVector.push_back(sf::Text("Attack damage: 15", f, 20));
				m_infoVector[0].setPosition(1268, 700);
				m_infoVector.push_back(sf::Text("Attack speed: 5", f, 20));
				m_infoVector[1].setPosition(1268, 725);
				m_infoVector.push_back(sf::Text("Cost: 25", f, 20));
				m_infoVector[2].setPosition(1268, 750);
			}

			if (ent->tag() == "barricade") {
				auto& ent_health = ent->getComponent<CHealth>();
				auto& ent_level = ent->getComponent<CLevel>();
				std::string health = "Health: " + std::to_string((int)ent_health.health) + "/" + std::to_string((int)ent_health.totalHealth);
				m_infoVector.push_back(sf::Text(health, f, 20));
				m_infoVector[0].setPosition(1268, 700);
			}
		}
	}
}

void Scene_Play::spawnSpikes(const std::string& type , const Vec2& pos) {
	if (type == "freeze") {
		auto iceSpike = m_entityManager.addEntity("attack");
		iceSpike->addComponent<CTransform>(pos);
		iceSpike->addComponent<CAnimation>();
		iceSpike->getComponent<CAnimation>().animation = m_game->getAssets().getAnimation("iceSpike");
		iceSpike->addComponent<CType>().type = "freeze";
	}
	else if (type == "area") {
		auto woodSpike = m_entityManager.addEntity("attack");
		woodSpike->addComponent<CTransform>(pos);
		woodSpike->addComponent<CAnimation>();
		woodSpike->getComponent<CAnimation>().animation = m_game->getAssets().getAnimation("woodSpike");
		woodSpike->addComponent<CType>().type = "area";
		woodSpike->addComponent<CAttack>().damage = 1;
	}
}

void Scene_Play::sUpgrade() {
	auto & player_animation = m_player->getComponent<CAnimation>().animation;
	auto mouse_pos = sf::Mouse::getPosition(m_game->window());
	
	if (player_animation.getSprite().getGlobalBounds().contains(mouse_pos.x, mouse_pos.y)) {
		if (m_player->getComponent<CInput>().rightClick) {
			m_player->getComponent<CInput>().rightClick = false;
			auto upgradePrice = m_player->getComponent<CHealth>().totalHealth * 0.6 + 300;
			if (m_coins >= upgradePrice) {
				m_coins -= upgradePrice;
				m_player->getComponent<CState>().state = "upgrade";
				m_player->getComponent<CHealth>().totalHealth += 200;
			}
		}
	}
}