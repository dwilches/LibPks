#include "PksException.h"

PksException::PksException(const std::string &message) : message(message) {
}

const char *PksException::what() const noexcept {
    return message.c_str();
}
