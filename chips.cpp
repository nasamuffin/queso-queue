#include "chips.h"

#include <spdlog/spdlog.h>
#include <iostream>

void ChipContainer::_FreeChip::operator()(void *handle) {
    if (handle)
        dlclose(handle);
}

ChipContainer LoadChip(const std::string &file) {
    void *handle = dlopen(file.c_str(), RTLD_NOW | RTLD_GLOBAL);

    if (!handle) {
        spdlog::error("Unable to dlopen chip file: {}", file);
    }

    const Chip *chip = ((ChipGet)dlsym(handle, "get_chip"))();
    return ChipContainer{chip, ChipContainer::handlePtr(handle)};
}
