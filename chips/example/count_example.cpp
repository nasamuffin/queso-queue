#include "chip_interface.h"

static int count = 0;

std::string Counter(const std::string & msg) {
    return msg + ": " + std::to_string(++count) + "\r\n";
}

const Chip chip {
    "Counter",
    "Counts invocations and reports to cout",
    "count",
    Counter
};

GET_CHIP