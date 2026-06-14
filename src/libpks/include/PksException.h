//
// Created by dwilches on 6/11/26.
//

#ifndef LIBPKS_PKSEXCEPTION_H
#define LIBPKS_PKSEXCEPTION_H

#include <exception>
#include <string>


class PksException : public std::exception {
    std::string message;

public:
    PksException(const std::string &message);

    [[nodiscard]] const char *what() const noexcept override;
};


#endif //LIBPKS_PKSEXCEPTION_H
