#include "ResourceManager.h"
#include "GameExceptions.h"
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>


template <typename T>
ResourceManager<T>& ResourceManager<T>::instance() {
    static ResourceManager<T> _instance;
    return _instance;
}

template <typename T>
const T& ResourceManager<T>::get(const std::string& path) {
    auto it = resources.find(path);
    if (it != resources.end()) {
        return it->second;
    }

    T resource;
    bool success = false;
    
    if constexpr (std::is_same_v<T, sf::Font>) {
            success = resource.openFromFile(path);
    } else {
            success = resource.loadFromFile(path);
    }

    if (!success) {
        if constexpr (std::is_same_v<T, sf::Texture>) {
            throw TextureLoadingException(path);
        } else if constexpr (std::is_same_v<T, sf::SoundBuffer>) {
            throw AudioLoadingException(path);
        } else if constexpr (std::is_same_v<T, sf::Font>) {
            throw FontLoadingException(path);
        } else {
            throw GameException("Failed to load resource: " + path);
        }
    }

    auto [insertedIt, _] = resources.emplace(path, std::move(resource));
    return insertedIt->second;
}

template class ResourceManager<sf::Texture>;
template class ResourceManager<sf::SoundBuffer>;
template class ResourceManager<sf::Font>;
