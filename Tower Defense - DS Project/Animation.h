#pragma once

#include <SFML/Graphics.hpp>
#include "Vec2.h"
#include <string>

class Animation {
	sf::Sprite m_sprite;
	size_t m_frameCount = 1;
	size_t m_currentFrame = 0;
	size_t m_speed = 0;
	Vec2 m_size = { 1,1 };
	std::string m_name = "none";
public:
	Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed);
	Animation(const std::string& name, const sf::Texture& t);
	Animation();

	void update();
	bool hasEnded();
	std::string& getName();
	Vec2& getSize();
	sf::Sprite& getSprite();
};