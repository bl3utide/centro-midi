#include "common.hpp"
#include "annotation.hpp"

namespace CentroMidi
{
namespace Annotation
{

// private
std::string text_;
Type type_;
const std::map<Message, std::string> MESSAGES {
    {Message::DisconnectedAlert, "Please set the MIDI connections"},
};

const std::string& getText() noexcept
{
    return text_;
}

Type getType() noexcept
{
    return type_;
}

void setText(const Message message, const Type type) noexcept
{
    text_ = MESSAGES.at(message);
    type_ = type;
}

void setText(const std::string& message, const Type type) noexcept
{
    text_ = message;
    type_ = type;
}

void clearText() noexcept
{
    text_.clear();
}

} // Annotation
} // CentroMidi