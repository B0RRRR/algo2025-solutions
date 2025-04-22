#pragma once

#include <fmt/core.h>

#include <cstdlib>
#include <functional>
#include <iterator>
#include <memory>
#include <stack>
#include <utility>
#include <vector>

class EraseNotExistingValue : public std::exception {
public:
    explicit EraseNotExistingValue(const std::string& text) : error_message_(text) {
    }

    const char* what() const noexcept override {
        return error_message_.c_str();
    }

private:
    std::string error_message_;
};

template <typename Key, typename Value, typename Compare = std::less<Key>>
class Map {
    class Node;

public:
    class MapIterator {
    public:
        // NOLINTNEXTLINE
        using value_type = std::pair<const Key, Value>;
        // NOLINTNEXTLINE
        using reference_type = value_type&;
        // NOLINTNEXTLINE
        using pointer_type = value_type*;
        // NOLINTNEXTLINE
        using difference_type = std::ptrdiff_t;
        // NOLINTNEXTLINE
        using iterator_category = std::forward_iterator_tag;

        inline bool operator==(const MapIterator& other) const {
            return current_ == other.current_;
        }

        inline bool operator!=(const MapIterator& other) const {
            return current_ != other.current_;
        }

        inline reference_type operator*() const {
            return current_->data_;
        }

        MapIterator& operator++() {
            if (current_->is_right_thread_) {
                current_ = current_->right_;
            } else {
                current_ = current_->right_;
                while (current_ && !current_->is_left_thread_) {
                    current_ = current_->left_;
                }
            }
            return *this;
        }

        MapIterator operator++(int) {
            MapIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        inline pointer_type operator->() const {
            return &(current_->data_);
        }

    private:
        explicit MapIterator(Node* node) {
            current_ = node;
        }
        friend class Map;
        Node* current_;
    };

    inline MapIterator Begin() const noexcept {
        Node* current = root_;
        if (!current) {
            return End();
        }
        while (current->left_ && !current->is_left_thread_) {
            current = current->left_;
        }
        return MapIterator(current);
    }

    inline MapIterator End() const noexcept {
        return MapIterator(nullptr);
    }

    Map() : root_(nullptr), size_(0) {
    }

    Value& operator[](const Key& key) {
        if (!root_) {
            root_ = new Node(key, Value());
            size_++;
            return root_->data_.second;
        }

        Node* current = root_;
        Node* parent = nullptr;

        while (true) {
            if (comp_(key, current->data_.first)) {
                if (!current->is_left_thread_) {
                    parent = current;
                    current = current->left_;
                } else {
                    Node* new_node = new Node(key, Value());
                    new_node->left_ = current->left_;
                    new_node->right_ = current;
                    current->left_ = new_node;
                    current->is_left_thread_ = false;
                    size_++;
                    return new_node->data_.second;
                }
            } else if (comp_(current->data_.first, key)) {
                if (!current->is_right_thread_) {
                    parent = current;
                    current = current->right_;
                } else {
                    Node* new_node = new Node(key, Value());
                    new_node->right_ = current->right_;
                    new_node->left_ = current;
                    current->right_ = new_node;
                    current->is_right_thread_ = false;
                    size_++;
                    return new_node->data_.second;
                }
            } else {
                return current->data_.second;
            }
        }
    }

    inline bool IsEmpty() const noexcept {
        return size_ == 0;
    }
    inline size_t Size() const noexcept {
        return size_;
    }

    void Swap(Map& other) {
        static_assert(std::is_same<decltype(this->comp_), decltype(other.comp_)>::value);
        std::swap(root_, other.root_);
        std::swap(size_, other.size_);
        std::swap(comp_, other.comp_);
    }

    std::vector<std::pair<const Key, Value>> Values(bool is_increase = true) const noexcept {
        std::vector<std::pair<const Key, Value>> result;
        if (is_increase) {
            for (auto it = Begin(); it != End(); ++it) {
                result.push_back(*it);
            }
        } else {
            std::vector<std::pair<const Key, Value>> temp;
            for (auto it = Begin(); it != End(); ++it) {
                temp.push_back(*it);
            }
            for (auto it = temp.rbegin(); it != temp.rend(); ++it) {
                result.push_back(*it);
            }
        }
        return result;
    }

