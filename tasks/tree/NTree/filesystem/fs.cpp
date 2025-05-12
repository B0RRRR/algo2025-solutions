#include "fs.hpp"

#include <iostream>
#include <sstream>

#include "./detail/exceptions.hpp"
#include "./files/directory.hpp"

using filesystem::exceptions::FileNotFoundException;

namespace filesystem {

Fs::Fs() {
    root_ = new Directory();
    current_ = root_;
}

Fs::~Fs() {
    std::function<void(Directory*)> destroy = [&](Directory* dir) {
        for (const auto& [_, child] : dir->childs_.Values()) {
            destroy(child);
        }
        delete dir;
    };

    // fix
    destroy(root_);
}

void Fs::ChangeDir(const std::string& path) {
    Directory* target = nullptr;

    if (path.empty()) {
        return;
    }

    if (path[0] == '/') {
        target = root_;
    } else {
        target = current_;
    }

    auto parts = Split(path, "/");
    for (const auto& part : parts) {
        if (part == "." || part.empty()) {
            continue;
        } else if (part == "..") {
            if (target->parent_ != nullptr) {
                target = target->parent_;
            }
        } else {
            if (!target->childs_.Find(part)) {
                throw FileNotFoundException("Directory not found: " + part);
            }
            target = target->childs_[part];
        }
    }

    current_ = target;
}

void Fs::ListFiles(const std::string& path) const {
    Directory* dir = nullptr;

    if (path.empty() || path == ".") {
        dir = current_;
    } else {
        dir = (path[0] == '/') ? root_ : current_;
        auto parts = Split(path, "/");

        for (const auto& part : parts) {
            if (part.empty() || part == ".") {
                continue;
            }
            if (part == "..") {
                if (dir->parent_) {
                    dir = dir->parent_;
                }
            } else {
                if (!dir->childs_.Find(part)) {
                    throw FileNotFoundException("Directory not found: " + part);
                }
                dir = dir->childs_[part];
            }
        }
    }
}

void Fs::MakeDir(const std::string& path, bool is_create_parents) {
    if (path.empty()) {
        return;
    }

    Directory* dir = (path[0] == '/') ? root_ : current_;
    auto parts = Split(path, "/");

    for (size_t i = 0; i < parts.size(); ++i) {
        const std::string& part = parts[i];
        if (part.empty() || part == ".") {
            continue;
        }

        if (part == "..") {
            if (dir->parent_ != nullptr) {
                dir = dir->parent_;
            }
            continue;
        }

        if (dir->childs_.Find(part)) {
            dir = dir->childs_[part];
        } else {
            if (!is_create_parents && i != parts.size() - 1) {
                throw FileNotFoundException("Parent directory not found: " + part);
            }

            auto* new_dir = new Directory();
            new_dir->parent_ = dir;
            dir->childs_.Insert({part, new_dir});
            dir = new_dir;
        }
    }

}

auto Fs::Split(const std::string& str, const std::string& splitter) const -> std::vector<std::string> {
    std::vector<std::string> result;
    size_t pos = 0;
    size_t end = 0;
    while ((end = str.find(splitter, pos)) != std::string::npos) {
        if (end != pos) {
            result.push_back(str.substr(pos, end - pos));
        }
        pos = end + splitter.length();
    }
    if (pos < str.size()) {
        result.push_back(str.substr(pos));
    }
    return result;
}

void Fs::RemoveFile(const std::string& path) {
    if (path.empty()) {
        return;
    }

    auto parts = Split(path, "/");
    if (parts.empty()) {
        return;
    }

    std::string target_name = parts.back();
    parts.pop_back();

    Directory* dir = (path[0] == '/') ? root_ : current_;

    for (const auto& part : parts) {
        if (part.empty() || part == ".") {
            continue;
        }
        if (part == "..") {
            if (dir->parent_) {
                dir = dir->parent_;
            }
        } else {
            if (!dir->childs_.Find(part)) {
                throw FileNotFoundException("Directory not found: " + part);
            }
            dir = dir->childs_[part];
        }
    }

    if (dir->files_.Find(target_name)) {
        dir->files_.Erase(target_name);
    } else if (dir->childs_.Find(target_name)) {
        auto* sub = dir->childs_[target_name];
        std::vector<std::string> subnames;
        for (const auto& [name, _] : sub->childs_.Values()) {
            subnames.push_back(name);
        }
        for (const auto& child : subnames) {
            sub->childs_.Erase(child);
        }

        std::vector<std::string> filenames;
        for (const auto& [name, _] : sub->files_.Values()) {
            filenames.push_back(name);
        }
        for (const auto& f : filenames) {
            sub->files_.Erase(f);
        }

        dir->childs_.Erase(target_name);
        delete sub;
    } else {
        throw FileNotFoundException("No file or directory found with name: " + target_name);
    }
}

void Fs::CreateFile(const std::string& path, bool is_overwrite) {
    if (path.empty()) {
        return;
    }

    auto parts = Split(path, "/");
    if (parts.empty()) {
        return;
    }

    std::string filename = parts.back();
    parts.pop_back();

    Directory* dir = (path[0] == '/') ? root_ : current_;

    for (const auto& part : parts) {
        if (part == "." || part.empty()) {
            continue;
        }

        if (part == "..") {
            if (dir->parent_ != nullptr) {
                dir = dir->parent_;
            }
        } else {
            if (!dir->childs_.Find(part)) {
                throw FileNotFoundException("Directory not found: " + part);
            }
            dir = dir->childs_[part];
        }
    }

    if (dir->files_.Find(filename)) {
        if (!is_overwrite) {
            throw FileNotFoundException("File already exists: " + filename);
        }
        dir->files_[filename] = File();
    } else {
        dir->files_.Insert({filename, File()});
    }
}

void Fs::WriteToFile(const std::string& path, bool is_overwrite, std::ostringstream& stream) {

    if (path.empty()) {
        return;
    }

    auto parts = Split(path, "/");
    if (parts.empty()) {
        return;
    }


    std::string filename = parts.back();

    parts.pop_back();

    Directory* dir = (path[0] == '/') ? root_ : current_;

    for (const auto& part : parts) {
        if (part.empty() || part == ".") {
            continue;
        }
        if (part == "..") {
            if (dir->parent_ != nullptr) {

                dir = dir->parent_;
            }
        } else {
            if (!dir->childs_.Find(part)) {
                throw FileNotFoundException("Directory not found: " + part);
            }
            dir = dir->childs_[part];
        }
    }

    if (!dir->files_.Find(filename)) {
        throw FileNotFoundException("File not found: " + filename);
    }

    std::string data = stream.str();
    if (is_overwrite) {
        dir->files_[filename].content_ = data;
    } else {
        dir->files_[filename].content_ += data;
    }
}

void Fs::ShowFileContent(const std::string& path) {

    if (path.empty()) {
        return;
    }

    auto parts = Split(path, "/");
    if (parts.empty()) {
        return;
    }

    std::string filename = parts.back();
    parts.pop_back();

    Directory* dir = (path[0] == '/') ? root_ : current_;

    for (const auto& part : parts) {
        if (part.empty() || part == ".") {
            continue;
        }
        if (part == "..") {
            if (dir->parent_) {

                dir = dir->parent_;
            }
        } else {
            if (!dir->childs_.Find(part)) {
                throw FileNotFoundException("Directory not found: " + part);
            }
            dir = dir->childs_[part];
        }
    }


    if (!dir->files_.Find(filename)) {
        throw FileNotFoundException("File not found: " + filename);
    }
}


void Fs::FindFile(const std::string& filename) {
    bool found = false;

    std::function<void(Directory*, std::string)> dfs = [&](Directory* dir, std::string path_prefix) {
        for (const auto& [name, file] : dir->files_.Values()) {
            if (name == filename) {
                found = true;
            }
        }
        for (const auto& [name, child] : dir->childs_.Values()) {
            dfs(child, path_prefix + "/" + name);
        }
    };

    dfs(root_, "");

    if (!found) {
        throw filesystem::exceptions::FileNotFoundException("File not found: " + filename);
        ;
    }
}

void Fs::PWD() const {
    std::vector<std::string> path;
    Directory* node = current_;

    while (node && node != root_) {
        path.push_back(node->GetName());
        node = node->parent_;
    }
}

}  // end namespace filesystem