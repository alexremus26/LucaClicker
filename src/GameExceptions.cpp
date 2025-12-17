#include "GameExceptions.h"

GameException::GameException(std::string msg)
    : message(std::move(msg)) {}

const char* GameException::what() const noexcept {
    return message.c_str();
}

FileOpenException::FileOpenException(const std::string& path)
    : GameException("Failed to open file: " + path) {}

InvalidFormatException::InvalidFormatException(const std::string& detail)
    : GameException("Invalid data format: " + detail) {}

SaveStateException::SaveStateException(const std::string& detail)
    : GameException("Saved game invalid: " + detail) {}

InvalidIndexException::InvalidIndexException(const std::string& detail)
    : GameException("Invalid index: " + detail) {}
