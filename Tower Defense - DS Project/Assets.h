#pragma once

#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Animation.h"

typedef std::map<std::string, sf::Texture> Textures;
typedef std::map<std::string, Animation> AnimationsMap;
typedef std::map<std::string, sf::SoundBuffer> SoundBuffers;
typedef std::map<std::string, sf::Sound> Sounds;
typedef std::map<std::string, sf::Music*> Musics;
typedef std::map<std::string, sf::Font> Fonts;


class Assets {
	AnimationsMap m_animations;
	Textures m_textures;
	SoundBuffers m_soundBuffers;
	Sounds m_sounds;
	Musics m_musics;
	Fonts m_fonts;
public:
	~Assets();


	void addTexture(const std::string& name, const std::string& path);
	void addSoundBuffer(const std::string& name, const std::string& path);
	void addMusic(const std::string& name, const std::string& path);
	void addFont(const std::string& name, const std::string& path);
	void addAnimation(const std::string& name, const Animation & animation);

	sf::Texture & getTexture(const std::string & name);
	sf::SoundBuffer& getSoundBuffer(const std::string& name);
	sf::Music* getMusic(const std::string& name);
	sf::Sound & getSound(const std::string& name);
	sf::Font & getFont(const std::string& name);
	Animation & getAnimation(const std::string& name);

	void loadFromFile(const std::string& path);
};