    void Insert(const std::pair<const Key, Value>& val) {
        (*this)[val.first] = val.second;
    }

    void Insert(const std::initializer_list<std::pair<const Key, Value>>& values) {
        for (const auto& val : values) {
            Insert(val);
        }
    }

    void Erase(const Key& key) {
        Node* current = root_;
        Node* parent = nullptr;

        while (current) {
            if (comp_(key, current->data_.first)) {
                if (!current->is_left_thread_) {
                    parent = current;
                    current = current->left_;
                } else {
                    break;
                }
            } else if (comp_(current->data_.first, key)) {
                if (!current->is_right_thread_) {
                    parent = current;
                    current = current->right_;
                } else {
                    break;
                }
            } else {
                break;
            }
        }

        if (!current || current->data_.first != key) {
            throw EraseNotExistingValue("Key not found in Erase()");
        }

        size_--;

        if (current->is_left_thread_ && current->is_right_thread_) {
            if (!parent) {
                root_ = nullptr;
            } else if (current == parent->left_) {
                parent->left_ = current->left_;
                parent->is_left_thread_ = true;
            } else {
                parent->right_ = current->right_;
                parent->is_right_thread_ = true;
            }
            delete current;
        } else if (!current->is_left_thread_ && current->is_right_thread_) {
            Node* child = current->left_;
            if (!parent) {
                root_ = child;
            } else if (current == parent->left_) {
                parent->left_ = child;
            } else {
                parent->right_ = child;
            }

            Node* pred = child;
            while (!pred->is_right_thread_) {
                pred = pred->right_;
            }
            pred->right_ = current->right_;
            delete current;
        } else if (current->is_left_thread_ && !current->is_right_thread_) {
            Node* child = current->right_;
            if (!parent) {
                root_ = child;
            } else if (current == parent->left_) {
                parent->left_ = child;
            } else {
                parent->right_ = child;
            }

            Node* succ = child;
            while (!succ->is_left_thread_) {
                succ = succ->left_;
            }
            succ->left_ = current->left_;
            delete current;
        } else {
            Node* succ = current->right_;
            Node* succ_parent = current;

            while (!succ->is_left_thread_) {
                succ_parent = succ;
                succ = succ->left_;
            }

            const Key new_key = succ->data_.first;
            const Value new_value = succ->data_.second;

            if (succ_parent == current) {
                if (succ->is_right_thread_) {
                    current->right_ = succ->right_;
                    current->is_right_thread_ = true;
                } else {
                    current->right_ = succ->right_;
                }
            } else {
                if (succ->is_right_thread_) {
                    succ_parent->left_ = succ->right_;
                    succ_parent->is_left_thread_ = true;
                } else {
                    succ_parent->left_ = succ->right_;
                }
            }

            current->data_.~pair<const Key, Value>();
            new (&current->data_) std::pair<const Key, Value>(new_key, new_value);

            delete succ;
        }
    }

    void Clear() noexcept {
        while (root_ && !IsEmpty()) {
            Erase(root_->data_.first);
        }
    }

    MapIterator Find(const Key& key) const {
        Node* current = root_;
        while (current) {
            if (comp_(key, current->data_.first)) {
                if (!current->is_left_thread_) {
                    current = current->left_;
                } else {
                    break;
                }
            } else if (comp_(current->data_.first, key)) {
                if (!current->is_right_thread_) {
                    current = current->right_;
                } else {
                    break;
                }
            } else {
                return MapIterator(current);
            }
        }
        return End();
    }

    ~Map() {
        Clear();
    }

private:
    class Node {
        friend class MapIterator;
        friend class Map;

    public:
        Node(const Key& key, const Value& value)
            : data_(std::make_pair(key, value)),
              left_(nullptr),
              right_(nullptr),
              is_left_thread_(true),
              is_right_thread_(true) {
        }

    private:
        std::pair<const Key, Value> data_;
        Node* left_;
        Node* right_;
        bool is_left_thread_;
        bool is_right_thread_;
    };

    Node* root_;
    size_t size_;
    Compare comp_;
};

namespace std {
// Global swap overloading
template <typename Key, typename Value>
// NOLINTNEXTLINE
void swap(Map<Key, Value>& a, Map<Key, Value>& b) {
    a.Swap(b);
}
}  // namespace std
