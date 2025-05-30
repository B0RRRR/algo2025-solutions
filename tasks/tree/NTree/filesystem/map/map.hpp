#pragma once

#include <cstdlib>
#include <functional>
#include <utility>
#include <vector>

template <typename Key, typename Value, typename Compare = std::less<Key>>
class Map {
public:
    // fix
    Map() : root_(nullptr), tree_size_(0), comp_(Compare()) {
    }

    Value& operator[](const Key& key) {
        Node** curr = &root_;
        while (*curr) {
            if (comp_(key, (*curr)->key)) {
                curr = &((*curr)->left);
            } else if (comp_((*curr)->key, key)) {
                curr = &((*curr)->right);
            } else {
                return (*curr)->value;
            }
        }

        *curr = new Node(key, Value());
        ++tree_size_;
        return (*curr)->value;
    }

    inline bool IsEmpty() const noexcept {
        return tree_size_ == 0;
    }

    inline size_t Size() const noexcept {
        return tree_size_;
    }

    void Swap(Map& other) {
        std::swap(root_, other.root_);
        std::swap(tree_size_, other.tree_size_);
    }

    std::vector<std::pair<const Key, Value>> Values(bool is_increase = true) const noexcept {
        std::vector<std::pair<const Key, Value>> result;
        InOrder(root_, result, is_increase);
        return result;
    }

    void Insert(const std::pair<const Key, Value>& val) {
        Node** curr = &root_;
        while (*curr) {
            if (comp_(val.first, (*curr)->key)) {
                curr = &((*curr)->left);
            } else if (comp_((*curr)->key, val.first)) {
                curr = &((*curr)->right);
            } else {
                (*curr)->value = val.second;
                return;
            }
        }
        *curr = new Node(val.first, val.second);
        ++tree_size_;
    }

    void Insert(const std::initializer_list<std::pair<const Key, Value>>& values) {
        for (const auto& val : values) {
            Insert(val);
        }
    }

    void Erase(const Key& key) {
        root_ = EraseNode(root_, key);
    }

    void Clear() noexcept {
        ClearTree(root_);
        root_ = nullptr;
        tree_size_ = 0;
    }

    bool Find(const Key& key) const {
        Node* curr = root_;
        while (curr) {
            if (comp_(key, curr->key)) {
                curr = curr->left;
            } else if (comp_(curr->key, key)) {
                curr = curr->right;
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
    struct Node {
        Key key;
        Value value;
        Node* left;
        Node* right;

        Node(const Key& k, const Value& v) : key(k), value(v), left(nullptr), right(nullptr) {
        }
    };

    Node* root_;
    size_t tree_size_;
    Compare comp_;

    void InOrder(Node* node, std::vector<std::pair<const Key, Value>>& result, bool is_increase) const {
        if (!node) {
            return;
        }
        if (is_increase) {
            InOrder(node->left, result, is_increase);
            result.emplace_back(node->key, node->value);
            InOrder(node->right, result, is_increase);
        } else {
            InOrder(node->right, result, is_increase);
            result.emplace_back(node->key, node->value);
            InOrder(node->left, result, is_increase);
        }
    }

    void ClearTree(Node* node) {
        if (!node) {
            return;
        }
        ClearTree(node->left);
        ClearTree(node->right);
        delete node;
    }

    Node* EraseNode(Node* node, const Key& key) {
        if (!node) {
            return nullptr;
        }

        if (comp_(key, node->key)) {
            node->left = EraseNode(node->left, key);
        } else if (comp_(node->key, key)) {
            node->right = EraseNode(node->right, key);
        } else {
            if (!node->left) {
                Node* temp = node->right;
                delete node;
                --tree_size_;
                return temp;
            } else if (!node->right) {
                Node* temp = node->left;
                delete node;
                --tree_size_;
                return temp;
            } else {
                Node* min_node = node->right;
                while (min_node->left) {
                    min_node = min_node->left;
                }
                node->key = min_node->key;
                node->value = min_node->value;
                node->right = EraseNode(node->right, min_node->key);
            }
        }
        return node;
    }
};

namespace std {
template <typename Key, typename Value>
// NOLINTNEXTLINE
void swap(Map<Key, Value>& a, Map<Key, Value>& b) {
    a.Swap(b);
}
}  // namespace std
