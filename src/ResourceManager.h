#ifndef OOP_RESOURCEMANAGER_H
#define OOP_RESOURCEMANAGER_H

#include <unordered_map>
#include <string>

template <typename T>
class ResourceManager {
private:
    std::unordered_map<std::string, T> resources;

    ResourceManager() = default;

public:
    static ResourceManager<T>& instance();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    const T& get(const std::string& path);
};

#endif // OOP_RESOURCEMANAGER_H