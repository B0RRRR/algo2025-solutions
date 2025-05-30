#pragma once

#include <fmt/core.h>

#include <cstddef>
#include <cstdlib>
#include <exception>
#include <functional>
#include <iterator>
#include <string>
#include <utility>

#include "exceptions.hpp"

template <typename T>
class List {
private:
    class Node {
        friend class ListIterator;
        friend class List;

        T data_;
        Node* prev_;
        Node* next_;
        // fix
        explicit Node(const T& value, Node* p = nullptr, Node* n = nullptr) : data_(value), prev_(p), next_(n) {
        }
    };

public:
    class ListIterator {
    public:
        // NOLINTNEXTLINE
        using value_type = T;
        // NOLINTNEXTLINE
        using reference_type = value_type&;
        // NOLINTNEXTLINE
        using pointer_type = value_type*;
        // NOLINTNEXTLINE
        using difference_type = std::ptrdiff_t;
        // NOLINTNEXTLINE
        using iterator_category = std::bidirectional_iterator_tag;

        inline bool operator==(const ListIterator& other) const {
            return current_ == other.current_;
        }

        inline bool operator!=(const ListIterator& other) const {
            return current_ != other.current_;
        }

        inline reference_type operator*() const {
            if (!current_) {
                throw std::runtime_error("Dereferencing end iterator");
            }
            return current_->data_;
        }

        inline pointer_type operator->() const {
            return &current_->data_;
        }

        ListIterator& operator++() {
            if (current_) {
                current_ = current_->next_;
            }
            return *this;
        }

        ListIterator operator++(int) {
            ListIterator temp = *this;
            ++(*this);
            return temp;
        }

        ListIterator& operator--() {
            if (current_ == nullptr) {
                current_ = list_->tail_;
            } else {
                current_ = current_->prev_;
            }
            return *this;
        }

        ListIterator operator--(int) {
            ListIterator temp = *this;
            --(*this);
            return temp;
        }

        friend class List<T>;

    private:
        Node* current_;
        const List* list_;

        explicit ListIterator(Node* node, const List* list) : current_(node), list_(list) {
        }
    };

public:
    List() : head_(nullptr), tail_(nullptr), size_(0) {
    }

    explicit List(size_t sz) : List() {
        for (size_t i = 0; i < sz; ++i) {
            PushBack(T());
        }
    }

    List(const std::initializer_list<T>& values) : List() {
        for (const auto& value : values) {
            PushBack(value);
        }
    }

    List(const List& other) : List() {
        for (Node* curr = other.head_; curr != nullptr; curr = curr->next_) {
            PushBack(curr->data_);
        }
    }

    List& operator=(const List& other) {
        if (this != &other) {
            Clear();
            for (Node* curr = other.head_; curr != nullptr; curr = curr->next_) {
                PushBack(curr->data_);
            }
        }
        return *this;
    }

    ListIterator Begin() const noexcept {
        return ListIterator(head_, this);
    }

    ListIterator End() const noexcept {
        return ListIterator(nullptr, this);
    }

    inline T& Front() const {
        if (IsEmpty()) {
            throw ListIsEmptyException("List is empty");
        }
        return head_->data_;
    }

    inline T& Back() const {
        if (IsEmpty()) {
            throw ListIsEmptyException("List is empty");
        }
        return tail_->data_;
    }

    inline bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    inline size_t Size() const noexcept {
        return size_;
    }

    void Swap(List& a) {
        std::swap(head_, a.head_);
        std::swap(tail_, a.tail_);
        std::swap(size_, a.size_);
    }

    void PushFront(const T& value) {
        Node* new_node = new Node(value, nullptr, head_);
        if (IsEmpty()) {
            head_ = tail_ = new_node;
        } else {
            head_->prev_ = new_node;
            head_ = new_node;
        }
        ++size_;
    }

    void PushBack(const T& value) {
        Node* new_node = new Node(value, tail_, nullptr);
        if (IsEmpty()) {
            head_ = tail_ = new_node;
        } else {
            tail_->next_ = new_node;
            tail_ = new_node;
        }
        ++size_;
    }

    void PopBack() {
        if (IsEmpty()) {
            throw ListIsEmptyException("Cannot pop from an empty list");
        }
        Node* temp = tail_;
        tail_ = tail_->prev_;
        if (tail_) {
            tail_->next_ = nullptr;
        } else {
            head_ = nullptr;
        }
        delete temp;
        --size_;
    }

    void PopFront() {
        if (IsEmpty()) {
            throw ListIsEmptyException("Cannot pop from an empty list");
        }
        Node* temp = head_;
        head_ = head_->next_;
        if (head_) {
            head_->prev_ = nullptr;
        } else {
            tail_ = nullptr;
        }
        delete temp;
        --size_;
    }

    ListIterator Find(const T& value) const {
        for (Node* curr = head_; curr != nullptr; curr = curr->next_) {
            if (curr->data_ == value) {
                return ListIterator(curr, this);
            }
        }
        return End();
    }

    void Erase(ListIterator pos) {
        if (!pos.current_) {
            return;
        }

        Node* node = pos.current_;
        if (node->prev_) {
            node->prev_->next_ = node->next_;
        }
        if (node->next_) {
            node->next_->prev_ = node->prev_;
        }
        if (node == head_) {
            head_ = node->next_;
        }
        if (node == tail_) {
            tail_ = node->prev_;
        }

        delete node;
        --size_;
    }

    void Insert(ListIterator pos, const T& value) {
        if (!pos.current_) {
            PushBack(value);
            return;
        }

        Node* new_node = new Node(value, pos.current_->prev_, pos.current_);
        if (pos.current_->prev_) {
            pos.current_->prev_->next_ = new_node;
        }
        pos.current_->prev_ = new_node;

        if (pos.current_ == head_) {
            head_ = new_node;
        }
        ++size_;
    }

    void Clear() noexcept {
        while (!IsEmpty()) {
            PopFront();
        }
    }

    ~List() {
        Clear();
    }

private:
    Node* head_;
    Node* tail_;
    size_t size_;
};

namespace std {
template <typename T>
// NOLINTNEXTLINE
void swap(List<T>& a, List<T>& b) {
    a.Swap(b);
}
}  // namespace std