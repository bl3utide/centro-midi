#include "common.hpp"
#include "annotation.hpp"

namespace CentroMidi
{
namespace Annotation
{

// private
std::string _text;
Type _type;
const std::map<Message, std::string> MESSAGES {
    {Message::DisconnectedAlert, "Please set the MIDI connections"},
};

const std::string& getText() noexcept
{
    return _text;
}

Type getType() noexcept
{
    return _type;
}

void setText(const Message message, const Type type)
{
    _text = MESSAGES.at(message);
    _type = type;
}

void clearText() noexcept
{
    _text.clear();
}

} // Annotation
} // CentroMidi