#pragma once

#include <fmt/core.h>

#include <cstdlib>
#include <exception>
#include <functional>
#include <iterator>
#include <utility>

#include "exceptions.hpp"

template <typename T>
class ForwardList {
private:
    class Node {
        friend class ForwardListIterator;
        friend class ForwardList;

    private:
        T data_;
        Node* next_;

        explicit Node(const T& value, Node* next_node = nullptr) {
            data_ = value;
            next_ = next_node;
        }
    };

public:
    class ForwardListIterator {
        friend class ForwardList;

    public:
        // NOLINTNEXTLINE
        using iterator_category = std::forward_iterator_tag;
        // NOLINTNEXTLINE
        using value_type = T;
        // NOLINTNEXTLINE
        using difference_type = std::ptrdiff_t;
        // NOLINTNEXTLINE
        using pointer = T*;
        // NOLINTNEXTLINE
        using reference = T&;

        explicit ForwardListIterator(Node* node) {
            current_ = node;
        }

        T& operator*() const {
            return current_->data_;
        }

        ForwardListIterator& operator++() {
            if (current_) {
                current_ = current_->next_;
            }
            return *this;
        }

        ForwardListIterator operator++(int) {
            ForwardListIterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const ForwardListIterator& other) const {
            return current_ == other.current_;
        }

        bool operator!=(const ForwardListIterator& other) const {
            return current_ != other.current_;
        }

    private:
        Node* current_;
    };

public:
    ForwardList() {
        head_ = nullptr;
        size_ = 0;
    }

    explicit ForwardList(size_t sz) {
        head_ = nullptr;
        size_ = 0;
        for (size_t i = 0; i < sz; ++i) {
            PushFront(T());
        }
    }

    ForwardList(const std::initializer_list<T>& values) {
        head_ = nullptr;
        size_ = 0;
        for (auto it = std::rbegin(values); it != std::rend(values); ++it) {
            PushFront(*it);
        }
    }

    ForwardList(const ForwardList& other) {
        head_ = nullptr;
        size_ = 0;
        Node* cur = other.head_;
        Node* prev = nullptr;

        while (cur) {
            Node* new_node = new Node(cur->data_);
            if (!head_) {
                head_ = new_node;
            }
            if (prev) {
                prev->next_ = new_node;
            }
            prev = new_node;
            cur = cur->next_;
        }
        size_ = other.size_;
    }

    ForwardList& operator=(const ForwardList& other) {
        if (this == &other) {
            return *this;
        }
        Clear();
        Node* cur = other.head_;
        Node* prev = nullptr;

        while (cur) {
            Node* new_node = new Node(cur->data_);
            if (!head_) {
                head_ = new_node;
            }
            if (prev) {
                prev->next_ = new_node;
            }
            prev = new_node;
            cur = cur->next_;
        }
        size_ = other.size_;
        return *this;
    }

    ForwardListIterator Begin() const noexcept {
        return ForwardListIterator(head_);
    }
    ForwardListIterator End() const noexcept {
        return ForwardListIterator(nullptr);
    }

    inline T& Front() const {
        return head_->data_;
    }
    inline bool IsEmpty() const noexcept {
        return head_ == nullptr;
    }
    inline size_t Size() const noexcept {
        return size_;
    }

    void Swap(ForwardList& other) {
        std::swap(head_, other.head_);
        std::swap(size_, other.size_);
    }

    void EraseAfter(ForwardListIterator pos) {
        if (!head_) {
            throw ListIsEmptyException("List is empty");
        }
        if (!pos.current_ || !pos.current_->next_) {
            return;
        }
        Node* temp = pos.current_->next_;
        pos.current_->next_ = temp->next_;
        delete temp;
        --size_;
    }

    void InsertAfter(ForwardListIterator pos, const T& value) {
        if (!pos.current_) {
            return;
        }
        Node* new_node = new Node(value, pos.current_->next_);
        pos.current_->next_ = new_node;
        ++size_;
    }

    ForwardListIterator Find(const T& value) const {
        Node* cur = head_;
        while (cur) {
            if (cur->data_ == value) {
                return ForwardListIterator(cur);
            }
            cur = cur->next_;
        }
        return End();
    }

    void Clear() noexcept {
        while (head_) {
            PopFront();
        }
    }

    void PushFront(const T& value) {
        head_ = new Node(value, head_);
        ++size_;
    }

    void PopFront() {
        if (!head_) {
            throw ListIsEmptyException("List is empty");
        }
        // Node -> begin
        Node* temp = head_;
        head_ = head_->next_;
        delete temp;
        --size_;
    }

    ~ForwardList() {
        Clear();
    }

private:
    Node* head_;
    size_t size_;
};

namespace std {
template <typename T>
// NOLINTNEXTLINE
void swap(ForwardList<T>& a, ForwardList<T>& b) {
    a.Swap(b);
}
}  // namespace std
