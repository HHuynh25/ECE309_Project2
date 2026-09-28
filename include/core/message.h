#ifndef CORE_MESSAGE_H
#define CORE_MESSAGE_H

#include <string>

enum class Role { System, User, Assistant };

class Message {
public:
    // Default constructor: empty System message with empty content
    Message() : role_(Role::System), content_("") {}

    // Parameterized constructor
    Message(Role role, std::string content)
        : role_(role), content_(std::move(content)) {}

    // Getters
    Role role() const noexcept { return role_; }
    const std::string& content() const noexcept { return content_; }

private:
    Role role_;
    std::string content_;
};

#endif // CORE_MESSAGE_H