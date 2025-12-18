#ifndef OOP_ITEMFACTORY_H
#define OOP_ITEMFACTORY_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include "Item.h"

class ItemFactory {
public:
    static ItemFactory& getInstance();

    ItemFactory(const ItemFactory&) = delete;
    ItemFactory& operator=(const ItemFactory&) = delete;

    void registerType(
        const std::string& type,
        std::function<std::unique_ptr<Item>(const std::map<std::string, std::string>&)> createMethod
    );

    std::unique_ptr<Item> create(
        const std::string& type,
        const std::map<std::string, std::string>& config
    );

private:
    ItemFactory() = default;

    std::map<std::string,
        std::function<std::unique_ptr<Item>(const std::map<std::string, std::string>&)>
    > creationMethods;
};

#endif // OOP_ITEMFACTORY_H
