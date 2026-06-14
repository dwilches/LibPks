
#include "PksColor.h"

#include <iostream>

std::ostream &operator<<(std::ostream &os, const PksColor &color) {
    switch (color) {
        case PksColor::Red: os << "Red";
            break;
        case PksColor::Green: os << "Green";
            break;
        case PksColor::Blue: os << "Blue";
            break;
        case PksColor::Yellow: os << "Yellow";
            break;
        default: os << "Unknown";
            break;
    }
    return os;
}
