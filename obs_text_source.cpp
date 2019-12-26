#include "obs_text_source.h"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>

ObsTextSource::ObsTextSource(std::string path) : _path(path) {

}


void ObsTextSource::UpdateTime(std::chrono::seconds seconds) {
    std::stringstream ss;
    ss << seconds.count() / 60 << ":" << std::setw(2) << std::setfill('0') << seconds.count() % 60;

    WriteMessage(ss.str());
}

void ObsTextSource::WriteMessage(std::string message) {
    std::ofstream textSource(_path, std::ofstream::out);
    textSource << message;
}