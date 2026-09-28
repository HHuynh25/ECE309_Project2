#include "core/conversation.h"
#include <utility>

// 1. Constructor
Conversation::Conversation() : data_(nullptr), size_(0), capacity_(0) {}

// 2. Destructor
Conversation::~Conversation() {
    delete[] data_;
}

// 3. Copy Constructor (Deep Copy)
Conversation::Conversation(const Conversation& other) 
    : size_(other.size_), capacity_(other.capacity_) {
    if (capacity_ > 0) {
        data_ = new Message[capacity_];
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    } else {
        data_ = nullptr;
    }
}

// 4. Copy Assignment (Deep Copy with self-assignment check)
Conversation& Conversation::operator=(const Conversation& other) {
    if (this == &other) return *this; // Protect against self-assignment

    // Allocate new memory first (strong exception safety)
    Message* new_data = nullptr;
    if (other.capacity_ > 0) {
        new_data = new Message[other.capacity_];
        for (std::size_t i = 0; i < other.size_; ++i) {
            new_data[i] = other.data_[i];
        }
    }

    delete[] data_; // Free old memory
    data_ = new_data;
    size_ = other.size_;
    capacity_ = other.capacity_;

    return *this;
}

// 5. Move Constructor (Steal pointer)
Conversation::Conversation(Conversation&& other) noexcept 
    : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
    // Zero out the source
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}

// 6. Move Assignment (Clean up and steal)
Conversation& Conversation::operator=(Conversation&& other) noexcept {
    if (this == &other) return *this;

    delete[] data_; // Free our existing memory

    // Steal
    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;

    // Zero out
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;

    return *this;
}

// Array Growth & Insertion
void Conversation::append(Message m) {
    if (size_ == capacity_) {
        // Double the capacity, or set to 1 if currently 0
        std::size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
        Message* new_data = new Message[new_capacity];
        
        // Move old elements into the new array
        for (std::size_t i = 0; i < size_; ++i) {
            new_data[i] = std::move(data_[i]);
        }
        
        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }
    // Add the new message
    data_[size_++] = std::move(m);
}

// Accessors
std::size_t Conversation::size() const noexcept {
    return size_;
}

const Message& Conversation::at(std::size_t i) const {
    if (i >= size_) {
        throw std::out_of_range("Index out of bounds");
    }
    return data_[i];
}

const Message* Conversation::begin() const noexcept {
    return data_;
}

const Message* Conversation::end() const noexcept {
    return data_ + size_;
}