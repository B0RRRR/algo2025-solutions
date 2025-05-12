
#pragma once

#include <sstream>
#include <vector>

#include "./detail/exceptions.hpp"

namespace filesystem {

class Directory;

class Fs {
public:
    Fs();
    ~Fs();
    void ChangeDir(const std::string& path);
    void PWD() const;
    void RemoveFile(const std::string& path);
    void ListFiles(const std::string& path = ".") const;
    void MakeDir(const std::string& path, bool is_create_parents = false);
    void CreateFile(const std::string& path = ".", bool is_overwrite = false);
    void WriteToFile(const std::string& filename, bool is_overwrite, std::ostringstream& stream);
    void ShowFileContent(const std::string& path);
    void FindFile(const std::string& filename);

private:
    std::vector<std::string> Split(const std::string& str, const std::string& splitter) const;

private:
    Directory* root_;
    Directory* current_;
};

}  // end namespace filesystem
