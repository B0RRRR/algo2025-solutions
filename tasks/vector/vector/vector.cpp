#include "vector.hpp"

#include <cstdlib>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

template <typename T>
Vector<T>::Vector() {
    data_ = nullptr;
    size_ = 0;
    capacity_ = 0;
}

template <typename T>
Vector<T>::Vector(size_t count, const T& value) {
    data_ = static_cast<T*>(operator new[](count * sizeof(T)));
    size_ = count;
    capacity_ = count;
    for (size_t i = 0; i < count; ++i) {
        new (data_ + i) T(value);
    }
}

template <typename T>
Vector<T>::Vector(const Vector& other) {
    data_ = nullptr;
    size_ = 0;
    capacity_ = 0;
    if (other.size_ > 0) {
        data_ = static_cast<T*>(operator new[](other.capacity_ * sizeof(T)));
        if constexpr (std::is_copy_constructible_v<T>) {
            for (size_t i = 0; i < other.size_; ++i) {
                new (data_ + i) T(other.data_[i]);
            }
        } else if constexpr (std::is_move_constructible_v<T>) {
            for (size_t i = 0; i < other.size_; ++i) {
                new (data_ + i) T(std::move(other.data_[i]));
            }
        }
        size_ = other.size_;
        capacity_ = other.capacity_;
    }
}

template <typename T>
Vector<T>& Vector<T>::operator=(const Vector& other) {
    if (this != &other) {
        Clear();
        if (data_ != nullptr) {
            operator delete[](data_);
            data_ = nullptr;
        }
        size_ = 0;
        capacity_ = 0;
        if (other.size_ > 0) {
            data_ = static_cast<T*>(operator new[](other.capacity_ * sizeof(T)));
            if constexpr (std::is_copy_constructible_v<T>) {
                for (size_t i = 0; i < other.size_; ++i) {
                    new (data_ + i) T(other.data_[i]);
                }
            } else if constexpr (std::is_move_constructible_v<T>) {
                for (size_t i = 0; i < other.size_; ++i) {
                    new (data_ + i) T(std::move(other.data_[i]));
                }
            }
            size_ = other.size_;
            capacity_ = other.capacity_;
        }
    }
    return *this;
}

template <typename T>
Vector<T>& Vector<T>::operator=(Vector&& other) {
    if (this != &other) {
        Clear();
        if (data_ != nullptr) {
            operator delete[](data_);
        }
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    return *this;
}

template <typename T>
Vector<T>::Vector(Vector&& other) noexcept : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}

template <typename T>
Vector<T>::Vector(std::initializer_list<T> init) : data_(nullptr), size_(0), capacity_(0) {
    Reserve(init.size() + 1);
    for (const auto& elem : init) {
        new (data_ + size_) T(elem);
        ++size_;
    }
}

template <typename T>
T& Vector<T>::operator[](size_t pos) {
    return data_[pos];
}

template <typename T>
T& Vector<T>::Front() const noexcept {
    return (data_[0]);
}

template <typename T>
bool Vector<T>::IsEmpty() const noexcept {
    return size_ == 0;
}

template <typename T>
T& Vector<T>::Back() const noexcept {
    return (data_[size_ - 1]);
}

template <typename T>
T* Vector<T>::Data() const noexcept {
    return data_;
}

template <typename T>
size_t Vector<T>::Size() const noexcept {
    return size_;
}

template <typename T>
size_t Vector<T>::Capacity() const noexcept {
    return capacity_;
}

template <typename T>
void Vector<T>::Reserve(size_t new_cap) {
    if (new_cap > capacity_) {
        T* new_data = static_cast<T*>(operator new[](new_cap * sizeof(T)));
        if constexpr (std::is_move_constructible_v<T>) {
            for (size_t i = 0; i < size_; ++i) {
                new (new_data + i) T(std::move(data_[i]));
            }
        } else if constexpr (std::is_copy_constructible_v<T>) {
            for (size_t i = 0; i < size_; ++i) {
                new (new_data + i) T(data_[i]);
            }
        }
        if constexpr (!std::is_same_v<T, void*>) {
            for (size_t i = 0; i < size_; ++i) {
                data_[i].~T();
            }
        }
        operator delete[](data_);
        data_ = new_data;
        capacity_ = new_cap;
    }
}

template <typename T>
void Vector<T>::Clear() noexcept {
    if constexpr (std::is_same_v<T, void*>) {
        for (size_t i = 0; i < size_; ++i) {
            free(data_[i]);
        }
    } else {
        for (size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
    }
    size_ = 0;
}

template <typename T>
void Vector<T>::Insert(size_t pos, T value) {
    if (size_ == capacity_) {
        Reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    for (size_t i = size_; i > pos; --i) {
        new (data_ + i) T(std::move(data_[i - 1]));
        data_[i - 1].~T();
    }
    new (data_ + pos) T(std::move(value));
    ++size_;
}

template <typename T>
void Vector<T>::Erase(size_t begin_pos, size_t end_pos) {
    if (begin_pos >= size_ || begin_pos >= end_pos) {
        return;
    }
    size_t real_end_pos = std::min(end_pos, size_);
    size_t range = real_end_pos - begin_pos;

    if constexpr (std::is_same_v<T, void*>) {
        for (size_t i = begin_pos; i < real_end_pos; ++i) {
            free(data_[i]);
        }
    } else {
        for (size_t i = begin_pos; i < real_end_pos; ++i) {
            data_[i].~T();
        }
    }
    for (size_t i = real_end_pos; i < size_; ++i) {
        new (data_ + i - range) T(std::move(data_[i]));
        if constexpr (!std::is_same_v<T, void*>) {
            data_[i].~T();
        }
    }
    size_ -= range;
}

template <typename T>
void Vector<T>::PushBack(T value) {
    if (size_ == capacity_) {
        Reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    new (data_ + size_) T(std::move(value));
    ++size_;
}

template <typename T>
template <class... Args>
void Vector<T>::EmplaceBack(Args&&... args) {
    if (size_ == capacity_) {
        Reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    new (data_ + size_) T(std::forward<Args>(args)...);
    ++size_;
}

template <typename T>
void Vector<T>::PopBack() {
    if (size_ > 0) {
        data_[size_ - 1].~T();
        --size_;
    }
}

template <typename T>
void Vector<T>::Resize(size_t count, const T& value) {
    if (count < size_) {
        if constexpr (std::is_same_v<T, void*>) {
            for (size_t i = count; i < size_; ++i) {
                free(data_[i]);
            }
        } else {
            for (size_t i = count; i < size_; ++i) {
                data_[i].~T();
            }
        }
    } else if (count > size_) {
        Reserve(count);
        for (size_t i = size_; i < count; ++i) {
            new (data_ + i) T(value);
        }
    }
    size_ = count;
}

template <typename T>
Vector<T>::~Vector() {
    Clear();
    if (data_ != nullptr) {
        operator delete[](data_);
        data_ = nullptr;
    }
}
