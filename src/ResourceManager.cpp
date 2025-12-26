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

