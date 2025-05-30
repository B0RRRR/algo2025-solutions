#pragma once

#include <string>

namespace filesystem {

class File {
    friend class Fs;

public:
    void Read(size_t bytes) const;

    // overwrite fix
    void Write();

    void Append();

private:
    std::string content_;
};

}  // end namespace filesystem
