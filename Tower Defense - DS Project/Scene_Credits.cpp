#pragma warning(disable : 4996)

#include "Scene_Credits.h"
#include "Scene_Menu.h"
#include "GameEngine.h"

Scene_Credits::Scene_Credits(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_Credits::init() {
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "QUIT");

	// Set the sf::window in a more convenient variable
	auto& m_window = m_game->window();
	// Extract the font from the assets
	auto& f = m_game->getAssets().getFont("RETROGAMING");
	// Calculate middle of the screen in X axis
	auto mx = m_window.getSize().x / 2;
	// Set the titleText
	m_titleText = sf::Text(m_title, f, 40);
	// Set it's position and it's color
	m_titleText.setPosition(mx - m_titleText.getGlobalBounds().width / 2.f, 80);
	m_titleText.setColor(sf::Color::Black);

	// Constant for the fontSize of the levels
	int levels_fontSize = 30;

	// Add levels text to the vector
	m_texts.push_back(sf::Text("Pablo Lird", f, levels_fontSize));
	m_texts.push_back(sf::Text("Fernando Rojas", f, levels_fontSize));
	m_texts.push_back(sf::Text("Aristides Gernhofer", f, levels_fontSize));
	m_texts.push_back(sf::Text("Alvaro Lial", f, levels_fontSize));
	m_texts.push_back(sf::Text("Tamara Barrios", f, levels_fontSize));

	std::string helpText("UP: W   DOWN: S   ENTER: ENTER   BACK: ESC");
	m_texts.push_back(sf::Text(helpText, f, 30));
	m_texts[5].setPosition(20, m_window.getSize().y - 50);
	m_texts[5].setColor(sf::Color::Black);

	for (int i = 0; i < 5; i++) {
		m_texts[i].setPosition(mx - m_texts[i].getGlobalBounds().width / 2.f, 200 + 100 * i);
	}
}

void Scene_Credits::update() {
	m_entityManager.update();
	sRender();
}

void Scene_Credits::onEnd() {
	m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game), true);
}


void Scene_Credits::sDoAction(const Action& action) {
	if (action.type() == "START") {
		if (action.name() == "PAUSE") { setPaused(!m_paused); }
		else if (action.name() == "QUIT") { onEnd(); }
	}
}

void Scene_Credits::sRender() {
	// Store sf::window in a variable for more convenience
	auto& m_window = m_game->window();
	// Calculate middle of the screen in X axis
	auto mx = m_window.getSize().x / 2;
	// Calculate middle of the screen in Y axis
	auto my = m_window.getSize().y / 2;
	// Clear the window with blue
	m_window.clear(sf::Color(153, 255, 204));
	sf::Sprite backgroundSprite = m_game->getAssets().getAnimation("backgroundSettings").getSprite();
	backgroundSprite.setPosition(mx, my);
	m_window.draw(backgroundSprite);

	// Iterate through the text vector and set their respective color and position
	for (int i = 0; i < 5; i++) {
		m_texts[i].setColor(sf::Color::Black);
		// Draw the levels text
		m_window.draw(m_texts[i]);
	}

	// Draw the title
	m_window.draw(m_titleText);
	m_window.draw(m_texts[5]);

	// Display the window
	m_window.display();
}