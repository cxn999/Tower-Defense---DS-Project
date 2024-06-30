#pragma once


#include "EntityManager.h"		/// include entity manager header file
#include "Scene.h"			/// include scene header file
#include <map>				/// include map library
#include <memory>			/// include memory library
#include <SFML/Graphics.hpp>	/// include graphics library

/// derived Scene from Scene.h to implement the credits Scene
class Scene_Credits : public Scene {
protected:
	std::string m_title = "Credits to: ";
	sf::Text m_titleText;					/// text for the credits
	std::vector<sf::Text> m_texts;			/// vector for the texts in the scene
	std::vector<std::string> m_menuStrings;	/// vector for the strings 
	sf::Text m_menuText;					/// text for the menu

	/// pure virtual method of the base class
	void init();
	void update();
	void onEnd();
	void sDoAction(const Action& action);


public:

	/// constructor for play scene that receives the pointer to the game engine
	Scene_Credits(GameEngine* gameEngine = nullptr);

	/// virtual method of the base class
	void sRender();
};
