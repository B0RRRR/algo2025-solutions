#pragma once

#include <vector>

#include "../fs.hpp"
#include "../map/map.hpp"
#include "file.hpp"

namespace filesystem {

class Directory {
    friend class Fs;

    std::string GetName() const;

private:
    Directory* parent_;
    Map<std::string, Directory*> childs_;
    Map<std::string, File> files_;
};

}  // end namespace filesystem