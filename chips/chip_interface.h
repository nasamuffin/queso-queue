#ifndef _CHIP_INTERFACE_H
#define _CHIP_INTERFACE_H

#include <string_view>
#include <string>

//Conform to this API for Chip modules
// const Chip chip = {...};
struct Chip {
    const std::string_view name;
    const std::string_view desc;
    const std::string_view command;

    std::string(*const chip)(const std::string &);
};

using ChipGet = Chip *(*)();
extern "C" {
    const Chip * get_chip();
}

#define GET_CHIP \
extern "C" {const Chip *get_chip() { \
    return &chip; \
}}

#endif // _CHIP_INTERFACE_H
