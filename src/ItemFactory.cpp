#include "ItemFactory.h"
#include <utility>


ItemFactory &ItemFactory::getInstance() {
    static ItemFactory instance;
    return instance;
}

void ItemFactory::registerType(const std::string &type, std::function<std::unique_ptr<Item>(const std::map<std::string, std::string> &)> createMethod) {
    creationMethods[type] = std::move(createMethod);
}

std::unique_ptr<Item> ItemFactory::create(const std::string &type, const std::map<std::string, std::string> &config) {
    auto it = creationMethods.find(type);
    if (it != creationMethods.end()) {
        return it->second(config);
    }
    return nullptr;
}