#include "directory.hpp"

namespace filesystem {

std::string Directory::GetName() const {
    // fix
    if (!parent_) {
        return "/";
    }

    for (const auto& [name, child_ptr] : parent_->childs_.Values()) {
        if (child_ptr == this) {
            return name;
        }
    }

    return {};
}

}  // end namespace filesystem
