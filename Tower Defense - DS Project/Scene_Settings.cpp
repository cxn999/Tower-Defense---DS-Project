#pragma warning(disable : 4996)

#include "Scene_Settings.h"
#include "Scene_Menu.h"
#include "GameEngine.h"

sf::Color Scene_Settings::getVolumeColor(float volume) {
	if (volume <= 50.0f) {
		// Volume ranges from 0 to 50, stay green
		return sf::Color(0, 255, 0);
	}
	else {
		// Volume ranges from 50 to 100, change rapidly to red
		int red = static_cast<int>((volume - 50) * 5.1f); // 5.1f scales 50 to 100 into 0 to 255
		int green = 255 - static_cast<int>((volume - 50) * 5.1f);
		return sf::Color(red, green, 0);
	}
}

void Scene_Settings::updateVolumeSlider() {
	auto& m_window = m_game->window();
	auto mx = m_window.getSize().x / 2;

	float newWidth = (m_game->getVolume() / 100.0f) * 200.0f; 
	m_volumeSlider.setSize(sf::Vector2f(newWidth, 20));
	m_volumeSlider.setPosition(mx - m_volumeSlider.getSize().x / 2, 370); 
}

Scene_Settings::Scene_Settings(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_Settings::init() {
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::Enter, "ENTER");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::Down, "DOWN");
	registerAction(sf::Keyboard::Left, "LEFT");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::Right, "RIGHT");
	registerAction(sf::Keyboard::D, "RIGHT");

	// Set the sf::window in a more convenient variable
	auto& m_window = m_game->window();
	// Extract the font from the assets
	auto& f = m_game->getAssets().getFont("RETROGAMING");
	// Calculate middle of the screen in X axis
	auto mx = m_window.getSize().x / 2;
	// Set the titleText
	// Set it's position and it's color
	m_titleText.setPosition(mx - m_titleText.getGlobalBounds().width / 2.f, 50);
	m_titleText.setColor(sf::Color::Black);

	// Constant for the fontSize of the levels
	int levels_fontSize = 50;

	// Add levels text to the vector
	m_texts.push_back(sf::Text("Volume", f, levels_fontSize));
	m_texts.push_back(sf::Text("Music", f, levels_fontSize));

	std::string helpText("UP: W   DOWN: S   ENTER: ENTER   BACK: ESC");
	m_texts.push_back(sf::Text(helpText, f, 30));
	m_texts[2].setPosition(20, m_window.getSize().y - 50);
	m_texts[2].setColor(sf::Color::Black);

	for (int i = 0; i < 2; i++) {
		m_texts[i].setPosition(mx - m_texts[i].getGlobalBounds().width / 2.f, 300 + 200 * i);
	}

	// Volume slider
	m_volumeSlider.setFillColor(getVolumeColor(m_game->getVolume()));
	updateVolumeSlider();

	m_volumeText.setFont(f);
	m_volumeText.setString(std::to_string(static_cast<int>(m_game->getVolume())));
	m_volumeText.setCharacterSize(30);
	m_volumeText.setFillColor(sf::Color::Black);
	m_volumeText.setPosition(mx + m_volumeSlider.getSize().x / 2 + 60, 360);

	// Music toggle
	m_musicToggleText.setFont(f);
	m_musicToggleText.setString(m_game->getMusic() ? "Music: On" : "Music: Off");
	m_musicToggleText.setCharacterSize(30);
	m_musicToggleText.setFillColor(sf::Color::Black);
	m_musicToggleText.setPosition(mx - m_musicToggleText.getGlobalBounds().width / 2, 550);
}

void Scene_Settings::update() {
	m_entityManager.update();
	sRender();
}

void Scene_Settings::onEnd() {
	m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game), true);
}


void Scene_Settings::sDoAction(const Action& action) {
	if (action.type() == "START") {
		auto& clickSound = m_game->getAssets().getSound("clickSound");
		auto& enterSound = m_game->getAssets().getSound("enterSound");
		if (action.name() == "PAUSE") { setPaused(!m_paused); }
		else if (action.name() == "QUIT") { onEnd(); }
		else if (action.name() == "ENTER") { 
			std::cout << "ENTER PRESSED" << std::endl;
			if (m_selectedMenuIndex == 1) { // Toggle music on/off
				enterSound.setVolume(m_game->getVolume());
				enterSound.play();
				m_game->setMusicState(!m_game->getMusic());
				m_musicToggleText.setString(m_game->getMusic() ? "Music: On" : "Music: Off");
			}
		}
		else if (action.name() == "DOWN") { m_selectedMenuIndex = (m_selectedMenuIndex + 1) % 2;
		clickSound.setVolume(m_game->getVolume());
		clickSound.play();
		}
		else if (action.name() == "UP") { m_selectedMenuIndex = (m_selectedMenuIndex + 1) % 2;
		clickSound.setVolume(m_game->getVolume());
		clickSound.play();
		}
		else if (action.name() == "LEFT" && m_selectedMenuIndex == 0) { // Adjust volume down
			m_game->setVolume(std::max(0.0f, m_game->getVolume() - 10.0f));
			m_volumeText.setString(std::to_string(static_cast<int>(m_game->getVolume())));
			m_volumeSlider.setFillColor(getVolumeColor(m_game->getVolume()));
			updateVolumeSlider();
		}
		else if (action.name() == "RIGHT" && m_selectedMenuIndex == 0) { // Adjust volume up
			m_game->setVolume(std::min(100.0f, m_game->getVolume() + 10.0f));
			m_volumeText.setString(std::to_string(static_cast<int>(m_game->getVolume())));
			m_volumeSlider.setFillColor(getVolumeColor(m_game->getVolume()));
			updateVolumeSlider();
		}
	}
}

void Scene_Settings::sRender() {
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
	m_window.draw(m_texts[2]);
	m_window.draw(m_volumeSlider);
	m_window.draw(m_volumeText);
	m_window.draw(m_musicToggleText);
	m_window.draw(m_controlSchemeText);

	// Display the window
	m_window.display();
}


