#pragma once

#include <fmt/core.h>
#include <cstdlib>
#include <functional>
#include <utility>
#include <vector>
#include <stdexcept>

template <typename Key, typename Value, typename Compare = std::less<Key>>
class Map {
public:
    Map() {
        root = nullptr;
        tree_size = 0;
    }

    Value& operator[](const Key& key) {
        Node** cur = &root;
        while (*cur != nullptr) {
            if (comp(key, (*cur)->data.first)) {
                cur = &((*cur)->left);
            } else if (comp((*cur)->data.first, key)) {
                cur = &((*cur)->right);
            } else {
                return (*cur)->data.second;
            }
        }
        *cur = new Node;
        (*cur)->data = std::make_pair(key, Value());
        (*cur)->left = nullptr;
        (*cur)->right = nullptr;
        ++tree_size;
        return (*cur)->data.second;
    }

    inline bool IsEmpty() const noexcept {
        return root == nullptr;
    }

    inline size_t Size() const noexcept {
        return tree_size;
    }

    void Swap(Map& a) {
        static_assert(std::is_same<decltype(this->comp), decltype(a.comp)>::value,
                      "The compare function types are different");
    
        Node* tmp_root = root;
        root = a.root;
        a.root = tmp_root;
    
        size_t tmp_size = tree_size;
        tree_size = a.tree_size;
        a.tree_size = tmp_size;
    }

    std::vector<std::pair<const Key, Value>> Values(bool is_increase = true) const noexcept {
        std::vector<std::pair<const Key, Value>> result;
        Traverse(root, result, is_increase);
        return result;
    }

    void Insert(const std::pair<const Key, Value>& val) {
        Node** cur = &root;
        while (*cur != nullptr) {
            if (comp(val.first, (*cur)->data.first)) {
                cur = &((*cur)->left);
            } else if (comp((*cur)->data.first, val.first)) {
                cur = &((*cur)->right);
            } else {
                (*cur)->data.second = val.second;
                return;
            }
        }
        *cur = new Node;
        (*cur)->data = val;
        (*cur)->left = nullptr;
        (*cur)->right = nullptr;
        ++tree_size;
    }

    void Insert(const std::initializer_list<std::pair<const Key, Value>>& values) {
        const std::pair<const Key, Value>* data = values.begin();
        size_t size = values.size();
        for (size_t i = 0; i < size; ++i) {
            Insert(data[i]);
        }
    }

    void Erase(const Key& key) {
        Node** cur = &root;
        while (*cur != nullptr && (*cur)->data.first != key) {
            if (comp(key, (*cur)->data.first)) {
                cur = &((*cur)->left);
            } else {
                cur = &((*cur)->right);
            }
        }

        if (*cur == nullptr) {
            throw std::runtime_error("Key not found");
        }

        Node* target = *cur;

        if (target->left == nullptr && target->right == nullptr) {
            delete target;
            *cur = nullptr;
        } else if (target->left == nullptr) {
            *cur = target->right;
            delete target;
        } else if (target->right == nullptr) {
            *cur = target->left;
            delete target;
        } else {
            Node** succ = &(target->right);
            while ((*succ)->left != nullptr) {
                succ = &((*succ)->left);
            }
            target->data = (*succ)->data;
            Node* temp = *succ;
            *succ = (*succ)->right;
            delete temp;
        }
        --tree_size;
    }

    void Clear() noexcept {
        ClearTree(root);
        root = nullptr;
        tree_size = 0;
    }

    bool Find(const Key& key) const {
        Node* cur = root;
        while (cur != nullptr) {
            if (comp(key, cur->data.first)) {
                cur = cur->left;
            } else if (comp(cur->data.first, key)) {
                cur = cur->right;
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
        private:
        std::pair<const Key, Value> data;
        Node* left;
        Node* right;
    };

    Node* root;
    size_t tree_size;
    private:
    Compare comp;

    void Traverse(Node* node, std::vector<std::pair<const Key, Value>>& result, bool asc) const {
        if (node == nullptr) return;
        if (asc) {
            Traverse(node->left, result, asc);
            result.push_back(node->data);
            Traverse(node->right, result, asc);
        } else {
            Traverse(node->right, result, asc);
            result.push_back(node->data);
            Traverse(node->left, result, asc);
        }
    }

    void ClearTree(Node* node) noexcept {
        if (node == nullptr) return;
        ClearTree(node->left);
        ClearTree(node->right);
        delete node;
    }
};

namespace std {
// Global swap overloading
template <typename Key, typename Value>
void swap(Map<Key, Value>& a, Map<Key, Value>& b) {
    a.Swap(b);
}
}