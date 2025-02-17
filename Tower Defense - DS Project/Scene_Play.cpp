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
#include <iomanip>
#include <sstream>
#include <algorithm>


Scene_Play::Scene_Play(GameEngine* gameEngine) 
	: Scene(gameEngine)
{
	srand(time(NULL));
	init();

}

void Scene_Play::generateRoadRectangles() {
	int n = 0;
	for (int i = 64; i < m_game->window().getSize().x - 128; i += 128) {
		if (i != 128 * 6 + 64) {
			sf::RectangleShape roadRect(sf::Vector2f(128, 128));
			roadRect.setPosition(i, m_game->window().getSize().y / 2.f);
			roadRect.setOutlineThickness(1.f);
			roadRect.setOutlineColor(sf::Color::Magenta);
			roadRect.setFillColor(sf::Color(219, 116, 209, 80));
			m_roadRectanglesGrid.push_back(roadRect);
			m_usedRoadRectanglesIndex[n] = false;
		}
		n++;
	}
	for (int i = 64; i < m_game->window().getSize().y / 2.f; i += 128) {
		sf::RectangleShape roadRect(sf::Vector2f(128, 128));
		roadRect.setPosition(m_game->window().getSize().x / 2.f - 64, i);
		roadRect.setOutlineThickness(1.f);
		roadRect.setOutlineColor(sf::Color::Magenta);
		roadRect.setFillColor(sf::Color(219, 116, 209, 80));
		m_roadRectanglesGrid.push_back(roadRect);
		m_usedRoadRectanglesIndex[n] = false;
		n++;
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
			m_usedGrassRectanglesIndex[n] = false;
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
		m_usedGrassRectanglesIndex[n] = false;
		n++;
	}
}

void Scene_Play::init() {
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE"); // Toggle drawing textures
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION"); // Toggle drawing collision boxes
	registerAction(sf::Keyboard::R, "RESTART");
	registerAction(sf::Keyboard::H, "HELP");
	registerAction(sf::Keyboard::N, "NEXT");
	registerAction(sf::Mouse::Right, "RIGHTCLICK"); 
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

	m_pauseBackground = m_game->getAssets().getAnimation("backgroundPause").getSprite();
	m_pauseBackground.setPosition(sf::Vector2f(m_game->window().getSize().x / 2, m_game->window().getSize().y * 0.86));

	tutorials.push_back(m_game->getAssets().getAnimation("tuto1").getSprite()); 
	tutorials.push_back(m_game->getAssets().getAnimation("tuto2").getSprite());
	tutorials.push_back(m_game->getAssets().getAnimation("tuto3").getSprite());
	tutorials.push_back(m_game->getAssets().getAnimation("tuto4").getSprite());
	tutorials.push_back(m_game->getAssets().getAnimation("tuto5").getSprite());
	tutorials.push_back(m_game->getAssets().getAnimation("tuto6").getSprite());
	tutorials.push_back(m_game->getAssets().getAnimation("tuto7").getSprite());
	tutorials.push_back(m_game->getAssets().getAnimation("tuto8").getSprite());

	for (int i = 0; i <= 7; i++) {
		tutorials[i].setPosition(sf::Vector2f(m_game->window().getSize().x / 2, m_game->window().getSize().y / 2));
	}
}

