#include "Animation.h"
#include <iostream>

Animation::Animation() = default;

Animation::Animation(const std::string& name, const sf::Texture& t)
	: Animation(name, t, 1, 0)
{}

Animation::Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed)
	: m_name(name)
	, m_sprite(t)
	, m_frameCount(frameCount)
	, m_currentFrame(0)
	, m_speed(speed)
{
	m_size = Vec2((float)(t.getSize().x) / frameCount, (float)t.getSize().y);
	m_sprite.setOrigin(m_size.x / 2.f, m_size.y / 2.f);
	m_sprite.setTextureRect(sf::IntRect(floor(m_currentFrame) * m_size.x, 0, m_size.x, m_size.y));
}

// updates the animation to show the next frame, depending on it's speed
// animation loops when it reaches the end
void Animation::update() {
	// add the speed variable to the current frame
	m_currentFrame++;
	if (m_speed > 0) {
		m_sprite.setTextureRect(sf::IntRect(floor(m_currentFrame/m_speed % m_frameCount) * m_size.x, 0, m_size.x, m_size.y));
	}
}

bool Animation::hasEnded() {
	return (m_frameCount-1 == m_currentFrame/m_speed%m_frameCount);
}
std::string& Animation::getName() {
	return m_name;
}

Vec2& Animation::getSize() {
	return m_size;
}

sf::Sprite& Animation::getSprite() {
	return m_sprite;
}