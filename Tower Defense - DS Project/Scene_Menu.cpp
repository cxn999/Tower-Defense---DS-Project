#pragma warning(disable : 4996)

#include "Scene_Menu.h"
#include "Scene_Play.h"
#include "Scene_Settings.h"
#include "GameEngine.h"

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_Menu::init() {
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::Enter, "ENTER");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::Down, "DOWN");


	// Set the sf::window in a more convenient variable
	auto& m_window = m_game->window();
	// Extract the font from the assets
	auto& f = m_game->getAssets().getFont("RETROGAMING");
	// Calculate middle of the screen in X axis
	auto mx = m_window.getSize().x / 2;
	// Set the titleText
	m_titleText = sf::Text(m_title, f, 40);
	// Set it's position and it's color
	m_titleText.setPosition(mx - m_titleText.getGlobalBounds().width / 2.f, 130);
	m_titleText.setColor(sf::Color::Black);
	
	// Constant for the fontSize of the levels
	int levels_fontSize = 30;

	// Add levels text to the vector
	m_texts.push_back(sf::Text("PLAY", f, levels_fontSize));
	m_texts.push_back(sf::Text("SETTINGS", f, levels_fontSize));
	m_texts.push_back(sf::Text("CREDITS", f, levels_fontSize));

	std::string helpText("UP: W   DOWN: S   ENTER: ENTER   BACK: ESC");
	m_texts.push_back(sf::Text(helpText, f, 30));
	m_texts[3].setPosition(20, m_window.getSize().y - 50);
	m_texts[3].setColor(sf::Color::Black);

	for (int i = 0; i < 3; i++) {
		m_texts[i].setPosition(mx - m_texts[i].getGlobalBounds().width / 2.f, 630 + 50 * i);
	}
}

void Scene_Menu::update() {
	m_entityManager.update();
	sRender();
}

void Scene_Menu::onEnd() {
	m_game->quit();
}

void Scene_Menu::sDoAction(const Action& action) {
	if (action.type() == "START") {
		if (action.name() == "PAUSE") { setPaused(!m_paused); }
		else if (action.name() == "QUIT") { onEnd(); }
		else if (action.name() == "ENTER") {
			if (m_selectedMenuIndex == 0) {
				m_game->changeScene("PLAY", std::make_shared<Scene_Play>(m_game, "level1.txt"), true);
			}
			else if (m_selectedMenuIndex == 1) {
				m_game->changeScene("SETTINGS", std::make_shared<Scene_Settings>(m_game), true);
			}
			else if (m_selectedMenuIndex == 2) {
				//m_game->changeScene("SETTINGS", std::make_shared<Scene_Settings>(m_game), true);
			}
		}
		else if (action.name() == "DOWN") { m_selectedMenuIndex = (m_selectedMenuIndex + 1) % 3; }
		else if (action.name() == "UP") { m_selectedMenuIndex = (m_selectedMenuIndex + 2) % 3; }
	}
}

void Scene_Menu::sRender() {
	// Store sf::window in a variable for more convenience
	auto& m_window = m_game->window();
	// Calculate middle of the screen in X axis
	auto mx = m_window.getSize().x / 2;
	// Calculate middle of the screen in Y axis
	auto my = m_window.getSize().y / 2;
	// Clear the window with blue
	m_window.clear(sf::Color::Blue);
	sf::Sprite backgroundSprite = m_game->getAssets().getAnimation("backgroundMenu").getSprite();
	backgroundSprite.setPosition(mx, my);
	m_window.draw(backgroundSprite);

	// Iterate through the text vector and set their respective color and position
	for (int i = 0; i < 3; i++) {
		if (i != m_selectedMenuIndex) {
			m_texts[i].setColor(sf::Color::Black);
		}
		else {
			m_texts[i].setColor(sf::Color::White);
		}
		// Draw the levels text
		m_window.draw(m_texts[i]);
	}

	// Draw the title
	m_window.draw(m_titleText);
	m_window.draw(m_texts[3]);

	// Display the window
	m_window.display();
}
