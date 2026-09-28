#ifndef CORE_CONVERSATION_H
#define CORE_CONVERSATION_H

#include "message.h"
#include <cstddef>
#include <stdexcept>

class Conversation {
public:
    Conversation();
    ~Conversation();

    // Rule of Five: Copy Semantics
    Conversation(const Conversation& other);
    Conversation& operator=(const Conversation& other);

    // Rule of Five: Move Semantics
    Conversation(Conversation&& other) noexcept;
    Conversation& operator=(Conversation&& other) noexcept;

    // Core functionality
    void append(Message m);
    std::size_t size() const noexcept;
    const Message& at(std::size_t i) const;

    // Iterators for range-based for loops
    const Message* begin() const noexcept;
    const Message* end() const noexcept;

private:
    Message* data_;
    std::size_t size_;
    std::size_t capacity_;
};

#endif // CORE_CONVERSATION_H