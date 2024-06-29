#include "Assets.h"
#include <fstream>
#include <iostream>

Assets::~Assets() {
    for (auto& pair : m_musics) {
        delete pair.second;
    }
}

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

void Assets::addSoundBuffer(const std::string& name, const std::string& path) {
    sf::SoundBuffer buffer;
    if (buffer.loadFromFile(path)) {
        m_soundBuffers[name] = buffer;
        m_sounds[name].setBuffer(m_soundBuffers[name]);
        std::cout << "Loaded buffer: " << name << std::endl;
    }
    else {
        std::cerr << "Error loading buffer from: " << path << std::endl;
    }
}

void Assets::addMusic(const std::string& name, const std::string& path) {
    sf::Music* music = new sf::Music();
    if (music->openFromFile(path)) {
        m_musics[name] = music;
        std::cout << "Loaded music: " << name << std::endl;
    }
    else {
        std::cerr << "Error loading music from: " << path << std::endl;
    }
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

sf::SoundBuffer& Assets::getSoundBuffer(const std::string& name) {
    return m_soundBuffers.at(name);
}

sf::Sound& Assets::getSound(const std::string& name) {
    return m_sounds.at(name);
}

sf::Music* Assets::getMusic(const std::string& name) {
    return m_musics.at(name);
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
            file >> name >> textureName >> frameCount >> animDuration;
            addAnimation(name, Animation(name, getTexture(textureName), frameCount, animDuration));
        }
        else if (identifier == "Music") {
            file >> name >> filePath;
            addMusic(name, filePath);
        }

        else if (identifier == "SoundEffects") {
            file >> name >> filePath;
            addSoundBuffer(name, filePath);
        }
    }
}