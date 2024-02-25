#include "common.hpp"
#include "annotation.hpp"

namespace CentroMidi
{
namespace Annotation
{

// private
std::string _annotation;
Type _type;
const std::map<Message, std::string> MESSAGES {
    {Message::DisconnectedAlert, "Please set the MIDI connections"},
};

const std::string& getText() noexcept
{
    return _annotation;
}

Type getType() noexcept
{
    return _type;
}

void setText(const Message message, const Type type)
{
    _annotation = MESSAGES.at(message);
    _type = type;
}

void clearText() noexcept
{
    _annotation.clear();
}

} // Annotation
} // CentroMidi