void Scene_Play::sDoAction(const Action& action) {
	if (action.type() == "START") {
		if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "HELP") { 
			setPaused(!m_paused); 
			if (m_toggleHelp)
				m_toggleHelp = false;
			else
				m_toggleHelp = true;
		}
		else if (action.name() == "NEXT") {
			tutorialIndex += 1;
			if (tutorialIndex == 8)
				tutorialIndex = 0;
		}
		else if (action.name() == "PAUSE") { 
			setPaused(!m_paused); 
			if (m_pauseP)
				m_pauseP = false;
			else
				m_pauseP = true;
		}
		else if (action.name() == "QUIT") { onEnd(); }
		else if (action.name() == "RESTART") {
			if (!m_player->isActive() || m_paused) {
				replay();
			}
		}
		else if (action.name() == "CLICK") { m_player->getComponent<CInput>().click = true; }
		else if (action.name() == "RIGHTCLICK") { m_player->getComponent<CInput>().rightClick = true; }
		// ADD REMAINING ACTIONS
	}
	else if (action.type() == "END") {
		if (action.name() == "CLICK") { m_player->getComponent<CInput>().click = false; }
		else if (action.name() == "RIGHTCLICK") { m_player->getComponent<CInput>().rightClick = false; }
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

void Scene_Play::spawnBarricade(const Vec2& position, size_t line, size_t index) {
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
	m_barricade->getComponent<CTransform>().index = index;
	m_barricade->getComponent<CTransform>().pos = { position.x, position.y - offset};
	m_barricade->addComponent<CHealth>(200);
}

void Scene_Play::sRender() {
	auto& window = m_game->window();

	window.clear(sf::Color(166, 176, 79));

	window.draw(m_game->getAssets().getAnimation("background").getSprite());

	if (m_drawTextures) {
		for (auto& e : m_entityManager.getEntities()) {
			if (e->tag() == "barricade") continue;
			if (e->getComponent<CType>().type == "bee") continue;
			if (e->tag() == "defense" || e->tag() == "archer") continue;

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

		for (auto& e : m_entityManager.getEntities("barricade")) {
			if (e->tag() != "enemyBoss")
					e->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(255, 255, 255));
			if (e->hasComponent<CHealth>() && e->getComponent<CHealth>().prevHealth > e->getComponent<CHealth>().health)
				e->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(255, 0, 0, 200));
			window.draw(e->getComponent<CAnimation>().animation.getSprite());
		}

		for (auto& e : m_entityManager.getEntities("enemy")) {
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

		for (auto& e : m_entityManager.getEntities("enemyBoss")) {
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

		auto& defenses = m_entityManager.getEntities("defense");

		// Sort the vector based on the Y component of the CTransform component
		std::sort(defenses.begin(), defenses.end(), [](const auto& lhs, const auto& rhs) {
			return lhs->getComponent<CTransform>().pos.y < rhs->getComponent<CTransform>().pos.y;
			});

		// Accessing the sorted vector
		for (const auto& entity : defenses) {
			window.draw(entity->getComponent<CAnimation>().animation.getSprite());
		}

		for (auto& archer : m_entityManager.getEntities("archer")) {
			auto & sprite = archer->getComponent<CAnimation>().animation.getSprite();
			sprite.setColor(sf::Color(255, 255, 255));
			window.draw(sprite);
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

	sShop();

	if (m_grassGrid && !m_paused) {
		int n = 0;
		for (auto& grassRect : m_grassRectanglesGrid) {
			if (m_usedGrassRectanglesIndex[n] == true) {
				grassRect.setFillColor(sf::Color(255, 0, 0, 80));
				grassRect.setOutlineColor(sf::Color::Red);
			}
			window.draw(grassRect);
			n++;
		}
	}
	if (m_roadGrid && !m_paused) {
		int n = 0;
		for (auto& roadRect : m_roadRectanglesGrid) {
			roadRect.setFillColor(sf::Color(219, 116, 209, 80));
			roadRect.setOutlineColor(sf::Color::Magenta);
			if (m_selectedItem == 5) {
				std::cout << m_selectedItem << std::endl;
				if (m_usedRoadRectanglesIndex[n] == true) {
					roadRect.setFillColor(sf::Color(255, 0, 0, 80));
					roadRect.setOutlineColor(sf::Color::Red);
				}
			}
			window.draw(roadRect);
			n++;
		}
	}

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
	
	 m_helpTutorialText = sf::Text("PRESS 'H' FOR HELP", m_game->getAssets().getFont("RETROGAMING"), 20);
	 m_helpTutorialText.setPosition(window.getSize().x * 0.40f, window.getSize().y * 0.92f);
	 m_helpTutorialText.setOutlineColor(sf::Color::Black);
	 m_helpTutorialText.setOutlineThickness(3.f);
	 window.draw(m_helpTutorialText);

	if (!m_player->isActive() || m_paused) {
		 sf::Text replay = sf::Text("Restart: R", m_game->getAssets().getFont("RETROGAMING"), 40);
		 sf::Text quit = sf::Text("Quit: ESC", m_game->getAssets().getFont("RETROGAMING"), 40);
		 sf::Text gameOver = sf::Text("GAME OVER", m_game->getAssets().getFont("RETROGAMING"), 40);
		 gameOver.setFillColor(sf::Color::Red);

		

		 replay.setPosition(window.getSize().x * 0.1f, window.getSize().y * 0.83f);
		 gameOver.setPosition(window.getSize().x * 0.4f, window.getSize().y * 0.83f);
		 quit.setPosition(window.getSize().x * 0.7f, window.getSize().y * 0.83f);

		 gameOver.setOutlineThickness(3.f);
		 replay.setOutlineThickness(3.f);
		 quit.setOutlineThickness(3.f);

		 gameOver.setOutlineColor(sf::Color::Black);
		 replay.setOutlineColor(sf::Color::Black);
		 quit.setOutlineColor(sf::Color::Black);

		 window.draw(m_pauseBackground); 
		 window.draw(replay);

		 window.draw(gameOver);

		 window.draw(quit);
	}

	if (m_pauseP){
		
		sf::Text replay = sf::Text("Restart: R", m_game->getAssets().getFont("RETROGAMING"), 40); 
		sf::Text quit = sf::Text("Quit: ESC", m_game->getAssets().getFont("RETROGAMING"), 40); 
		m_helpPauseText = sf::Text("Resume: P", m_game->getAssets().getFont("RETROGAMING"), 40);

		

		replay.setPosition(window.getSize().x * 0.1f, window.getSize().y * 0.83f);
		quit.setPosition(window.getSize().x * 0.4f, window.getSize().y * 0.83f);
		m_helpPauseText.setPosition(window.getSize().x * 0.7f, window.getSize().y * 0.83f);

		m_helpPauseText.setOutlineThickness(3.f); 
		replay.setOutlineThickness(3.f);
		quit.setOutlineThickness(3.f);

		m_helpPauseText.setOutlineColor(sf::Color::Black);
		replay.setOutlineColor(sf::Color::Black);
		quit.setOutlineColor(sf::Color::Black);

		window.draw(m_pauseBackground);
		window.draw(replay); 
		
		window.draw(m_helpPauseText); 

		window.draw(quit); 

	}

	if (m_toggleHelp) {
			window.draw(tutorials[tutorialIndex]);
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
		auto& type = e->getComponent<CType>().type; 
		auto& direction = e->getComponent<CState>().direction; 
		
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
		auto& type = e->getComponent<CType>().type;
		auto& direction = e->getComponent<CState>().direction;

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
				m_coins += 30;
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
		auto d_level = defense->getComponent<CLevel>().level;
		auto& d_state = defense->getComponent<CState>().state;

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

		if (d_state == "upgrade" && d.animation.getName().find("upgrade") == std::string::npos) {
			if (d_level == 2) {
				auto& archer = defense->getComponent<CFocus>().entity;
				archer->getComponent<CTransform>().pos.y -= 12;
				if (d_type == "area") {
					d.animation = (m_game->getAssets().getAnimation("upgradeAreaTower2"));
					archer->getComponent<CDelay>().delay /= 2.f;
				}
				else if (d_type == "freeze") {
					d.animation = (m_game->getAssets().getAnimation("upgradeFreezeTower2"));
				}
				else if (d_type == "target") {
					d.animation = (m_game->getAssets().getAnimation("upgradeTargetTower2"));
					archer->getComponent<CAttack>().damage *= 2;
				}
			}
			else if (d_level == 3) {
				auto& archer = defense->getComponent<CFocus>().entity;
				archer->getComponent<CTransform>().pos.y -= 16;
				if (d_type == "area") {
					d.animation = (m_game->getAssets().getAnimation("upgradeAreaTower3"));
					archer->getComponent<CDelay>().delay /= 2.f;
				}
				else if (d_type == "freeze") {
					d.animation = (m_game->getAssets().getAnimation("upgradeFreezeTower3"));
				}
				else if (d_type == "target") {
					d.animation = (m_game->getAssets().getAnimation("upgradeTargetTower3"));
					archer->getComponent<CAttack>().damage *= 2;
				}
			}
			d.animation.getSprite().setPosition(d_pos.x, d_pos.y);
		}

		if (d.animation.getName().find("upgrade") != std::string::npos && d.animation.hasEnded()) {
			d_state = "idle";
			if (d_level == 1) {
				if (d_type == "area") {
					d.animation = (m_game->getAssets().getAnimation("areaTower2"));
				}
				else if (d_type == "freeze") {
					d.animation = (m_game->getAssets().getAnimation("freezeTower2"));
				}
				else if (d_type == "target") {
					d.animation = (m_game->getAssets().getAnimation("targetTower2"));
				}

				auto archer = m_entityManager.addEntity("archer");
				defense->addComponent<CFocus>();
				defense->getComponent<CFocus>().entity = archer;

				archer->addComponent<CType>(d_type);
				archer->addComponent<CAnimation>();

				auto type = archer->getComponent<CType>().type;

				if (type == "area") {
					archer->getComponent<CAnimation>().animation = (m_game->getAssets().getAnimation("D_archerAreaIdle"));
					archer->addComponent<CDelay>(0, 200);
					archer->addComponent<CAttack>(15);
				}
				else if (type == "freeze") {
					archer->getComponent<CAnimation>().animation = (m_game->getAssets().getAnimation("D_archerFreezeIdle"));
					archer->addComponent<CDelay>(0, 200);
				}
				else if (type == "target") {
					archer->getComponent<CAnimation>().animation = (m_game->getAssets().getAnimation("D_archerTargetIdle"));
					archer->addComponent<CAttack>(18);
				}

				archer->addComponent<CTransform>(d_pos);
				archer->addComponent<CRange>(265);
				archer->addComponent<CState>("idle", "vertical");
			}
			else if (d_level == 2) {
				if (d_type == "area") {
					d.animation = (m_game->getAssets().getAnimation("areaTower3"));
				}
				else if (d_type == "freeze") {
					d.animation = (m_game->getAssets().getAnimation("freezeTower3"));
				}
				else if (d_type == "target") {
					d.animation = (m_game->getAssets().getAnimation("targetTower3"));
				}
			}
			else if (d_level == 3) {
				if (d_type == "area") {
					d.animation = (m_game->getAssets().getAnimation("areaTower4"));
				}
				else if (d_type == "freeze") {
					d.animation = (m_game->getAssets().getAnimation("freezeTower4"));
				}
				else if (d_type == "target") {
					d.animation = (m_game->getAssets().getAnimation("targetTower4"));
				}
			}

			d.animation.getSprite().setPosition(d_pos.x, d_pos.y);
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
			size_t index = barricade->getComponent<CTransform>().index;
			m_usedRoadRectanglesIndex[index] = false;
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
			else if (e->getComponent<CState>().effect == "freeze2") {
				e_transform.pos += e_transform.velocity / 4.f;
			}
			else if (e->getComponent<CState>().effect == "freeze3") {
				e_transform.pos += e_transform.velocity / 8.f;
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
		if (e_transform.move) {
			e_transform.prevPos = e_transform.pos;

			if (e->getComponent<CState>().effect == "freeze") {
				e_transform.pos += e_transform.velocity / 2.f;
			}
			else if (e->getComponent<CState>().effect == "freeze2") {
				e_transform.pos += e_transform.velocity / 4.f;
			}
			else if (e->getComponent<CState>().effect == "freeze3") {
				e_transform.pos += e_transform.velocity / 8.f;
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
					auto & type = e->getComponent<CType>().type;
				}
				else if (a->getComponent<CType>().type == "freeze2") {
					e->getComponent<CState>().effect = "freeze2";
					auto& type = e->getComponent<CType>().type;
				}
				else if (a->getComponent<CType>().type == "freeze3") {
					e->getComponent<CState>().effect = "freeze3";
					auto& type = e->getComponent<CType>().type;
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
						if (archer->getComponent<CLevel>().level == 1) {
							spawnSpikes("freeze", e_transform.pos);

						}
						else if (archer->getComponent<CLevel>().level == 2) {
							spawnSpikes("freeze2", e_transform.pos);

						}
						else if (archer->getComponent<CLevel>().level == 3) {
							spawnSpikes("freeze3", e_transform.pos);

						}
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
		if (t % 1 == 0 && m_spawnRateFrame >= 5) {		//Each second that passes by, the n of frames needed for an enemy to spawn gets reduced by 2 
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
			damage = 30;
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
			damage = 40;
			animation = "D_beeWalk";
			health = 222;
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

	if (m_player->getComponent<CInput>().rightClick && m_mouseItem) {
		m_mouseItem = false;
		m_roadGrid = false;
		m_grassGrid = false;
	}

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
				if (m_coins < 40) { continue; }
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
					if (m_usedRoadRectanglesIndex[i] == true) continue;
					m_usedRoadRectanglesIndex[i] = true;
					m_coins -= 40;
					auto pos = Vec2(roadRect.getPosition().x + roadRect.getSize().x / 2.f, roadRect.getPosition().y + roadRect.getSize().y / 2.f);
					
					int line;
					if (i < 6)
						line = 1;
					else if (i < 12)
						line = 3;
					else
						line = 2;

					spawnBarricade(pos, line, i);
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

				if (m_usedGrassRectanglesIndex[i] == true) continue;

				click = false;
				m_mouseItem = false;
				m_grassGrid = false;
				m_usedGrassRectanglesIndex[i] = true;


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
								grassRect.getPosition().y + grassRect.getSize().y / 2.f + 15.f};

				m_defenseSquare = grassRect;
				m_lastFrameDefenseSpawn = m_currentFrame;

				auto defense = m_entityManager.addEntity("defense");
				defense->addComponent<CTransform>(m_defensePos);
				defense->addComponent<CAnimation>(m_game->getAssets().getAnimation("constructionTower"), false);
				defense->getComponent<CAnimation>().animation.getSprite().setScale(2, 2);
				defense->getComponent<CAnimation>().animation.getSprite().setPosition(m_defensePos.x,m_defensePos.y);
				defense->addComponent<CLevel>(3);

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
	int i = 0;
	for (auto& rect : m_shopRectangles) {
		auto mouse_pos = sf::Mouse::getPosition(m_game->window());
		if (rect.getGlobalBounds().contains(mouse_pos.x, mouse_pos.y)) {
			m_drawInfo = true;
			auto& f = m_game->getAssets().getFont("RETROGAMING");

			switch (i) {
			case 0:
				m_infoVector.push_back(sf::Text("Red Archer Tower", f, 20));
				m_infoVector[0].setPosition(1268, 700);
				m_infoVector[0].setFillColor(sf::Color(130, 9, 15));
				m_infoVector.push_back(sf::Text("Area damage: 60", f, 20));
				m_infoVector[1].setPosition(1268, 725);
				m_infoVector.push_back(sf::Text("Attack speed: 3.33s", f, 20));
				m_infoVector[2].setPosition(1268, 750);
				m_infoVector.push_back(sf::Text("Cost: 20", f, 20));
				m_infoVector[3].setPosition(1268, 775);
				break;
			case 1:
				m_infoVector.push_back(sf::Text("Blue Archer Tower", f, 20));
				m_infoVector[0].setPosition(1268, 700);
				m_infoVector[0].setFillColor(sf::Color::Blue);
				m_infoVector.push_back(sf::Text("Speed of enemies: 50%", f, 20));
				m_infoVector[1].setPosition(1268, 725);
				m_infoVector.push_back(sf::Text("Attack speed: 3.33s", f, 20));
				m_infoVector[2].setPosition(1268, 750);
				m_infoVector.push_back(sf::Text("Cost: 30", f, 20));
				m_infoVector[3].setPosition(1268, 775);
				break;
			case 2:
				m_infoVector.push_back(sf::Text("Green Archer Tower", f, 20));
				m_infoVector[0].setPosition(1268, 700);
				m_infoVector[0].setFillColor(sf::Color::Green);
				m_infoVector.push_back(sf::Text("Attack damage: 18", f, 20));
				m_infoVector[1].setPosition(1268, 725);
				m_infoVector.push_back(sf::Text("Attack speed: 0.2s", f, 20));
				m_infoVector[2].setPosition(1268, 750);
				m_infoVector.push_back(sf::Text("Cost: 25", f, 20));
				m_infoVector[3].setPosition(1268, 775);
				break;
			case 3:
				m_infoVector.push_back(sf::Text("Lightning", f, 20));
				m_infoVector[0].setPosition(1268, 700);
				m_infoVector.push_back(sf::Text("Attack damage: 120", f, 20));
				m_infoVector[1].setPosition(1268, 725);
				m_infoVector.push_back(sf::Text("Attack speed: 1s", f, 20));
				m_infoVector[2].setPosition(1268, 750);
				m_infoVector.push_back(sf::Text("Cost: 150", f, 20));
				m_infoVector[3].setPosition(1268, 775);
				break;
			case 4:
				m_infoVector.push_back(sf::Text("Heal", f, 20));
				m_infoVector[0].setPosition(1268, 700);
				m_infoVector.push_back(sf::Text("Tower HP: +50", f, 20));
				m_infoVector[1].setPosition(1268, 725);
				break;
			case 5:
				m_infoVector.push_back(sf::Text("Barricade", f, 20));
				m_infoVector[0].setPosition(1268, 700);
				m_infoVector.push_back(sf::Text("HP: 200", f, 20));
				m_infoVector[1].setPosition(1268, 725);
				break;
			}
		}
		i++;
	}
	for (auto& ent : m_entityManager.getEntities()) {
		auto mouse_pos = sf::Mouse::getPosition(m_game->window());
		if (ent->getComponent<CAnimation>().animation.getSprite().getGlobalBounds().contains(mouse_pos.x, mouse_pos.y)) {
			m_drawInfo = true;
			auto& f = m_game->getAssets().getFont("RETROGAMING");
			if (ent->tag() == "enemy" || ent->tag() == "enemyBoss") {
				auto& ent_health = ent->getComponent<CHealth>();
				auto& ent_damage = ent->getComponent<CAttack>();
				auto& ent_type = ent->getComponent<CType>().type;
				m_infoVector.push_back(sf::Text(ent_type, f, 20));
				m_infoVector[0].setPosition(1268, 700);
				m_infoVector[0].setFillColor(sf::Color::Black);
				std::string health = "Health: " + std::to_string((int)ent_health.health) + "/" + std::to_string((int)ent_health.totalHealth);
				m_infoVector.push_back(sf::Text(health, f, 20));
				m_infoVector[1].setPosition(1268, 725);
				std::string damage = "Damage: " + std::to_string((int)ent_damage.damage);
				m_infoVector.push_back(sf::Text(damage, f, 20));
				m_infoVector[2].setPosition(1268, 750);
				auto& speed = ent->getComponent<CTransform>().velocity;
				auto m = ent->getComponent<CAnimation>().animation.getFrameCount();
				std::string velocity;
				float v;
				if (speed.x != 0)
					v = (int)abs(speed.x * m);
				else
					v = (int)abs(speed.y * m);

				if (ent->getComponent<CState>().effect == "freeze") {
					v = v/2.f;
				}
				else if (ent->getComponent<CState>().effect == "freeze2") {
					v = v / 4.f;
				}
				else if (ent->getComponent<CState>().effect == "freeze3") {
					v = v / 8.f;
				}

				velocity = "Speed: " + std::to_string((int)v);

				m_infoVector.push_back(sf::Text(velocity, f, 20));
				m_infoVector[3].setPosition(1268, 775);
				break;
			}
			else if (ent->tag() == "defense") {

				if (ent->getComponent<CAnimation>().animation.getName().find("construction") != std::string::npos) continue;
				if (ent->getComponent<CAnimation>().animation.getName().find("upgrade") != std::string::npos) continue;

				auto& f = m_game->getAssets().getFont("RETROGAMING");
				auto archer_type = ent->getComponent<CType>().type;
				auto archer_level = ent->getComponent<CLevel>();
				if (archer_type == "area") {
					m_infoVector.push_back(sf::Text("Red Archer Tower", f, 20));
					m_infoVector[0].setPosition(1268, 700);
					m_infoVector[0].setFillColor(sf::Color(130, 9, 15));
					m_infoVector.push_back(sf::Text("Area damage: 60", f, 20));
					m_infoVector[1].setPosition(1268, 725);

					auto & archer = ent->getComponent<CFocus>().entity;
					float s = archer->getComponent<CDelay>().delay/60.f;
					std::stringstream stream;
					stream << std::fixed << std::setprecision(2) << s;
					std::string speed = "Attack Speed: " + stream.str() + "s";

					m_infoVector.push_back(sf::Text(speed, f, 20));
					m_infoVector[2].setPosition(1268, 750);
				}
				else if (archer_type == "freeze") {
					m_infoVector.push_back(sf::Text("Blue Archer Tower", f, 20));
					m_infoVector[0].setPosition(1268, 700);
					m_infoVector[0].setFillColor(sf::Color::Blue);
					m_infoVector.push_back(sf::Text("Speed of enemies: 50%", f, 20));
					m_infoVector[1].setPosition(1268, 725);

					auto& archer = ent->getComponent<CFocus>().entity;
					float s = archer->getComponent<CDelay>().delay / 60.f;
					std::stringstream stream;
					stream << std::fixed << std::setprecision(2) << s;
					std::string speed = "Attack Speed: " + stream.str() + "s";

					m_infoVector.push_back(sf::Text(speed, f, 20));
					m_infoVector[2].setPosition(1268, 750);
				}
				else if (archer_type == "target") {
					m_infoVector.push_back(sf::Text("Green Archer Tower", f, 20));
					m_infoVector[0].setPosition(1268, 700);
					m_infoVector[0].setFillColor(sf::Color::Green);

					auto& archer = ent->getComponent<CFocus>().entity;
					int a = archer->getComponent<CAttack>().damage;
					std::string attack = "Attack Damage: " + std::to_string(a);
					m_infoVector.push_back(sf::Text(attack, f, 20));
					m_infoVector[1].setPosition(1268, 725);
					m_infoVector.push_back(sf::Text("Attack speed: 0.2s", f, 20));
					m_infoVector[2].setPosition(1268, 750);
				}

				std::string level = "Level: " + std::to_string((int)archer_level.level) + "/" + std::to_string((int)archer_level.max_level);
				m_infoVector.push_back(sf::Text(level, f, 20));
				m_infoVector[3].setPosition(1268, 775);

				if (archer_level.level != archer_level.max_level) {
					int upgradePrice;
					switch (archer_level.level) {
					case 1:
						upgradePrice = 80;
						break;
					case 2:
						upgradePrice = 120;
						break;
					case 3:
						upgradePrice = 160;
						break;
					}

					std::string upgrade = "Upgrade: " + std::to_string(m_coins) + "/" + std::to_string(upgradePrice);
					m_infoVector.push_back(sf::Text(upgrade, f, 20));
					if (upgradePrice < m_coins) m_infoVector[4].setFillColor(sf::Color::Green);
					else m_infoVector[4].setFillColor(sf::Color(130, 9, 15));
					m_infoVector[4].setPosition(1268, 800);
				}

			}
			else if (ent->tag() == "barricade") {
				auto& ent_health = ent->getComponent<CHealth>();
				auto& ent_level = ent->getComponent<CLevel>();
				std::string health = "Health: " + std::to_string((int)ent_health.health) + "/" + std::to_string((int)ent_health.totalHealth);
				m_infoVector.push_back(sf::Text(health, f, 20));
				m_infoVector[0].setPosition(1268, 700);
			}
		}
	}
	auto mouse_pos = sf::Mouse::getPosition(m_game->window());
	if (m_player->getComponent<CAnimation>().animation.getSprite().getGlobalBounds().contains(mouse_pos.x, mouse_pos.y)) {
		m_infoVector.clear();
		auto& f = m_game->getAssets().getFont("RETROGAMING");
		auto& ent_health = m_player->getComponent<CHealth>();
		auto& ent_level = m_player->getComponent<CLevel>();
		std::string health = "Health: " + std::to_string((int)ent_health.health) + "/" + std::to_string((int)ent_health.totalHealth);
		m_infoVector.push_back(sf::Text(health, f, 20));
		m_infoVector[0].setPosition(1268, 700);
		std::string level = "Level: " + std::to_string((int)ent_level.level) + "/" + std::to_string((int)ent_level.max_level);
		m_infoVector.push_back(sf::Text(level, f, 20));
		m_infoVector[1].setPosition(1268, 725);

		if (ent_level.level != ent_level.max_level) {
			int upgradePrice = ent_health.totalHealth * 0.6 + 300;
			std::string upgrade = "Upgrade: " + std::to_string(m_coins) + "/" + std::to_string(upgradePrice);
			m_infoVector.push_back(sf::Text(upgrade, f, 20));
			if (upgradePrice < m_coins) m_infoVector[2].setFillColor(sf::Color::Green);
			else m_infoVector[2].setFillColor(sf::Color(130, 9, 15));
			m_infoVector[2].setPosition(1268, 750);
		}
	}
}

void Scene_Play::spawnSpikes(const std::string& type , const Vec2& pos) {
	if (type.find("freeze") != std::string::npos) {
		auto iceSpike = m_entityManager.addEntity("attack");
		iceSpike->addComponent<CTransform>(pos);
		iceSpike->addComponent<CAnimation>();
		iceSpike->getComponent<CAnimation>().animation = m_game->getAssets().getAnimation("iceSpike");

		if (type == "freeze") {
			iceSpike->addComponent<CType>().type = "freeze";
		}
		else if (type == "freeze2") {
			iceSpike->addComponent<CType>().type = "freeze2";
		}
		else if (type == "freeze3") {
			iceSpike->addComponent<CType>().type = "freeze3";
		}
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
		auto upgradePrice = m_player->getComponent<CHealth>().totalHealth * 0.6 + 300;

		if (m_player->getComponent<CInput>().rightClick && m_player->getComponent<CState>().state != "upgrade" && m_coins >= upgradePrice) {
			if (m_player->getComponent<CLevel>().level < m_player->getComponent<CLevel>().max_level) {
				m_player->getComponent<CLevel>().level++;
				m_player->getComponent<CInput>().rightClick = false;
				if (m_coins >= upgradePrice) {
					m_coins -= upgradePrice;
					auto& cHealth = m_player->getComponent<CHealth>();
					m_player->getComponent<CState>().state = "upgrade";
					float percent = cHealth.health / cHealth.totalHealth;
					cHealth.totalHealth += 200;
					cHealth.health = cHealth.totalHealth * percent;
				}
			}
		}
	}

	for (auto& defense : m_entityManager.getEntities("defense")) {
		auto upgradePrice = 80 + 40 * (defense->getComponent<CLevel>().level - 1);

		if (defense->getComponent<CAnimation>().animation.getSprite().getGlobalBounds().contains(mouse_pos.x, mouse_pos.y) && m_coins >= upgradePrice) {

			if (defense->getComponent<CAnimation>().animation.getName().find("construction") != std::string::npos) continue;

			if (defense->getComponent<CAnimation>().animation.getName().find("upgrade") != std::string::npos) continue;

			if (m_player->getComponent<CInput>().rightClick && defense->getComponent<CState>().state != "upgrade") {
				if (defense->getComponent<CLevel>().level < defense->getComponent<CLevel>().max_level) {
					defense->getComponent<CLevel>().level++;
					defense->getComponent<CInput>().rightClick = false;
					if (m_coins >= upgradePrice) {
						m_coins -= upgradePrice;
						auto& cHealth = m_player->getComponent<CHealth>();
						defense->getComponent<CState>().state = "upgrade";
					}
				}
			}
		}
	}
}

void Scene_Play::replay() {
	m_game->changeScene("PLAY", std::make_shared<Scene_Play>(m_game), true);
}