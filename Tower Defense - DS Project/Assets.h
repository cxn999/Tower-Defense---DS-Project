#pragma once

#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Animation.h"

typedef std::map<std::string, sf::Texture> Textures;
typedef std::map<std::string, Animation> AnimationsMap;
typedef std::map<std::string, sf::Sound> Sounds;
typedef std::map<std::string, sf::Font> Fonts;


class Assets {
	AnimationsMap m_animations;
	Textures m_textures;
	Sounds m_sounds;
	Fonts m_fonts;
public:
	void addTexture(const std::string& name, const std::string& path);
	void addSound(std::string& name, std::string& path);
	void addFont(const std::string& name, const std::string& path);
	void addAnimation(const std::string& name, const Animation & animation);

	sf::Texture & getTexture(const std::string & name);
	sf::Sound & getSound(std::string& name);
	sf::Font & getFont(const std::string& name);
	Animation & getAnimation(const std::string& name);

	void loadFromFile(const std::string& path);
};