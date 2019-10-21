#ifndef _CHIPS_H
#define _CHIPS_H

#include <dlfcn.h>

#include <memory>
#include <string>

#include "chips/chip_interface.h"

struct ChipContainer {
    struct _FreeChip {
        void operator()(void *hndl);
    };

    using handlePtr = std::unique_ptr<void, _FreeChip>;

    const Chip *chip;
    handlePtr handle;
};

ChipContainer LoadChip(const std::string &file);

#endif // _CHIPS_H

