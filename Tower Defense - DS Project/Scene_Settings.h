#pragma once


#include "EntityManager.h"		/// include entity manager header file
#include "Scene.h"			/// include scene header file
#include <map>				/// include map library
#include <memory>			/// include memory library
#include <SFML/Graphics.hpp>	/// include graphics library

/// derived Scene from Scene.h to implement the settings Scene
class Scene_Settings : public Scene {
protected:
	sf::Text m_titleText;					/// text for the settings
	std::vector<sf::Text> m_texts;			/// vector for the texts in the scene
	std::vector<std::string> m_menuStrings;	/// vector for the strings 
	sf::Text m_menuText;					/// text for the menu
	int m_selectedMenuIndex = 0;			/// is the selected menu index, it makes the index in which we are sitting highlighted

	/// New UI elements
	sf::RectangleShape m_volumeSlider;
	sf::Text m_volumeText;
	sf::Text m_musicToggleText;
	sf::Text m_controlSchemeText;

	/// State variables
	float m_volume = 50.0f; /// Initial volume level     deprecated doesnt function anymore
	bool m_musicOn = true; /// Initial music state
	bool m_useWASD = true; /// Initial control scheme

	/// pure virtual method of the base class
	void init();
	void update();
	void onEnd();
	void sDoAction(const Action& action);

	/// method for getting the color of the volume slider depending on the volume
	sf::Color getVolumeColor(float volume);
	/// method for updating the volume slider width
	void updateVolumeSlider();


public:

	/// constructor for play scene that receives the pointer to the game engine
	Scene_Settings(GameEngine* gameEngine = nullptr);

	/// virtual method of the base class
	void sRender();
};
