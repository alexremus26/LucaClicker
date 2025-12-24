#include "ResourceManager.h"
#include "GameExceptions.h"

ResourceManager& ResourceManager::instance() {
    static ResourceManager instance;
    return instance;
}

const sf::Texture& ResourceManager::getTexture(const std::string& path) {
    auto it = textures.find(path);
    if (it != textures.end())
        return it->second;

    sf::Texture texture;
    if (!texture.loadFromFile(path))
        throw TextureLoadingException(path);

    auto [insertedIt, _] = textures.emplace(path, std::move(texture));
    return insertedIt->second;
}

const sf::Font& ResourceManager::getFont(const std::string& path) {
    auto it = fonts.find(path);
    if (it != fonts.end())
        return it->second;

    sf::Font font;
    if (!font.openFromFile(path))
        throw FontLoadingException(path);

    auto [insertedIt, _] = fonts.emplace(path, std::move(font));
    return insertedIt->second;
}

const sf::SoundBuffer& ResourceManager::getSound(const std::string& path) {
    auto it = sounds.find(path);
    if (it != sounds.end())
        return it->second;

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(path))
        throw AudioLoadingException(path);

    auto [insertedIt, _] = sounds.emplace(path, std::move(buffer));
    return insertedIt->second;
}

void ResourceManager::cleanup() {
    textures.clear();
    fonts.clear();
    sounds.clear();
}
