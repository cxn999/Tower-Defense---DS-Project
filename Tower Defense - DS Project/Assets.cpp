#include "Assets.h"
#include <fstream>
#include <iostream>

void Assets::addTexture(const std::string& name, const std::string& path) {
    sf::Texture texture;
    if (texture.loadFromFile(path)) {
        m_textures[name] = texture;
        std::cout << "Loaded texture: " << name << std::endl;
    }
    else {
        std::cerr << "Error loading texture from: " << path << std::endl;
    }
}

void Assets::addSound(std::string& name, std::string& path) {

}

void Assets::addFont(const std::string& name, const std::string& path) {
	sf::Font f;
    if (f.loadFromFile(path)) {
        m_fonts[name] = f;
        std::cout << "Loaded font: " << name << std::endl;
    }
    else {
        std::cerr << "Error loading font from: " << path << std::endl;
    }
}

void Assets::addAnimation(const std::string& name, const Animation& animation) {
	m_animations[name] = animation;
}


sf::Texture& Assets::getTexture(const std::string& name) {
    auto it = m_textures.find(name);
    if (it != m_textures.end()) {
        std::cout << "Retrieved texture: " << name << std::endl;
        return it->second;
    }
    else {
        std::cerr << "Texture not found: " << name << std::endl;
        throw std::out_of_range("Texture not found: " + name);
    }
}

sf::Sound& Assets::getSound(std::string& name) {
	return m_sounds[name];
}

sf::Font& Assets::getFont(const std::string& name) {
	return m_fonts[name];
}

Animation& Assets::getAnimation(const std::string& name) {
	return m_animations[name];
}

void Assets::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    std::string identifier;

    if (!file.is_open()) {
        std::cerr << "Error opening file...\n";
        exit(EXIT_FAILURE);
    }

    while (file >> identifier) {
        std::string name;
        std::string filePath;
        if (identifier == "Texture") {
            file >> name >> filePath;
            addTexture(name, filePath); // Use addTexture directly
        }
        else if (identifier == "Font") {
            file >> name >> filePath;
            addFont(name, filePath); // Use addFont method
        }
        else if (identifier == "Animation") {
            std::string textureName;
            int frameCount, animDuration;
            bool repeat;
            file >> name >> textureName >> frameCount >> animDuration >> repeat;
            addAnimation(name, Animation(name, getTexture(textureName), frameCount, animDuration));
        }
    }
}