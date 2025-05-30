#pragma once

#include <fmt/core.h>

#include <cstdlib>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

class MapIsEmptyException : public std::exception {
public:
    explicit MapIsEmptyException(const std::string& text) : error_message_(text) {
    }

    const char* what() const noexcept override {
        return error_message_.c_str();
    }

private:
    std::string error_message_;
};

template <typename Key, typename Value, typename Compare = std::less<Key>>
class Map {
public:
    Map() {
        root_ = nullptr;
        tree_size_ = 0;
    }

    Value& operator[](const Key& key) {
        Node** cur = &root_;
        while (*cur != nullptr) {
            if (comp_(key, (*cur)->data_.first)) {
                cur = &((*cur)->left_);
            } else if (comp_((*cur)->data_.first, key)) {
                cur = &((*cur)->right_);
            } else {
                return (*cur)->data_.second;
            }
        }

        *cur = new Node;
        (*cur)->left_ = nullptr;
        (*cur)->right_ = nullptr;

        new (std::addressof((*cur)->data_)) std::pair<const Key, Value>(key, Value{});

        ++tree_size_;
        return (*cur)->data_.second;
    }

    inline bool IsEmpty() const noexcept {
        return root_ == nullptr;
    }

    inline size_t Size() const noexcept {
        return tree_size_;
    }

    void Swap(Map& a) {
        Node* tmp_root = root_;
        root_ = a.root_;
        a.root_ = tmp_root;

        size_t tmp_size = tree_size_;
        tree_size_ = a.tree_size_;
        a.tree_size_ = tmp_size;
    }

    std::vector<std::pair<const Key, Value>> Values(bool is_increase = true) const noexcept {
        std::vector<std::pair<const Key, Value>> result;
        Traverse(root_, result, is_increase);
        return result;
    }

    void Insert(const std::pair<const Key, Value>& val) {
        Node** cur = &root_;
        while (*cur != nullptr) {
            if (comp_(val.first, (*cur)->data_.first)) {
                cur = &((*cur)->left_);
            } else if (comp_((*cur)->data_.first, val.first)) {
                cur = &((*cur)->right_);
            } else {
                (*cur)->data_.second = val.second;
                return;
            }
        }
        if (*cur == nullptr) {
            *cur = new Node;
            (*cur)->left_ = nullptr;
            (*cur)->right_ = nullptr;
            new (std::addressof((*cur)->data_)) std::pair<const Key, Value>(val.first, val.second);
            ++tree_size_;
            return;
        }
    }

    void Insert(const std::initializer_list<std::pair<const Key, Value>>& values) {
        const std::pair<const Key, Value>* data = values.begin();
        size_t size = values.size();
        for (size_t i = 0; i < size; ++i) {
            Insert(data[i]);
        }
    }

    void Erase(const Key& key) {
        Node** parent_ptr = &root_;
        Node* current = root_;
        while (current != nullptr) {
            if (comp_(key, current->data_.first)) {
                parent_ptr = &current->left_;
                current = current->left_;
            } else if (comp_(current->data_.first, key)) {
                parent_ptr = &current->right_;
                current = current->right_;
            } else {
                break;
            }
        }

        if (current == nullptr) {
            throw MapIsEmptyException("Value not found");
        }

        if (current->left_ == nullptr && current->right_ == nullptr) {
            *parent_ptr = nullptr;
        } else if (current->left_ == nullptr) {
            *parent_ptr = current->right_;
        } else if (current->right_ == nullptr) {
            *parent_ptr = current->left_;
        } else {
            Node** min_parent_ptr = &current->right_;
            Node* min_node = current->right_;

            while (min_node->left_ != nullptr) {
                min_parent_ptr = &min_node->left_;
                min_node = min_node->left_;
            }

            if (min_parent_ptr != &current->right_) {
                *min_parent_ptr = min_node->right_;
                min_node->right_ = current->right_;
            }
            min_node->left_ = current->left_;
            *parent_ptr = min_node;
        }

        current->data_.~pair<const Key, Value>();
        delete current;
        --tree_size_;
    }

    void Clear() noexcept {
        ClearTree(root_);
        root_ = nullptr;
        tree_size_ = 0;
    }

    bool Find(const Key& key) const {
        Node* cur = root_;
        while (cur != nullptr) {
            if (comp_(key, cur->data_.first)) {
                cur = cur->left_;
            } else if (comp_(cur->data_.first, key)) {
                cur = cur->right_;
            } else {
                return true;
            }
        }
        return false;
    }

    ~Map() {
        Clear();
    }

private:
    class Node {
        friend class Map;
        std::pair<const Key, Value> data_;
        Node* left_;
        Node* right_;
    };

    Node* root_;
    size_t tree_size_;
    Compare comp_;

    void Traverse(Node* node, std::vector<std::pair<const Key, Value>>& result, bool asc) const {
        if (node == nullptr) {
            return;
        }
        if (asc) {
            Traverse(node->left_, result, asc);
            result.push_back(node->data_);
            Traverse(node->right_, result, asc);
        } else {
            Traverse(node->right_, result, asc);
            result.push_back(node->data_);
            Traverse(node->left_, result, asc);
        }
    }

    void ClearTree(Node* node) noexcept {
        if (node == nullptr) {
            return;
        }
        ClearTree(node->left_);
        ClearTree(node->right_);
        node->data_.~pair();
        delete node;
    }
};

namespace std {
// Global swap overloading
template <typename Key, typename Value>
// NOLINTNEXTLINE
void swap(Map<Key, Value>& a, Map<Key, Value>& b) {
    a.Swap(b);
}
}  // namespace std
