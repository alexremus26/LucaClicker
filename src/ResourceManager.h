#ifndef OOP_RESOURCEMANAGER_H
#define OOP_RESOURCEMANAGER_H

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <unordered_map>
#include <string>

class ResourceManager {
private:
    ResourceManager() = default;

    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, sf::Font> fonts;
    std::unordered_map<std::string, sf::SoundBuffer> sounds;

public:
    static ResourceManager& instance();

    const sf::Texture& getTexture(const std::string& path);
    const sf::Font& getFont(const std::string& path);
    const sf::SoundBuffer& getSound(const std::string& path);

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;


};

#endif // OOP_RESOURCEMANAGER_H
