#ifndef OOP_GAMEEXCEPTIONS_H
#define OOP_GAMEEXCEPTIONS_H

#include <exception>
#include <string>

class GameException : public std::exception {
protected:
    std::string message;

public:
    explicit GameException(std::string msg);
    [[nodiscard]] const char* what() const noexcept override;
};

class FileOpenException : public GameException {
public:
    explicit FileOpenException(const std::string& path);
};

class InvalidFormatException : public GameException {
public:
    explicit InvalidFormatException(const std::string& detail);
};

class SaveStateException : public GameException {
public:
    explicit SaveStateException(const std::string& detail);
};

class InvalidIndexException : public GameException {
public:
    explicit InvalidIndexException(const std::string& detail);
};

#endif // OOP_GAMEEXCEPTIONS_H
